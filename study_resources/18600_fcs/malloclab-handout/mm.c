/*
 * mm.c
 *
 * Name: Kshitij Khode
 * Andrew ID: kkhode
 *
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*
 * If you want debugging output, uncomment the following.  Be sure not
 * to have debugging enabled in your final submission
 */
// #define DEBUG

#ifdef DEBUG
/* When debugging is enabled, the underlying functions get called */
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_assert(...) assert(__VA_ARGS__)
#define dbg_requires(...) assert(__VA_ARGS__)
#define dbg_ensures(...) assert(__VA_ARGS__)
#define dbg_checkheap(...) mm_checkheap(__VA_ARGS__)
#define dbg_printheap(...) print_heap(__VA_ARGS__)
#else
/* When debugging is disabled, no code gets generated */
#define dbg_printf(...)
#define dbg_assert(...)
#define dbg_requires(...)
#define dbg_ensures(...)
#define dbg_checkheap(...)
#define dbg_printheap(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define memset mem_memset
#define memcpy mem_memcpy
#endif /* def DRIVER */

/* What is the correct alignment? */
#define ALIGNMENT 16
#define SEG_BINS  5

/* Basic constants */
typedef uint64_t word_t;
static const size_t wsize = sizeof(word_t);       // word and header size (bytes)
static const size_t dsize = 2*wsize;              // double word size (bytes)
static const size_t min_block_size = 2*dsize;     // Minimum block size
static const size_t chunksize = (1 << 12); // requires (chunksize % 16 == 0)

static const word_t alloc_mask  = 0x1;
static const word_t palloc_mask = 0x2;
static const word_t size_mask   = ~(word_t)0xF;

typedef struct block
{
    /* Header contains size + allocation flag */
    word_t header;

    /*
     * We don't know how big the payload will be.  Declaring it as an
     * array of size 0 allows computing its starting address using
     * pointer notation.
     */
    char payload[0];

    /*
     * We can't declare the footer as part of the struct, since its starting
     * position is unknown
     */
} block_t;

/* Global variables */
/* Pointer to first block */
static block_t* heap_hp = NULL;
static block_t* heap_tp = NULL;
static block_t* seg_hps[SEG_BINS];
static block_t* seg_tps[SEG_BINS];
static size_t seg_size[] = {32, 48, 64, 80};

/* Function prototypes for internal helper routines */
static block_t *extend_heap(size_t size);
static void place(block_t *block, size_t asize);
static block_t *find_fit(size_t asize);
static block_t *coalesce(block_t *block);

static size_t max(size_t x, size_t y);
static size_t round_up(size_t size, size_t n);
static word_t pack(size_t size, bool alloc);

static size_t extract_size(word_t header);
static size_t get_size(block_t *block);
static size_t get_payload_size(block_t *block);

static bool extract_alloc(word_t header);
static bool get_alloc(block_t *block);

static void write_header(block_t *block, size_t size, bool alloc);
static void write_footer(block_t *block, size_t size, bool alloc);

static block_t *find_next(block_t *block);
static word_t *find_prev_footer(block_t *block);
static block_t *find_prev(block_t *block);

static block_t *payload_to_header(void *bp);
static void *header_to_payload(block_t *block);

static bool in_heap(const void *p);
static bool aligned(const void *p);
static size_t align(size_t x);

block_t** get_prevp(block_t* block);
block_t** get_nextp(block_t* block);
word_t* get_footer(block_t *block);
void set_palloc(block_t* block, bool palloc);
bool get_palloc(block_t* block);

void pop_from_freel(block_t* block);
void push_to_freel(block_t* block);

void print_heap();
void print_freelist();
bool mm_checkheap(int lineno);
bool mm_checkblock(block_t* block);

bool mm_init(void) {

    dbg_printf("mm_init::entry\n");

    heap_hp = NULL;
    heap_tp = NULL;
    memset(seg_hps, 0, SEG_BINS*sizeof(block_t*));
    memset(seg_tps, 0, SEG_BINS*sizeof(block_t*));

    word_t *start = (word_t*)(mem_sbrk(2*wsize));

    if (start == (void *)-1)
    {
        return false;
    }

    start[0] = pack(0, true); // Prologue footer
    start[1] = pack(0, true); // Epilogue header

    // Free list starts with first block header (epilogue)
    heap_hp = (block_t*) &(start[1]);

    // Extend the empty heap with a free block of chunksize bytes
    if (extend_heap(chunksize) == NULL)
    {
        return false;
    }
    return true;
}

void *malloc (size_t size) {

    dbg_printf("malloc:entry(%lu)\n", size);

    dbg_requires(mm_checkheap);

    size_t asize;      // Adjusted block size
    size_t extendsize; // Amount to extend heap if no fit is found
    block_t *block;
    void *bp = NULL;

    if (heap_hp == NULL) // Initialize heap if it isn't initialized
    {
        mm_init();
    }

    if (size == 0) // Ignore spurious request
    {
        dbg_ensures(mm_checkheap);
        return bp;
    }

    // Adjust block size to include overhead and to meet alignment requirements
    asize = max(round_up(size+wsize, dsize), min_block_size);

    // Search the free list for a fit
    block = find_fit(asize);

    // If no fit is found, request more memory, and then and place the block
    if (block == NULL)
    {
        extendsize = max(asize, chunksize);
        block = extend_heap(extendsize);
        if (block == NULL) // extend_heap returns an error
        {
            return bp;
        }
    }

    place(block, asize);
    bp = header_to_payload(block);

    dbg_printf("Malloc size %zd on address %p.\n", size, bp);
    dbg_ensures(mm_checkheap);

    return bp;
}

void free (void *ptr) {

    dbg_printf("free::entry(%p)\n", ptr);

    if (ptr == NULL) {
        return;
    }

    block_t *block = payload_to_header(ptr);
    size_t size = get_size(block);

    write_header(block, size, false);
    write_footer(block, size, false);

    dbg_printheap();

    block_t* nb = find_next(block);
    if (get_size(nb) > 0) {
        set_palloc(nb, false);
    }

    dbg_printheap();

    push_to_freel(block);

    dbg_printheap();

    coalesce(block);
}

void *realloc(void *oldptr, size_t size) {

    block_t *block = payload_to_header(oldptr);
    size_t copysize;
    void *newptr;

    // If size == 0, then free block and return NULL
    if (size == 0)
    {
        free(oldptr);
        return NULL;
    }

    // If oldptr is NULL, then equivalent to malloc
    if (oldptr == NULL)
    {
        return malloc(size);
    }

    // Otherwise, proceed with reallocation
    newptr = malloc(size);
    // If malloc fails, the original block is left untouched
    if (!newptr)
    {
        return NULL;
    }

    // Copy the old data
    copysize = get_payload_size(block); // gets size of old payload
    if(size < copysize)
    {
        copysize = size;
    }
    memcpy(newptr, oldptr, copysize);

    // Free the old block
    free(oldptr);

    return newptr;
}

void *calloc (size_t nmemb, size_t size) {

    void *bp;
    size_t asize = nmemb * size;

    if (asize/nmemb != size)
    // Multiplication overflowed
    return NULL;

    bp = malloc(asize);
    if (bp == NULL)
    {
        return NULL;
    }
    // Initialize all bits to 0
    memset(bp, 0, asize);

    return bp;
}

/******** The remaining content below are helper and debug routines ********/

/*
 * extend_heap: Extends the heap with the requested number of bytes, and
 *              recreates epilogue header. Returns a pointer to the result of
 *              coalescing the newly-created block with previous free block, if
 *              applicable, or NULL in failure.
 */
static block_t *extend_heap(size_t size) {

    dbg_printf("extend_heap::entry\n");

    void *bp;

    // Allocate an even number of words to maintain alignment
    size = round_up(size, dsize);

    if ((bp = mem_sbrk(size)) == (void *)-1) {
        return NULL;
    }

    // Initialize free block header/footer
    block_t* block = payload_to_header(bp);
    write_header(block, size, false);
    write_footer(block, size, false);
    if (heap_tp == NULL) set_palloc(block, true);
    else                 set_palloc(block, get_alloc(heap_tp));

    heap_tp = block;
    push_to_freel(block);

    // Create new epilogue header
    block_t* block_next = find_next(block);
    write_header(block_next, 0, true);

    // Coalesce in case the previous block was free
    return coalesce(block);
}

// [KBK:KBK:?] Remove unncess NULL'ing, removal of footers and unsetting pallocs
static block_t *coalesce(block_t * block) {

    dbg_printf("coalesce::entry(%p)\n", block);
    dbg_printheap();

    block_t* block_prev;
    block_t* block_next = find_next(block);

    bool prev_alloc = get_palloc(block);
    bool next_alloc = get_alloc(block_next);

    if (!prev_alloc) {
        block_prev = find_prev(block);
    }

    size_t size = get_size(block);

    if (prev_alloc && next_alloc) {
        return block;
    }
    else if (prev_alloc && !next_alloc) {
        pop_from_freel(block);
        pop_from_freel(block_next);
        block_t** block_next_pp = get_prevp(block_next);
        block_t** block_next_np = get_nextp(block_next);
        *block_next_pp = NULL;
        *block_next_np = NULL;
        write_footer(block, 0, false);
        write_footer(block_next, 0, false);
        size += get_size(block_next);
        write_header(block, size, false);
        write_footer(block, size, false);
        if (heap_tp == block_next) heap_tp = block;
        push_to_freel(block);
    }
    else if (!prev_alloc && next_alloc) {
        pop_from_freel(block);
        pop_from_freel(block_prev);
        block_t** block_pp = get_prevp(block);
        block_t** block_np = get_nextp(block);
        *block_pp = NULL;
        *block_np = NULL;
        write_footer(block, 0, false);
        write_footer(block_prev, 0, false);
        size += get_size(block_prev);
        write_header(block_prev, size, false);
        write_footer(block_prev, size, false);
        if (heap_tp == block) heap_tp = block_prev;
        push_to_freel(block_prev);
        block = block_prev;
    }
    else {
        pop_from_freel(block);
        pop_from_freel(block_prev);
        pop_from_freel(block_next);
        block_t** block_pp = get_prevp(block);
        block_t** block_np = get_nextp(block);
        *block_pp = NULL;
        *block_np = NULL;
        block_t** block_next_pp = get_prevp(block_next);
        block_t** block_next_np = get_nextp(block_next);
        *block_next_pp = NULL;
        *block_next_np = NULL;
        write_footer(block, 0, false);
        write_footer(block_prev, 0, false);
        write_footer(block_next, 0, false);
        size += get_size(block_next) + get_size(block_prev);
        write_header(block_prev, size, false);
        write_footer(block_prev, size, false);
        if (heap_tp == block_next) heap_tp = block_prev;
        push_to_freel(block_prev);
        block = block_prev;
    }

    dbg_printheap();

    return block;
}

/*
 * place: Places block with size of asize at the start of bp. If the remaining
 *        size is at least the minimum block size, then split the block to the
 *        the allocated block and the remaining block as free, which is then
 *        inserted into the segregated list. Requires that the block is
 *        initially unallocated.
 */
static void place(block_t *block, size_t asize) {

    dbg_printf("place::entry:asize:%lu\n", asize);

    pop_from_freel(block);
    write_footer(block, 0, false);

    size_t csize = get_size(block);
    if ((csize - asize) >= min_block_size) {
        write_header(block, asize, true);
        block_t* block_next = find_next(block);
        write_header(block_next, csize-asize, false);
        write_footer(block_next, csize-asize, false);
        set_palloc(block_next, true);
        if (block == heap_tp) heap_tp = block_next;
        push_to_freel(block_next);
    }
    else {
        write_header(block, csize, true);
        block_t* block_next = find_next(block);
        if (get_size(block_next) > 0) {
            set_palloc(block_next, true);
        }
    }
    dbg_printheap();
}

/*
 * find_fit: Looks for a free block with at least asize bytes with
 *           first-fit policy. Returns NULL if none is found.
 */
static block_t *find_fit(size_t asize) {

    dbg_printf("find_fit::entry\n");

    int sb;
    for (sb = 0; sb < SEG_BINS; sb++) {
        if (asize <= seg_size[sb] || sb == SEG_BINS-1) {
            block_t *block;
            for (block = seg_hps[sb]; block != NULL; block = *get_nextp(block)) {
                if (asize <= get_size(block)) {
                    return block;
                }
            }
        }
    }

    return NULL; // no fit found
}

/*
 * max: returns x if x > y, and y otherwise.
 */
static size_t max(size_t x, size_t y)
{
    return (x > y) ? x : y;
}

/*
 * round_up: Rounds size up to next multiple of n
 */
static size_t round_up(size_t size, size_t n)
{
    return (n * ((size + (n-1)) / n));
}

/*
 * pack: returns a header reflecting a specified size and its alloc status.
 *       If the block is allocated, the lowest bit is set to 1, and 0 otherwise.
 */
static word_t pack(size_t size, bool alloc)
{
    return size | (alloc ? 1: 0);
}

/*
 * extract_size: returns the size of a given header value based on the header
 *               specification above.
 */
static size_t extract_size(word_t word)
{
    return (word & size_mask);
}

/*
 * get_size: returns the size of a given block by clearing the lowest 4 bits
 *           (as the heap is 16-byte aligned).
 */
static size_t get_size(block_t *block)
{
    return extract_size(block->header);
}

/*
 * get_payload_size: returns the payload size of a given block, equal to
 *                   the entire block size minus the header size.
 */
static word_t get_payload_size(block_t *block)
{
    size_t asize = get_size(block);
    return asize - wsize;
}

/*
 * extract_alloc: returns the allocation status of a given header value based
 *                on the header specification above.
 */
static bool extract_alloc(word_t word)
{
    return (bool)(word & alloc_mask);
}

/*
 * get_alloc: returns true when the block is allocated based on the
 *            block header's lowest bit, and false otherwise.
 */
static bool get_alloc(block_t *block)
{
    return extract_alloc(block->header);
}

/*
 * write_header: given a block and its size and allocation status,
 *               writes an appropriate value to the block header.
 */
static void write_header(block_t *block, size_t size, bool alloc)
{
    block->header = (pack(size, alloc) | (block->header & palloc_mask));
}

/*
 * write_footer: given a block and its size and allocation status,
 *               writes an appropriate value to the block footer by first
 *               computing the position of the footer.
 */
static void write_footer(block_t *block, size_t size, bool alloc)
{
    word_t *footerp = (word_t *)((block->payload) + get_size(block) - dsize);
    *footerp = pack(size, alloc);
}


/*
 * find_next: returns the next consecutive block on the heap by adding the
 *            size of the block.
 */
static block_t *find_next(block_t *block)
{
    dbg_requires(block != NULL);
    block_t *block_next = (block_t *)(((char *)block) + get_size(block));

    dbg_ensures(block_next != NULL);
    return block_next;
}

/*
 * find_prev_footer: returns the footer of the previous block.
 */
static word_t *find_prev_footer(block_t *block)
{
    // Compute previous footer position as one word before the header
    return (&(block->header)) - 1;
}

/*
 * find_prev: returns the previous block position by checking the previous
 *            block's footer and calculating the start of the previous block
 *            based on its size.
 */
static block_t *find_prev(block_t *block)
{
    word_t *footerp = find_prev_footer(block);
    size_t size = extract_size(*footerp);
    return (block_t *)((char *)block - size);
}

/*
 * payload_to_header: given a payload pointer, returns a pointer to the
 *                    corresponding block.
 */
static block_t *payload_to_header(void *bp)
{
    return (block_t *)(((char *)bp) - offsetof(block_t, payload));
}

/*
 * header_to_payload: given a block pointer, returns a pointer to the
 *                    corresponding payload.
 */
static void *header_to_payload(block_t *block)
{
    return (void *)(block->payload);
}

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static bool in_heap(const void *p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static bool aligned(const void *p) {
    size_t ip = (size_t) p;
    return align(ip) == ip;
}

/* rounds up to the nearest multiple of ALIGNMENT */
static size_t align(size_t x) {
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

block_t** get_prevp(block_t* block) {
    return (block_t**)(block->payload);
}

block_t** get_nextp(block_t* block) {
    return (block_t**)(((char*)block->payload) + wsize);
}

word_t* get_footer(block_t *block) {
    return (word_t *)((block->payload) + get_size(block) - dsize);
}

void set_palloc(block_t* block, bool palloc) {
    if (palloc) block->header |= palloc_mask;
    else        block->header &= ~palloc_mask;
}

bool get_palloc(block_t* block) {
    return (bool)(block->header & palloc_mask);
}

void push_to_freel(block_t* block) {

    dbg_printf("push_to_freel::entry(%p)\n", block);

    size_t bsize = get_size(block);
    int sb;
    for (sb = 0; sb < SEG_BINS; sb++) {
        if (bsize <= seg_size[sb] || sb == SEG_BINS-1) {
            block_t** block_pp = get_prevp(block);
            block_t** block_np = get_nextp(block);
            *block_pp = seg_tps[sb];
            *block_np = NULL;
            if (seg_tps[sb] != NULL) {
                block_t** seg_tp_np = get_nextp(seg_tps[sb]);
                *seg_tp_np = block;
            }
            seg_tps[sb] = block;
            if (seg_hps[sb] == NULL) seg_hps[sb] = block;
            break;
        }
    }
}

void pop_from_freel(block_t* block) {

    dbg_printf("pop_from_freel::entry(%p)\n", block);

    block_t** block_np = get_nextp(block);
    block_t** block_pp = get_prevp(block);
    if (*block_np != NULL) {
        block_t** block_np_pp = get_prevp(*block_np);
        *block_np_pp = *block_pp;
    }
    if (*block_pp != NULL) {
        block_t** block_pp_np = get_nextp(*block_pp);
        *block_pp_np = *block_np;
    }

    size_t bsize = get_size(block);
    int sb;
    for (sb = 0; sb < SEG_BINS; sb++) {
        if (bsize <= seg_size[sb] || sb == SEG_BINS-1) {
            if (block == seg_hps[sb]) {
                seg_hps[sb] = *block_np;
            }
            if (block == seg_tps[sb]) {
                seg_tps[sb] = *block_pp;
            }
            *block_np = NULL;
            *block_pp = NULL;
            break;
        }
    }
}

void print_heap() {
    dbg_printf("\n  print_heap::entry::heap_tp(%p)\n\n", heap_tp);
    dbg_printf("  @prol(%09lx)->footer(%09lx)\n",
        (word_t)find_prev_footer(heap_hp),
        *find_prev_footer(heap_hp));

    block_t* block;
    for (block = heap_hp; get_size(block) > 0; block = find_next(block)) {
        int sb;
        if (block == heap_hp) {
            dbg_printf("  @(heap_hp:%09lx)\n", (word_t)block);
        }
        for (sb = 0; sb < SEG_BINS; sb++) {
            if (block == seg_hps[sb]) {
                dbg_printf("  @(seg_hps[%d]:%09lx)\n", sb, (word_t)block);
            }
        }
        dbg_printf("  @(%09lx)->head(%09lu):@(%09lx)->prev(%09lu):"
            "@(%09lx)->next(%09lx):@(%09lx)->footer(%09lu)\n",
            (word_t)block, (word_t)block->header,
            (word_t)get_prevp(block), (word_t)*get_prevp(block),
            (word_t)get_nextp(block), (word_t)*get_nextp(block),
            (word_t)get_footer(block), (word_t)*get_footer(block));
        for (sb = 0; sb < SEG_BINS; sb++) {
            if (block == seg_tps[sb]) {
                dbg_printf("  @(seg_tps[%d]:%09lx)\n", sb, (word_t)block);
            }
        }
        if (block == heap_tp) {
            dbg_printf("  @(heap_tp:%09lx)\n", (word_t)block);
        }
    }
    dbg_printf("  @epil(%09lx)->head(%09lx)\n",
        (word_t)&find_next(heap_tp)->header,
        find_next(heap_tp)->header);
    dbg_printf("\n  print_heap::exit\n");
}

void print_freelist() {
    dbg_printf("\n  print_freelist::entry\n\n");

    int sb;
    for (sb = 0; sb < SEG_BINS; sb++) {
        block_t* block;
        for (block = seg_hps[sb]; block != NULL; block = *get_nextp(block)) {
            if (block == seg_hps[sb]) {
                dbg_printf("  @(seg_hps[%d]:%09lx)\n", sb, (word_t)block);
            }
            dbg_printf("  @(%09lx)->head(%09lu):@(%09lx)->prev(%09lx):"
                "@(%09lx)->next(%09lx):@(%09lx)->footer(%09lu)\n",
                (word_t)block, (word_t)block->header,
                (word_t)get_prevp(block), (word_t)*get_prevp(block),
                (word_t)get_nextp(block), (word_t)*get_nextp(block),
                (word_t)get_footer(block), (word_t)*get_footer(block));
            if (block == seg_tps[sb]) {
                dbg_printf("  @(seg_tps[%d]:%09lx)\n", sb, (word_t)block);
            }
        }
    }

    dbg_printf("\n  print_freelist::exit\n");
}

bool mm_checkblock(block_t* block) {

    dbg_printf("  mm_checkblock::entry\n");

    word_t* fp = get_footer(block);

    /* check epilogue & prologue */
    if (aligned(block)) {
        dbg_printf("  mm_cb::block(@%p) is not aligned\n", block);
        return false;
    }
    if (!in_heap(block)) {
        dbg_printf("  mm_cb::block(@%p) not in heap\n", block);
        return false;
    }
    if (!get_alloc(block) && (get_size(block) != extract_size(*fp))) {
        dbg_printf("  mm_cb::block(@%p) header footer size don't match\n", block);
        return false;
    }
    if (!get_alloc(block) && (get_alloc(block) != extract_alloc(*fp))) {
        dbg_printf("  mm_cb::block(@%p) header footer alloc don't match\n", block);
        return false;
    }
    block_t* nb = find_next(block);
    if (get_size(nb) > 0 && get_alloc(block) == 0 && get_alloc(nb) == 0) {
        dbg_printf("  mm_cb::free blocks(@%p & @%p) aren't coalesced\n", block, nb);
        return false;
    }
    return true;
}

bool mm_checkheap(int lineno) {
    dbg_printf("\n  mm_checkheap::line(%d)\n", lineno);

    dbg_printheap();
    print_freelist();

    word_t* pf = find_prev_footer(heap_hp);

    if (extract_size(*pf) != 0) {
        dbg_printf("  mm_ch::prologue footer(@%p) size != 0\n", &pf);
        return false;
    }
    if (!extract_alloc(*pf)) {
        dbg_printf("  mm_ch::prologue footer(@%p) alloc unset\n", &pf);
        return false;
    }

    block_t* block;
    for (block = heap_hp; get_size(block) > 0; block = find_next(block)) {
        if (!mm_checkblock(block)) return false;
    }
    if (get_size(block) != 0) {
        dbg_printf("  mm_ch::epilogue header(@%p) size != 0\n", block);
        return false;
    }
    if (!get_alloc(block)) {
        dbg_printf("  mm_ch::epilogue header(@%p) alloc unset\n", block);
        return false;
    }

    int freel_size = 0;
    int freeh_size = 0;
    int sb;
    for (sb = 0; sb < SEG_BINS; sb++) {
        for (block = seg_hps[sb]; block != NULL; block = *get_nextp(block)) {
            freel_size++;
            if (*get_nextp(block) != NULL && !in_heap(*get_nextp(block))) {
                dbg_printf("  mm_cfl::(%p)->next not in heap\n", *get_nextp(block));
                return false;
            }
            if (*get_prevp(block) != NULL && !in_heap(*get_prevp(block))) {
                dbg_printf("  mm_cfl::(%p)->prev not in heap\n", *get_prevp(block));
                return false;
            }
            if (*get_nextp(block) != NULL && block != *get_prevp(*get_nextp(block))) {
                dbg_printf("  mm_cfl::(%p)->next(%p) & (%p)->prev(%p) don't match\n",
                    block, *get_nextp(block),
                    *get_nextp(block), *get_prevp(*get_nextp(block)));
                return false;
            }
        }
    }

    for (block = heap_hp; get_size(block) > 0; block = find_next(block)) {
        if (!get_alloc(block)) {
            freeh_size++;
        }
    }

    if (freel_size != freeh_size) {
        dbg_printf("  mm_cfl::freelist(%d) != freeheap(%d)\n", freel_size, freeh_size);
        return false;
    }
    dbg_printf(" mm_checkheap::exit\n");
    return true;
}

/*
 * mm.c
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
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

/* rounds up to the nearest multiple of ALIGNMENT */
static size_t align(size_t x) {
    return ALIGNMENT * ((x+ALIGNMENT-1)/ALIGNMENT);
}

/*
 * Initialize: return false on error, true on success.
 */
bool mm_init(void) {
    return true;
}

/*
 * malloc
 */
void *malloc (size_t size) {
    return NULL;
}

/*
 * free
 */
void free (void *ptr) {
    return;
}

/*
 * realloc
 */
void *realloc(void *oldptr, size_t size) {
    return NULL;
}

/*
 * calloc
 * This function is not tested by mdriver
 */
void *calloc (size_t nmemb, size_t size) {
    return NULL;
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

/*
 * mm_checkheap
 */
bool mm_checkheap(int lineno) {
    return true;
}


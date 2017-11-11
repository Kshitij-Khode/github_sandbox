/**
 * cache.c - Reference LRU cache implementation.
 * Everything we need to manage a cache instance is contained in here.
 */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "cache.h"
#include <stdio.h>

int verbosity = 1;

/*
 * Inputs: set_bits, associativity and block_bits parameters that define architecture of the
 *         cache.
 * Outputs: pointer of the cache struct initialized by the specifications provided in input
 *
 * Description:
 * 1. Allocates memory and stores input cache architecture params
 * 2. Calculates appropriate helper data based on input cache architecture params
 * 3. Allocates memory for all the lines pertaining to cache and initializes them
 */
void cacheInit(
  cache_t* cache,
  uint32_t set_bits,
  uint32_t associativity,
  uint32_t block_bits) {
    cache->block_bits = block_bits;
    cache->set_bits = set_bits;
    cache->associativity = associativity;
    cache->set_count = 0;
    cache->set_mask = ((1<<set_bits)-1)<<block_bits;
    cache->sets = (cache_line_t**)malloc(sizeof(cache_line_t*)*(1<<set_bits));
    mem_addr_t set_ind, set_line;
    for (set_ind = 0; set_ind < (1<<set_bits); set_ind++) {
        cache->sets[set_ind] = (cache_line_t*)malloc(sizeof(cache_line_t)*associativity);
        for (set_line = 0; set_line < associativity; set_line++) {
            cache->sets[set_ind][set_line].state = INVALID;
            cache->sets[set_ind][set_line].tag = -1;
            cache->sets[set_ind][set_line].age = -1;
        }
    }
}

/*
 * Description: Frees memory allocated to the cache struct
 */
void cacheDestroy(cache_t* cache) {
    mem_addr_t set_ind;
    for (set_ind = 0; set_ind < (1<<cache->set_bits); set_ind++) {
        free(cache->sets[set_ind]);
    }
    free(cache->sets);
}

/*
 * Inputs: pointer to the cache being accessed, memory address being accessed, the kind of
 *         operation being done i.e. load or store and the verbosity level of the function.
 * Outputs: Result containing if a hit, miss or eviction was done.
 *
 * Description:
 * 1. Looks through all cache line and seeks a matching tag. No need to check valid bit
 *    because in single core, after one hit or miss, valid bit will always be set. Return
 *    hit if successfull.
 * 2. If tag not found in cache, then see an uninitialized (valid bit = 0) line is present.
 *    If so, update the line appropriately and return miss.
 * 3. If tag not found and an unintialized line isn;t found, evict existing one based on
 *    LRU criteria.
 */
cache_result_t csimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op) {
    mem_addr_t set_ind = (addr & cache->set_mask)>>cache->block_bits;
    mem_addr_t tag_mask = ~((1<<(cache->set_bits+cache->block_bits))-1);
    mem_addr_t tag = (addr & tag_mask)>>(cache->set_bits+cache->block_bits);

    mem_addr_t set_line;
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].tag == tag) {
            cache->sets[set_ind][set_line].state = SHARED;
            cache->sets[set_ind][set_line].age = cache->set_count++;
            if (verbosity) printf("hit\n");
            return CACHE_HIT;
        }
    }
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].state == INVALID) {
            cache->sets[set_ind][set_line].tag = tag;
            cache->sets[set_ind][set_line].state = SHARED;
            cache->sets[set_ind][set_line].age = cache->set_count++;
            if (verbosity) printf("miss\n");
            return CACHE_MISS;
        }
    }
    cache_line_t* evicted_line = &cache->sets[set_ind][0];
    for (set_line = 1; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].age < evicted_line->age) {
            evicted_line = &cache->sets[set_ind][set_line];
        }
    }
    evicted_line->tag = tag;
    evicted_line->state = SHARED;
    evicted_line->age = cache->set_count++;
    if (verbosity) printf("miss eviction\n");
    return CACHE_EVICT;
}

/*
 * Inputs: pointer to the cache being accessed, memory address being accessed, the kind of
 *         operation being done i.e. load or store and the verbosity level of the function.
 * Outputs: Result containing if a hit, miss or eviction was done.
 *
 * Description: Function operates on a cache struct pertaining to a single core
 * 1. Looks through all cache line and seeks a matching tag with state SHARED or MODIFIED.
 *    If operation is a write with previous state being a SHARED would a state change occur.
 *    Return hit.
 * 2. If tag not found in cache, then see if an INVALID line is present. Based on if the
 *    operation is a load or store, set status to ShARED or MODIFIED.
 * 3. If tag not found and an INVALID line is not found, evict existing one based on
 *    LRU criteria. If operation is load or store, update state appropriately. Return evict.
 */
cache_result_t msimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op) {
    mem_addr_t set_ind = (addr & cache->set_mask)>>cache->block_bits;
    mem_addr_t tag_mask = ~((1<<(cache->set_bits+cache->block_bits))-1);
    mem_addr_t tag = (addr & tag_mask)>>(cache->set_bits+cache->block_bits);
    mem_addr_t set_line;
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].tag == tag &&
            cache->sets[set_ind][set_line].state != INVALID)
        {
            if (cache->sets[set_ind][set_line].state != MODIFIED && op == OP_WRITE) {
                cache->sets[set_ind][set_line].state = MODIFIED;
            }
            cache->sets[set_ind][set_line].age = cache->set_count++;
            if (verbosity) printf("hit\n");
            return CACHE_HIT;
        }
    }
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].state == INVALID) {
            if (op == OP_READ) {
                cache->sets[set_ind][set_line].state = SHARED;
            }
            else {
                cache->sets[set_ind][set_line].state = MODIFIED;
            }
            cache->sets[set_ind][set_line].tag = tag;
            cache->sets[set_ind][set_line].age = cache->set_count++;
            if (verbosity) printf("miss\n");
            return CACHE_MISS;
        }
    }
    cache_line_t* evicted_line = &cache->sets[set_ind][0];
    for (set_line = 1; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].age < evicted_line->age) {
            evicted_line = &cache->sets[set_ind][set_line];
        }
    }
    if (op == OP_READ) {
        evicted_line->state = SHARED;
    }
    else {
        evicted_line->state = MODIFIED;
    }
    evicted_line->tag = tag;
    evicted_line->age = cache->set_count++;
    if (verbosity) printf("miss eviction\n");
    return CACHE_EVICT;
}

/*
 * NOTE: Since the assignment only cares for invalidations, no matter which transition
 * caused, I'll be returning TRSN_S2I for any invalidation.
 *
 * Inputs: pointer to the cache being accessed, memory address being accessed, the kind of
 *         operation being done i.e. load or store and the verbosity level of the function.
 * Outputs: If a state change occured (to invalid or shared) or not in the cache.
 *
 * Description: Function operates on a cache struct pertaining to a single core
 * 1. If store occured in one of the core's cache line and another core's cache line has the
 *    same tag, change the state of the line in the other core to invalid.
 * 2. If a read occured in one of the core's cache line and another core's cache line has
 *    the same tag, change the state of the line in the other core to shared.
 *    Return hit.
 * 3. Else simply return no transition change, since all the other transition changes
 *    are inconsequential for the assignment results.
 */
msi_trsn_t cacheBus(cache_t* cache, mem_addr_t addr, op_t op) {
    mem_addr_t set_ind = (addr & cache->set_mask)>>cache->block_bits;
    mem_addr_t tag_mask = ~((1<<(cache->set_bits+cache->block_bits))-1);
    mem_addr_t tag = (addr & tag_mask)>>(cache->set_bits+cache->block_bits);

    mem_addr_t set_line;
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].tag == tag) {
            if (op == OP_WRITE && cache->sets[set_ind][set_line].state != INVALID) {
                cache->sets[set_ind][set_line].state = INVALID;
                /* if (verbosity) printf("invalidation\n"); */
                return TRSN_S2I;
            }
            if (op == OP_READ && cache->sets[set_ind][set_line].state == MODIFIED) {
                cache->sets[set_ind][set_line].state = SHARED;
                return TRSN_M2S;
            }
        }
    }
    return TRSN_NONE;
}

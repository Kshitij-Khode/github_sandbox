/**
 * cache.c - Reference LRU cache implementation.
 * Everything we need to manage a cache instance is contained in here.
 */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "cache.h"
#include <stdio.h>

cache_t* cacheInit(
  uint32_t set_bits,
  uint32_t associativity,
  uint32_t block_bits) {
    cache_t* cache = (cache_t*)malloc(sizeof(*cache));
    cache->block_bits = block_bits;
    cache->set_bits = set_bits;
    cache->associativity = associativity;
    cache->line_count = 0;
    cache->set_mask = ((1<<set_bits)-1)<<block_bits;
    cache->tag_mask = ~((1<<(set_bits+block_bits))-1);
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
    return cache;
}

void cacheDestroy(cache_t* cache) {
    mem_addr_t set_ind;
    for (set_ind = 0; set_ind < (1<<cache->set_bits); set_ind++) {
        free(cache->sets[set_ind]);
    }
    free(cache->sets);
    free(cache);
}

cache_result_t csimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op, int verbosity) {
    mem_addr_t set_ind = (addr & cache->set_mask)>>cache->block_bits;
    mem_addr_t tag = (addr & cache->tag_mask)>>(cache->set_bits+cache->block_bits);
    mem_addr_t set_line;
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].tag == tag) {
            cache->sets[set_ind][set_line].state = SHARED;
            cache->sets[set_ind][set_line].age = cache->line_count++;
            if (verbosity) printf("hit\n");
            return CACHE_HIT;
        }
    }
    for (set_line = 0; set_line < cache->associativity; set_line++) {
        if (cache->sets[set_ind][set_line].state == INVALID) {
            cache->sets[set_ind][set_line].tag = tag;
            cache->sets[set_ind][set_line].state = SHARED;
            cache->sets[set_ind][set_line].age = cache->line_count++;
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
    evicted_line->age = cache->line_count++;
    if (verbosity) printf("miss eviction\n");
    return CACHE_EVICT;
}

cache_result_t msimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op, int verbosity) {
  cache_result_t result = CACHE_HIT;
  return result;
}

msi_trsn_t cacheBus(cache_t* cache, mem_addr_t addr, op_t op) {
  msi_trsn_t transition = TRSN_NONE;
  return transition;
}

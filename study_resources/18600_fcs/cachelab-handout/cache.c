/**
 * cache.c - Reference LRU cache implementation.
 * Everything we need to manage a cache instance is contained in here.
 */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "cache.h"

void cacheInit(
  cache_t* cache,
  uint32_t set_bits,
  uint32_t associativity,
  uint32_t block_bits) {

}

void cacheDestroy(cache_t* cache) {
}


cache_result_t csimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op) {
 cache_result_t result = CACHE_HIT;
 return result;
}

cache_result_t msimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op) {
  cache_result_t result = CACHE_HIT;
  return result;
}

msi_trsn_t cacheBus(cache_t* cache, mem_addr_t addr, op_t op) {
  msi_trsn_t transition = TRSN_NONE;
  return transition;
}

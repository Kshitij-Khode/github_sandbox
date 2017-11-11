/**
 * cache.h - Prototypes for cache-simulator implementation.
 * This is the reference implementation.
 */

#include <stdint.h>


#ifndef CACHE_H
#define CACHE_H

typedef unsigned long long int mem_addr_t;

/** Enum for memory operation types. */
typedef enum {
  OP_READ = 0,
  OP_WRITE = 1
} op_t;

/** Enum for different cache responses. */
typedef enum {
  CACHE_HIT,
  CACHE_MISS,
  CACHE_EVICT
} cache_result_t;

/** Enum for each state in MSI protocol. */
typedef enum {
  INVALID = 0,
  SHARED = 1, // OR VALID
  MODIFIED = 2
} state_t;

/** Enum for tracking MSI state transitions. */
typedef enum {
  TRSN_NONE,
  TRSN_M2I,
  TRSN_M2S,
  TRSN_S2I,
  TRSN_S2M,
  TRSN_I2M,
  TRSN_I2S
} msi_trsn_t;

typedef struct cache_line {
  state_t state;
  mem_addr_t tag;
  uint32_t age;
} cache_line_t;

typedef struct cache {
  /* required */
  uint32_t set_bits;
  uint32_t associativity;
  uint32_t block_bits;
  /* computed */
  int set_count;
  mem_addr_t set_mask;
  cache_line_t** sets;
} cache_t;

/**
 * cache_init
 *
 * Initialize cache struct.
 */
void cacheInit(
  cache_t* cache,
  uint32_t set_bits,
  uint32_t associativity,
  uint32_t block_bits);

/**
 * cache_destroy
 *
 * Destroy cache struct and clean up memory.
 */
void cacheDestroy(cache_t* cache);

/**
 * cache_access
 *
 * Request an address from the cache. This can have side-effects that help
 * maintain the state of each line for coherency and replacement policies.
 */
cache_result_t csimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op);

/**
 * cache_access
 *
 * Request an address from the cache. This can have side-effects that help
 * maintain the state of each line for coherency and replacement policies.
 */
cache_result_t msimCacheAccess(cache_t* cache, mem_addr_t addr, op_t op);

/**
 * cache_bus
 *
 * Send a message to `cache` over the bus about an `addr`.
 * This message is a R/W operation about a block.
 * @return The [[msi_trsn_t]] block transition resulting from the message.
 */
msi_trsn_t cacheBus(cache_t* cache, mem_addr_t addr, op_t op);

#endif // CACHE_H

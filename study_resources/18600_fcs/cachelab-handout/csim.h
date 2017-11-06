/**
 * csim.h - Prototypes for cache-simulator implementation
 */
#include "trace-stream.h"


#ifndef CSIM_H
#define CSIM_H

typedef struct cache_config {
  int verbosity;
  int set_bits;
  int associativity;
  int block_bits;
  trace_stream_t trace;
} cache_config_t;

/**
 * This result struct should correspond to the config provided.
 * See definition for `cache_config` above.
 */
typedef struct sim_result {
  int hits;
  int misses;
  int evictions;
} sim_result_t;

sim_result_t runSimulator(cache_config_t* config);

#endif // CSIM_H

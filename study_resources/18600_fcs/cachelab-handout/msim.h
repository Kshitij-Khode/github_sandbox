/**
 * msim.h - Prototypes for cache-simulator implementation
 */
#include "trace-stream.h"


#ifndef MSIM_H
#define MSIM_H

#define MAX_CORES 4

typedef struct cache_config {
  int verbosity;
  int set_bits;
  int associativity;
  int block_bits;
  int num_cores;
  trace_stream_t core_traces[MAX_CORES];
} cache_config_t;

/**
 * This result struct should correspond to the config provided.
 * See definition for `cache_config` above.
 *
 * Ex. If `num_cores` is 2, `cores` should have 2 populated entries.
 */
typedef struct sim_result {
  int hits;
  int misses;
  int evictions;
  int invalidations;
} sim_result_t;

typedef struct sim_results {
  sim_result_t cores[MAX_CORES];
} sim_results_t;


sim_results_t runSimulator(cache_config_t* config);

#endif // MSIM_H

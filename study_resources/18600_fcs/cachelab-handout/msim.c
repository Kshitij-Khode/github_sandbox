/*
 * msim.c - A cache simulator that implements LRU and MSI protocol for
 *   cache coherency.
 *
 * Name: Kshitij Khode
 * Andrew ID: kkhode
 *
 */
#include <stdio.h>
#include "cache.h"
#include "msim.h"
#include "trace-stream.h"

int nextTraces(trace_entry_t** trace_entries, cache_config_t* config);

/*
 * Inputs: Config for the type of run specified through command line args
 * Outputs: No of misses, hits, evictions and invalidations
 *
 * Description:
 * Does all the following steps for all the number of cores specified in args
 * 1. Resets the counter for counting misses, hits, evictions and invalidations
 * 2. Initializes the cache struct (including mallocing and initializing default values).
 * 3. Attempts accessing cache for directives provided by a line provided in the trace file
 *    and records the outcome in terms of hit, miss or eviction.
 * 4. Allows rest of the cores to snoop and update their own state for tag matching lines
 * 5. Frees malloc'd memory provided to cache struct
 */
sim_results_t runSimulator(cache_config_t* config) {
    sim_results_t results;
    trace_entry_t* trace_entries[config->num_cores];
    cache_t caches[config->num_cores];
    op_t op;

    int core;
    for (core = 0; core < config->num_cores; core++) {
        results.cores[core].hits = 0;
        results.cores[core].misses = 0;
        results.cores[core].evictions = 0;
        results.cores[core].invalidations = 0;
        cacheInit(&caches[core],
                  config->set_bits,
                  config->associativity,
                  config->block_bits);
    }

    if (config->verbosity) {
        printf("Running trace simulation:\n");
    }

    while (nextTraces(trace_entries, config)) {
        for (core = 0; core < config->num_cores; core++) {
            /* Quickly add ugly statement to deal with reverting to default function proto */
            switch (trace_entries[core]->op) {
                case 'L':
                    op = OP_READ;
                    break;
                case 'S':
                    op = OP_WRITE;
                    break;
            }
            if (config->verbosity) {
                printf("  C%d: ", core);
                switch (trace_entries[core]->op) {
                    case 'L':
                        printf("L ");
                        break;
                    case 'S':
                        printf("S ");
                        break;
                }
                printf("%llx, %d\t", trace_entries[core]->addr, trace_entries[core]->size);
            }
            switch(msimCacheAccess(&caches[core],
                   trace_entries[core]->addr,
                   op))
            {
                case CACHE_HIT:
                    results.cores[core].hits++;
                    break;
                case CACHE_MISS:
                    results.cores[core].misses++;
                    break;
                case CACHE_EVICT:
                    results.cores[core].misses++;
                    results.cores[core].evictions++;
                    break;
            }
            int rest_core;
            for (rest_core = 0; rest_core < config->num_cores; rest_core++) {
                if (rest_core != core && cacheBus(&caches[rest_core],
                                                  trace_entries[core]->addr,
                                                  op) == TRSN_S2I)
                {
                    results.cores[core].invalidations++;
                }
            }
        }
    }
    for (core = 0; core < config->num_cores; core++) {
        cacheDestroy(&caches[core]);
    }
    return results;
}

/*
 * Inputs: Config for the type of run specified through command line args & trace entry struct
 *         used to figure out content of a single line provided in the trace.
 * Outputs: If next line was successfully fetched for any trace pertaining to any core.
 *
 * Description: Gets next line content for trace pertaining to every core and returns true if
 *              that action was successfull.
 */
int nextTraces(trace_entry_t** trace_entries, cache_config_t* config) {
    int core, rc = 0;
    for (core = 0; core < config->num_cores; core++) {
        if ((trace_entries[core] = traceStreamNext(&config->core_traces[core]))) {
            rc |= 1;
        }
        else rc |= 0;
    }
    return rc;
}
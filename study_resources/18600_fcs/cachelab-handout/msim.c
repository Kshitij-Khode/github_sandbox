/*
 * msim.c - A cache simulator that implements LRU and MSI protocol for
 *   cache coherency.
 *
 * You must implement this file.
 */
#include <stdio.h>
#include "cache.h"
#include "msim.h"
#include "trace-stream.h"

int nextTraces(trace_entry_t** trace_entries, cache_config_t* config);

sim_results_t runSimulator(cache_config_t* config) {
    sim_results_t results;
    trace_entry_t* trace_entries[config->num_cores];
    cache_t* caches[config->num_cores];
    int core;
    for (core = 0; core < config->num_cores; core++) {
        results.cores[core].hits = 0;
        results.cores[core].misses = 0;
        results.cores[core].evictions = 0;
        results.cores[core].invalidations = 0;
        caches[core] = cacheInit(config->set_bits,
                                config->associativity,
                                config->block_bits);
    }

    if (config->verbosity) {
        printf("Running trace simulation:\n");
    }

    while (nextTraces(trace_entries, config)) {
        for (core = 0; core < config->num_cores; core++) {
            if (config->verbosity) {
                printf("  C%d: ", core);
                switch (trace_entries[core]->op) {
                    case OP_READ: printf("L "); break;
                    case OP_WRITE: printf("S "); break;
                }
                printf("%llx, %d\t", trace_entries[core]->addr, trace_entries[core]->size);
            }
            switch(msimCacheAccess(caches[core],
                   trace_entries[core]->addr,
                   trace_entries[core]->op,
                   config->verbosity))
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
                if (rest_core != core && cacheBus(caches[rest_core],
                                                  trace_entries[core]->addr,
                                                  trace_entries[core]->op,
                                                  config->verbosity) == TRSN_I)
                {
                    results.cores[core].invalidations++;
                }
            }
        }
    }
    for (core = 0; core < config->num_cores; core++) {
        cacheDestroy(caches[core]);
    }
    return results;
}

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
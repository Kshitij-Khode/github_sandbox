/*
 * csim.c - A cache simulator that implements cache with LRU policy
 *   for replacement.
 *
 * Kshitij Khode
 * Andrew ID: kkhode
 */
#include <stdio.h>
#include "cache.h"
#include "csim.h"
#include "trace-stream.h"

sim_result_t runSimulator(cache_config_t* config) {
    sim_result_t result = {0, 0, 0};
    trace_entry_t* trace_entry;
    cache_t* cache = cacheInit(config->set_bits,
                               config->associativity,
                               config->block_bits);

    if (config->verbosity) {
        printf("Running trace simulation:\n");
    }

    while ((trace_entry = traceStreamNext(&config->trace))) {
        if (config->verbosity) {
            switch (trace_entry->op) {
                case OP_READ: printf("L "); break;
                case OP_WRITE: printf("S "); break;
            }
            printf("%llx,%d\t", trace_entry->addr, trace_entry->size);
        }
        switch (csimCacheAccess(cache, trace_entry->addr, trace_entry->op, config->verbosity)) {
            case CACHE_HIT:
                result.hits++;
                break;
            case CACHE_MISS:
                result.misses++;
                break;
            case CACHE_EVICT:
                result.misses++;
                result.evictions++;
                break;
        }
    }
    cacheDestroy(cache);
    return result;
}

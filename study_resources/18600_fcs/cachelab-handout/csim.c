/*
 * csim.c - A cache simulator that implements cache with LRU policy
 *   for replacement.
 *
 * Name: Kshitij Khode
 * Andrew ID: kkhode
 *
 */
#include <stdio.h>
#include "cache.h"
#include "csim.h"
#include "trace-stream.h"

/*
 * Inputs: Config for the type of run specified through command line args
 * Outputs: No of misses, hits and evictions
 *
 * Description:
 * 1. Resets the counter for counting misses, hits and evictions
 * 2. Initializes the cache struct (including mallocing and initializing default values).
 * 3. Attempts accessing cache for directives provided by a line provided in the trace file
 *    and records the outcome in terms of hit, miss or eviction.
 * 4. Frees malloc'd memory provided to cache struct
 */
sim_result_t runSimulator(cache_config_t* config) {
    sim_result_t result = {0, 0, 0};
    trace_entry_t* trace_entry;
    op_t op;

    cache_t cache;
    cacheInit(&cache, config->set_bits, config->associativity, config->block_bits);

    if (config->verbosity) {
        printf("Running trace simulation:\n");
    }

    while ((trace_entry = traceStreamNext(&config->trace))) {
            /* Quickly add ugly statement to deal with reverting to default function proto */
            switch (trace_entry->op) {
                case 'L':
                    op = OP_READ;
                    break;
                case 'S':
                    op = OP_WRITE;
                    break;
            }
            if (config->verbosity) {
                switch (trace_entry->op) {
                    case 'L':
                        printf("L ");
                        break;
                    case 'S':
                        printf("S ");
                        break;
            }
            printf("%llx,%d\t", trace_entry->addr, trace_entry->size);
        }
        switch (csimCacheAccess(&cache, trace_entry->addr, op)) {
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
    cacheDestroy(&cache);
    return result;
}

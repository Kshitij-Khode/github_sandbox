/**
 * msim-driver.c
 */
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msim.h"
#include "trace-stream.h"

/** printUsage - Print usage info */
void printUsage(char* argv[]) {
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file (may specify multiple)\n");
  printf("\nExamples:\n");
  printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
  printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
  printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace -t traces/yi.trace\n", argv[0]);
  exit(0);
}

/** printConfig - Human readable cache-configuration */
void printConfig(cache_config_t* config) {
  printf("Simulation configuration:\n");
  printf("  Cache dimensions: s = %d, E = %d, b = %d\n",
      config->set_bits,
      config->associativity,
      config->block_bits);
  printf("  Cores: %d\n", config->num_cores);
  printf("  Core kernel traces: ");
  for (int i = 0; i < config->num_cores; i++) {
    printf("%s ", config->core_traces[i].trace_path);
  }
  printf("\n");
  printf("  Verbosity: %d\n", config->verbosity);
}

/** printResults - Autograder readable simulator output */
void printResults(cache_config_t* config, sim_results_t* results) {
  FILE* output = fopen(".msim_results", "w");
  if (!output) {
    fprintf(stderr, ".msim_results: %s\n", strerror(errno));
    exit(1);
  }
  for (int i = 0; i < config->num_cores; i++) {
    sim_result_t r = results->cores[i];
    printf("hits:%d misses:%d evictions:%d invalidations:%d\n",
      r.hits,
      r.misses,
      r.evictions,
      r.invalidations);
    fprintf(output, "%d %d %d %d\n",
      r.hits,
      r.misses,
      r.evictions,
      r.invalidations);
  }
  fclose(output);
}

/* main - Run the driver */
int main(int argc, char* argv[]) {
  char* binary_name = argv[0];
  cache_config_t config;
  config.verbosity = 0;
  config.set_bits = 0;
  config.associativity = 0;
  config.block_bits = 0;
  config.num_cores = 0;

  // Iterate through command line arguments.
  char c;
  while((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
    switch(c) {
      case 's':
        config.set_bits = atoi(optarg);
        break;
      case 'E':
        config.associativity = atoi(optarg);
        break;
      case 'b':
        config.block_bits = atoi(optarg);
        break;
      case 't':
        traceStreamInit(&config.core_traces[config.num_cores], optarg);
        config.num_cores++;
        break;
      case 'v':
        config.verbosity = 1;
        break;
      case 'h':
        printUsage(argv);
        exit(0);
      default:
        printUsage(argv);
        exit(1);
    }
  }

  // Make sure that all required command line args were specified.
  if (config.set_bits == 0 ||
      config.associativity == 0 ||
      config.block_bits == 0 ||
      config.num_cores == 0) {
    printf("%s: Missing required command line argument\n", binary_name);
    printUsage(argv);
    exit(1);
  }

  // Print configuration before running if verbose.
  if (config.verbosity > 0) {
    printConfig(&config);
  }

  // Output the hit and miss statistics for the autograder.
  sim_results_t results = runSimulator(&config);
  printResults(&config, &results);

  // Cleanup.
  for (int i = 0; i < config.num_cores; i++) {
    traceStreamDestroy(&config.core_traces[i]);
  }
  return 0;
}

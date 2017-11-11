/**
 * trace-stream.c
 *
 * This is a memory efficient iterator implementation over a tracefile.
 * It provides a cleaner struct-based API over the entries in Valgrind trace
 * files.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trace-stream.h"

void traceStreamInit(trace_stream_t* ts, char* trace_path) {
  ts->trace_path = trace_path;
  ts->trace_file = fopen(trace_path, "r");
  if (!ts->trace_file) {
    fprintf(stderr, "trace_stream_init: %s: %s\n", trace_path, strerror(errno));
    exit(1);
  }
}

void traceStreamDestroy(trace_stream_t* ts) {
  fclose(ts->trace_file);
}

trace_entry_t* traceStreamNext(trace_stream_t* ts) {
  char* buf = fgets(ts->line_buf, TRACE_LINE_LEN, ts->trace_file);
  if (!buf) {
    return NULL;
  }

  char op = buf[1];
  if (op == 'S' || op == 'L' || op == 'M') {
    sscanf(buf + 3, "%llx,%u", &ts->next_entry.addr, &ts->next_entry.size);
    ts->next_entry.op = op;
    return &ts->next_entry;
  } else {
    // This will only happen if the trace has I operations or is malformed.
    // It is safe because the EOF will exit the routine at the first condition.
    return traceStreamNext(ts);
  }
}

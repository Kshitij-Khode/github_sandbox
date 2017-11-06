/**
 * trace-stream.h
 */
#include <stdio.h>


#ifndef TRACE_STREAM_H
#define TRACE_STREAM_H

#define TRACE_LINE_LEN 1000

typedef struct trace_entry {
  char op;
  unsigned long long int addr;
  unsigned int size;
} trace_entry_t;

typedef struct trace_stream {
  char* trace_path;
  FILE* trace_file;
  char line_buf[TRACE_LINE_LEN];
  trace_entry_t next_entry;
} trace_stream_t;

/**
 * traceStreamInit
 *
 * Opens the stream.
 * This call will fail and `exit` if the provided `trace_path` does not exist.
 */
void traceStreamInit(trace_stream_t* ts, char* trace_path);

/**
 * traceStreamDestroy
 *
 * Clean up memory allocated for stream management.
 */
void traceStreamDestroy(trace_stream_t* ts);

/**
 * traceStreamNext
 *
 * Get the next entry in stream.
 * This returns a pointer to a [[trace_entry_t]] struct. This memory is
 * managed by the stream and should not be modified by the caller.
 *
 * Upon reaching the end of the trace, this function will return a `NULL`
 * pointer.
 */
trace_entry_t* traceStreamNext(trace_stream_t* ts);

#endif // TRACE_STREAM_H

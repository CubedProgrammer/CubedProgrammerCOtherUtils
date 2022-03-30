#ifndef __cplusplus
#ifndef Included_cpcou_process_streams_h
#define Included_cpcou_process_streams_h
#include<cpcio_istream.h>
#include<cpcio_ostream.h>
#include<cpcou_process_basic.h>

/**
 * Checks if stream is ready for reading, returns true if so
 */
int cpcou____proc_stream_ready(void *src);

/**
 * Reading from process streams
 */
int cpcou____proc_rd(void *src, char *buf, size_t cnt);

/**
 * Writing to process streams
 */
int cpcou____proc_wr(void *dest, const char *buf, size_t cnt);

/**
 * Close process stream
 */
int cpcou____proc_close_stream(void *obj);

/**
 * Get the stdin of a process
 */
cpcio_ostream cpcou_process_stdin(pcpcou_process proc);

/**
 * Get the stdout of a process
 */
cpcio_istream cpcou_process_stdout(pcpcou_process proc);

/**
 * Get the stderr of a process
 */
cpcio_istream cpcou_process_stderr(pcpcou_process proc);

#endif
#endif

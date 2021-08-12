#ifndef __cplusplus
#ifndef Included_header_only_cpcou_process_streams_h
#define Included_header_only_cpcou_process_streams_h
#ifdef _WIN32
#else
#include<unistd.h>
#endif
#include<cpcou_process_streams.h>

/**
 * Reading from process streams
 */
int cpcou____proc_rd(void *src, char *buf, size_t cnt)
{
#ifdef _WIN32
#else
	return read(*(int*)src, buf, cnt);
#endif
}

/**
 * Writing to process streams
 */
int cpcou____proc_wr(void *dest, const char *buf, size_t cnt)
{
#ifdef _WIN32
#else
	return write(*(int*)dest, buf, cnt);
#endif
}

/**
 * Close process stream
 */
int cpcou____proc_close_stream(void *obj)
{
#ifdef _WIN32
#else
	return close(*(int*)obj);
#endif
}

/**
 * Get the stdin of a process
 */
cpcio_ostream cpcou_process_stdin(pcpcou_process proc)
{
#ifdef _WIN32
	return cpcio_open_ostream(proc->pstdin, cpcou____proc_wr, cpcou____proc_close_stream);
#else
	return cpcio_open_ostream(&proc->pstdin, cpcou____proc_wr, cpcou____proc_close_stream);
#endif
}

/**
 * Get the stdout of a process
 */
cpcio_istream cpcou_process_stdout(pcpcou_process proc)
{
#ifdef _WIN32
	return cpcio_open_istream(proc->pstdout, cpcou____proc_rd, cpcou____proc_close_stream);
#else
	return cpcio_open_istream(&proc->pstdout, cpcou____proc_rd, cpcou____proc_close_stream);
#endif
}

/**
 * Get the stderr of a process
 */
cpcio_istream cpcou_process_stderr(pcpcou_process proc)
{
#ifdef _WIN32
	return cpcio_open_istream(proc->pstderr, cpcou____proc_rd, cpcou____proc_close_stream);
#else
	return cpcio_open_istream(&proc->pstderr, cpcou____proc_rd, cpcou____proc_close_stream);
#endif
}

#endif
#endif

#ifndef __cplusplus
#ifndef Included_header_only_cpcou_process_streams_h
#define Included_header_only_cpcou_process_streams_h
#ifdef _WIN32
#include<windows.h>
#else
#include<sys/select.h>
#include<unistd.h>
#endif
#include<cpcou_process_streams.h>

/**
 * Checks if stream is ready for reading, returns true if so
 */
int cpcou____proc_stream_ready(void *src)
{
#ifdef _WIN32
	return 0;
#else
	int *filedesp = src, filedes = *filedesp;
	fd_set fds, *fdsp = &fds;
	FD_ZERO(fdsp);
	FD_SET(filedes, fdsp);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	return select(filedes + 1, fdsp, NULL, NULL, &tv);
#endif
}

/**
 * Reading from process streams
 */
int cpcou____proc_rd(void *src, char *buf, size_t cnt)
{
#ifdef _WIN32
	DWORD rd;
	ReadFile(src, buf, cnt, &rd, NULL);
	return rd;
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
	DWORD wr;
	WriteFile(dest, buf, cnt, &wr, NULL);
	return wr;
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
	return CloseHandle(obj) == 0 ? -1 : 0;
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
	cpcio_istream is = cpcio_open_istream(&proc->pstdout, cpcou____proc_rd, cpcou____proc_close_stream);
	is->ready = cpcou____proc_stream_ready;
	cpcio_toggle_buf_is(is);
	return is;
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
	cpcio_istream is = cpcio_open_istream(&proc->pstderr, cpcou____proc_rd, cpcou____proc_close_stream);
	is->ready = cpcou____proc_stream_ready;
	cpcio_toggle_buf_is(is);
	return is;
#endif
}

#endif
#endif

#ifndef __cplusplus
#ifndef Included_cpcou_process_basic_h
#define Included_cpcou_process_basic_h
#ifdef _WIN32
#include<windows.h>
#endif
#include<cpcou_file_system.h>
#define CPCOU_STDOUT_APPEND 0x01
#define CPCOU_STDERR_APPEND 0x02

typedef int cpcou_pid_t;

typedef struct cpcou____process
{
	cpcou_pid_t id;
	char name[MAX_PATH_LEN];
#ifdef _WIN32
	HANDLE
#else
	int
#endif
	pstdin, pstdout, pstderr;
}cpcou_process, *pcpcou_process;

/**
 * Returns false if process referred to by id is not running, true if it is
 */
int cpcou_proc_alive(cpcou_pid_t id);

/**
 * Get the executable file of a process
 */
size_t cpcou_proc_exe(cpcou_pid_t id, char *restrict cbuf, size_t sz);

/**
 * Wait for a child process to exit, returns exit code
 */
int cpcou_wait_process(cpcou_pid_t id);

/**
 * Creates a process whose stdin, stdout and stderr are redirected to other files
 * Use bitwise or combination of CPCOU_STDOUT_APPEND and CPCOU_STDERR_APPEND to write to the end of files
 * Use NULL as a parameter to use parent stdin, stdout, or stderr
 * To redirect output to pipes for parent process to access, use cpcou_create_process instead
 */
cpcou_process cpcou_create_process_with_redirects(const char *cmd, const char *fin, const char *fout, const char *ferr, int modes);

/**
 * Creates a process
 */
cpcou_process cpcou_create_process(const char *cmd);

/**
 * Get all processes, returns the number of processes found
 */
size_t cpcou_get_processes(pcpcou_process ps);

/**
 * End the life of a process by its id
 */
void cpcou_kill_process(cpcou_pid_t pid);

#endif
#endif

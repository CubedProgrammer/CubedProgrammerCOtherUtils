#ifndef __cplusplus
#ifndef Included_cpcou_process_basic_h
#define Included_cpcou_process_basic_h
#ifdef _WIN32
#include<windows.h>
#endif
#include<cpcou_file_system.h>

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
 * Wait for a child process to exit, returns exit code
 */
int cpcou_wait_process(cpcou_pid_t id);

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

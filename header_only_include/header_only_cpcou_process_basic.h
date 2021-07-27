#ifndef __cplusplus
#ifndef Included_header_only_cpcou_process_basic_h
#define Included_header_only_cpcou_process_basic_h
#ifdef _WIN32
#else
#include<dirent.h>
#include<signal.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cpcou_process_basic.h>

/**
 * Get all processes, returns the number of processes found
 */
size_t cpcou_get_processes(pcpcou_process ps)
{
	size_t found = 0, cmdlen = 0;
	char pdn[29];
#ifdef _WIN32
#else
	DIR *pdir = opendir("/proc");
	struct dirent *en = readdir(pdir);
	FILE *pcmd;
	while(en != NULL)
	{
		ps[found].id = atoi(en->d_name);
		strcpy(pdn, "/proc/");
		strcpy(pdn + 6, en->d_name);
		strcpy(pdn + 6 + strlen(en->d_name), "/cmdline");
		pcmd = fopen(pdn, "r");
		cmdlen = fread(ps[found].name, 1, sizeof(ps[found].name), pcmd);
		ps[found].name[cmdlen] = '\0';
		fclose(pcmd);
		en = readdir(pdir);
		++found;
	}
	closedir(pdir);
#endif
	return found;
}

/**
 * End the life of a process by its id
 */
void cpcou_kill_process(cpcou_pid_t pid)
{
#ifdef _WIN32
	HANDLE ph = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	TerminateProcess(ph, -1);
#else
	kill(pid, SIGKILL);
#endif
}
#endif
#endif

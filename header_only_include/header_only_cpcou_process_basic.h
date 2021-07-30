#ifndef __cplusplus
#ifndef Included_header_only_cpcou_process_basic_h
#define Included_header_only_cpcou_process_basic_h
#ifdef _WIN32
#else
#include<dirent.h>
#include<signal.h>
#include<unistd.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cpcou_process_basic.h>

/**
 * Creates a process
 */
cpcou_process cpcou_create_process(const char *cmd)
{
	cpcou_process proc;
	int p_in[2], p_out[2];
	int p_err[2];
	pipe(p_in);
	pipe(p_out);
	pipe(p_err);
	cpcou_pid_t pid = fork();
	if(pid == 0)
	{
		dup2(p_in[0], STDIN_FILENO);
		dup2(p_out[1], STDOUT_FILENO);
		dup2(p_err[1], STDERR_FILENO);
		system(cmd);
		exit(0);
	}
	else
	{
		close(p_in[0]);
		close(p_out[1]);
		close(p_err[1]);
		proc.id = pid;
		strcpy(proc.name, cmd);
		proc.pstdin = p_in[1];
		proc.pstdout = p_out[0];
		proc.pstderr = p_err[0];
	}
	return proc;
}

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
	char fc;
	while(en != NULL)
	{
		fc = en->d_name[0];
		if(fc > '0' && fc < '9')
		{
			ps[found].id = atoi(en->d_name);
			strcpy(pdn, "/proc/");
			strcpy(pdn + 6, en->d_name);
			strcpy(pdn + 6 + strlen(en->d_name), "/cmdline");
			pcmd = fopen(pdn, "r");
			cmdlen = fread(ps[found].name, 1, sizeof(ps[found].name), pcmd);
			ps[found].name[cmdlen] = '\0';
			fclose(pcmd);
			ps[found].pstdin = -1;
			ps[found].pstdout = -1;
			ps[found].pstderr = -1;
			++found;
		}
		en = readdir(pdir);
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

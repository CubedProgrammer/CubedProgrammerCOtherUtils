#ifndef __cplusplus
#ifndef Included_header_only_cpcou_process_basic_h
#define Included_header_only_cpcou_process_basic_h
#ifdef _WIN32
#include<windows.h>
#include<psapi.h>
#else
#include<dirent.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cpcou_misc_utils.h>
#include<cpcou_process_basic.h>

/**
 * Returns false if process referred to by id is not running, true if it is
 */
int cpcou_proc_alive(cpcou_pid_t id)
{
#ifdef _WIN32
	HANDLE ph = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
	DWORD ev;
	GetExitCodeProcess(ph, &ev);
	return ev == STILL_ALIVE;
#else
	char path[17];
	strcpy(path, "/proc/");
	cpcou_itoa(id, path + 6, 10);
	DIR *dir = opendir(path);
	if(dir != NULL)
		closedir(dir);
	return dir != NULL;
#endif
}

/**
 * Get the executable file of a process
 */
size_t cpcou_proc_exe(cpcou_pid_t id, char *restrict cbuf, size_t sz)
{
#ifdef _WIN32
	HANDLE proc_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
	DWORD succ = -1;
	if(proc_handle)
	{
		HMODULE hmod;
		DWORD needed;
		EnumProcessModules(proc_handle, &hmod, 1, &needed);
		succ = GetModuleFileNameA(hmod, cbuf, sz);
	}
	return succ;
#else
	char idstr[11];
	size_t ind = 0;
	while(id != 0)
	{
		idstr[ind] = '0' + id % 10;
		id /= 10;
		++ind;
	}
	idstr[ind] = '\0';
	cpcou_reverse_arr(idstr, ind);
	// process path
	char pp[21];
	strcpy(pp, "/proc/");
	strcpy(pp + 6, idstr);
	strcpy(pp + ind + 6, "/exe");
	return readlink(pp, cbuf, sz);
#endif
}

/**
 * Wait for a child process to exit, returns exit code
 */
int cpcou_wait_process(cpcou_pid_t id)
{
#ifdef _WIN32
	HANDLE ph = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, id);
	WaitForSingleObject(ph, INFINITE);
	DWORD status;
	GetExitCodeProcess(ph, &status);
#else
	int status;
	waitpid(id, &status, 0);
	status = WEXITSTATUS(status);
#endif
	return status;
}

/**
 * Creates a process
 */
cpcou_process cpcou_create_process(const char *cmd)
{
	cpcou_process proc;
#ifdef _WIN32
	HANDLE p_in[2], p_out[2];
	HANDLE p_err[2];
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle = TRUE;
	CreatePipe(p_in, p_in + 1, &attr, 0);
	CreatePipe(p_out, p_out + 1, &attr, 0);
	CreatePipe(p_err, p_err + 1, &attr, 0);
	SetHandleInformation(p_in[1], HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(p_out[0], HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(p_err[0], HANDLE_FLAG_INHERIT, 0);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	si.hStdInput = p_in[0];
	si.hStdOutput = p_out[1];
	si.hStdError = p_err[1];
	si.dwFlags |= STARTF_USESTDHANDLES;
	char sent_cmd[MAX_PATH];
	strcpy(sent_cmd, cmd);
	BOOL succ = CreateProcess(NULL, sent_cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if(!succ)
		puts("Process could not be created.");
	CloseHandle(p_in[0]);
	CloseHandle(p_out[1]);
	CloseHandle(p_err[1]);
	proc.id = pi.dwProcessId;
	HANDLE ph = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, proc.id);
	HMODULE mod;
	DWORD tmp;
	if(EnumProcessModules(ph, &mod, sizeof(mod), &tmp))
		GetModuleBaseName(ph, mod, proc.name, sizeof(proc.name));
	proc.pstdin = p_in[1];
	proc.pstdout = p_out[0];
	proc.pstderr = p_err[0];
#else
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
		exit(WEXITSTATUS(system(cmd)));
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
#endif
	return proc;
}

/**
 * Get all processes, returns the number of processes found
 */
size_t cpcou_get_processes(pcpcou_process ps)
{
	size_t found = 0;
#ifdef _WIN32
	DWORD pids[1024], sz;
	EnumProcesses(pids, sizeof(pids), &sz);
	DWORD cnt = sz / sizeof(DWORD);
	HANDLE ph;
	HMODULE mod;
	DWORD tmp;
	for(DWORD i = 0; i < cnt; ++i)
	{
		ph = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pids[i]);
		if(EnumProcessModules(ph, &mod, sizeof(mod), &tmp))
			GetModuleBaseName(ph, mod, ps[i].name, sizeof(ps[i].name));
		ps[i].id = pids[i];
		ps[i].pstdin = NULL;
		ps[i].pstdout = NULL;
		ps[i].pstderr = NULL;
		CloseHandle(ph);
	}
	found = cnt;
#else
	size_t cmdlen;
	char pdn[29];
	DIR *pdir = opendir("/proc");
	struct dirent *en = readdir(pdir);
	FILE *pcmd;
	char fc;
	while(en != NULL)
	{
		fc = en->d_name[0];
		if(fc >= '0' && fc <= '9')
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

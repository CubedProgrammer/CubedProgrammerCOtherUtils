#ifndef __cplusplus
#ifndef Included_header_only_cpcou_file_system_h
#define Included_header_only_cpcou_file_system_h
#ifdef __linux__
#include<dirent.h>
#include<unistd.h>
#elif defined _WIN32
#include<windows.h>
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<cpcou_file_system.h>

/**
 * Gets what is inside a folder, returned pointer will be heap allocated
 * So are pointers pointed to by the returned pointer
 * The last string in the array of strings will be NULL
 * DO NOT free the individual char pointers, ONLY free the char **
 */
char **cpcou_folder_insides(const char *name)
{
	char **insides;
	size_t capa = 5, ocapa = 3, lsz = 0;
	char **intmp = malloc(capa * sizeof(char*));
#ifdef __linux__
	DIR *dir = opendir(name);
	struct dirent *en = readdir(dir);
	while(en)
	{
		if(lsz == capa)
		{
			intmp = realloc(intmp, (capa + ocapa) * sizeof(char*));
			capa += ocapa;
			ocapa = lsz;
		}
		intmp[lsz] = malloc(1 + strlen(en->d_name));
		strcpy(intmp[lsz], en->d_name);
		++lsz;
		en = readdir(dir);
	}
	closedir(dir);
#elif defined _WIN32
	size_t namlen = strlen(name);
	char *wcsearch = malloc(namlen + 3);
	strcpy(wcsearch, name);
	wcsearch[namlen] = '\\';
	wcsearch[namlen + 1] = '*';
	wcsearch[namlen + 2] = '\0';
	WIN32_FIND_DATA dat;
	HANDLE fh = FindFirstFileA(wcsearch, &dat);
	if(fh != INVALID_HANDLE_VALUE)
	{
		intmp[lsz] = malloc(1 + strlen(dat.cFileName));;
		strcpy(intmp[lsz], dat.cFileName);
		++lsz;
		while(FindNextFileA(fh, &dat))
		{
			if(lsz == capa)
			{
				intmp = realloc(intmp, (capa + ocapa) * sizeof(char*));
				capa += ocapa;
				ocapa = lsz;
			}
			intmp[lsz] = malloc(1 + strlen(dat.cFileName));;
			strcpy(intmp[lsz], dat.cFileName);
			++lsz;
		}
	}
	free(wcsearch);
#endif
	size_t totlen = 0;
	for(size_t i = 0; i < lsz; ++i)
	{
		if(strcmp(intmp[i], ".") != 0 && strcmp(intmp[i], "..") != 0)
			totlen += strlen(intmp[i]) + 1;
	}
	// lsz, take away . and .., add NULL
	insides = malloc(totlen + (lsz - 1) * sizeof(char*));
	size_t pos = 0;
	char *next = (char*)(insides + lsz - 1);
	for(size_t i = 0; i < lsz; ++i)
	{
		if(strcmp(intmp[i], ".") != 0 && strcmp(intmp[i], "..") != 0)
		{
			strcpy(next, intmp[i]);
			insides[pos] = next;
			next += strlen(next) + 1;
			++pos;
		}
		free(intmp[i]);
	}
	insides[pos] = NULL;
	free(intmp);
	return insides;
}

/**
 * Moves a file to a new location, overwriting anything that was previously there.
 */
int cpcou_move_file(const char *old, const char *new)
{
	cpcou_delete_file(new);
#ifdef __linux__
	return rename(old, new);
#elif defined _WIN32
	return MoveFile(old, new) == 0 ? -1 : 0;
#endif
}

/**
 * Deletes a file or a directory
 */
int cpcou_delete_file(const char *file)
{
#ifdef __linux__
	if(cpcou_file_type(file) == CPCOU_DIRECTORY)
	{
		return 0;
	}
	else
		return unlink(file);
#elif defined _WIN32
	return DeleteFileA(file) == 0 ? -1 : 0;
#endif
}

/**
 * Gets the size of a file, in bytes
 */
long long unsigned cpcou_file_size(const char *name)
{
//#ifdef __linux__
	struct stat fstats;
	stat(name, &fstats);
	return fstats.st_size;
//#elif defined _WIN32
	/*HANDLE fhand = CreateFileA(name, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD hi, lo = GetFileSize(fhand, &hi);
	long long unsigned sz = hi;
	sz = sz << 32 + lo;
	return sz;*/
//#endif
}

/**
 * Get if the path points to a file or directory
 */
int cpcou_file_type(const char *name)
{
	int tp;
	struct stat fstats;
	stat(name, &fstats);
	if((fstats.st_mode & S_IFMT) == S_IFDIR)
		tp = CPCOU_DIRECTORY;
	else
		tp = CPCOU_FILE;
	return tp;
}

/**
 * Get the absolute file name of a file, returned string is heap allocated
 */
char *cpcou_absolute_path(const char *name)
{
	char *cbuf;
#ifdef __linux__
	cbuf = realpath(name, malloc(1 + PATH_MAX));
#elif defined _WIN32
	cbuf = malloc(1 + MAX_PATH);
	GetFullPathNameA(name, MAX_PATH, cbuf, NULL);
#endif
	return cbuf;
}

#endif
#endif

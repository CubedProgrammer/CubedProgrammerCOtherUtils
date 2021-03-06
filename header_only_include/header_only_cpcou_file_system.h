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
#include<cpcou_str_algo.h>

/**
 * Root directory, it has no parent
 */
#ifdef __linux__
const char cpcou_root_dir[]="/";
#elif defined _WIN32
const char cpcou_root_dir[]="C:\\";
#endif

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
	// Windows is nice and gives you the files in alphabetical order, no need to sort
#ifndef _WIN32
	qsort(insides, lsz - 2, sizeof(char*), &cpcou____ls_strcmp_helper);
#endif
	return insides;
}

/**
 * Moves a file to a new location, overwriting anything that was previously there.
 */
int cpcou_move_file(const char *old, const char *new)
{
	if(access(new, F_OK) == 0)
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
	if(cpcou_file_type(file) == CPCOU_DIRECTORY)
	{
		int res = 0;
		char **stuff;
		size_t ssz = 0, ocapa = 3, capa = 5;
		char **stack = malloc(capa * sizeof(char*));
		stack[ssz++] = (char*)file;
		char *curr;
		size_t currlen;
		while(ssz)
		{
			--ssz;
			curr = stack[ssz];
			if(cpcou_file_type(curr) == CPCOU_DIRECTORY)
			{
				stuff = cpcou_folder_insides(curr);
				currlen = strlen(curr);
				if(*stuff == NULL)
#ifdef __linux__
					rmdir(curr);
#elif defined _WIN32
					RemoveDirectoryA(curr);
#endif
				else
				{
					stack[ssz] = malloc(currlen + 1);
					strcpy(stack[ssz], curr);
					++ssz;
					for(char **it = stuff; *it != NULL; ++it)
					{
						if(ssz == capa)
						{
							capa += ocapa;
							ocapa = ssz;
							stack = realloc(stack, capa * sizeof(char*));
						}
						stack[ssz] = malloc(currlen + strlen(*it) + 2);
						strcpy(stack[ssz], curr);
#ifdef __linux__
						stack[ssz][currlen] = '/';
#elif defined _WIN32
						stack[ssz][currlen] = '\\';
#endif
						strcpy(stack[ssz] + currlen + 1, *it);
						++ssz;
					}
				}
				free(stuff);
			}
			else
#ifdef __linux__
				res += unlink(curr);
#elif defined _WIN32
				res += DeleteFileA(curr) == 0 ? -1 : 0;
#endif
			if(curr != file)
				free(curr);
		}
		free(stack);
		return res;
	}
	else
#ifdef __linux__
		return unlink(file);
#elif defined _WIN32
		return DeleteFileA(file) == 0 ? -1 : 0;
#endif
}

/**
 * Gets the last access time in milliseconds since epoch
 */
time_t cpcou_last_access(const char *name)
{
#ifdef __linux__
	struct stat fstats;
	stat(name, &fstats);
	// linux kernel 2.6 has been around since 2004, please update to use the newer features
	return fstats.st_atim.tv_sec * 1000 + fstats.st_atim.tv_nsec / 1000000;
#elif defined _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	LARGE_INTEGER num;
	num.u.LowPart = dat.ftLastAccessTime.dwLowDateTime;
	num.u.HighPart = dat.ftLastAccessTime.dwHighDateTime;
	num.QuadPart -= 116444736000000000;
	return num.QuadPart / 10000;
#endif
}

/**
 * Gets the last modify time in milliseconds since epoch
 */
time_t cpcou_last_modify(const char *name)
{
#ifdef __linux__
	struct stat fstats;
	stat(name, &fstats);
	// linux kernel 2.6 has been around since 2004, please update to use the newer features
	return fstats.st_mtim.tv_sec * 1000 + fstats.st_mtim.tv_nsec / 1000000;
#elif defined _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	LARGE_INTEGER num;
	num.u.LowPart = dat.ftLastWriteTime.dwLowDateTime;
	num.u.HighPart = dat.ftLastWriteTime.dwHighDateTime;
	num.QuadPart -= 116444736000000000;
	return num.QuadPart / 10000;
#endif
}

/**
 * Gets the last status change time in milliseconds since epoch
 * Only for linux
 */
time_t cpcou_last_stchange(const char *name)
{
#ifdef __linux__
	struct stat fstats;
	stat(name, &fstats);
	// linux kernel 2.6 has been around since 2004, please update to use the newer features
	return fstats.st_ctim.tv_sec * 1000 + fstats.st_ctim.tv_nsec / 1000000;
#else
	return-1;
#endif
}

/**
 * Gets the creation time of a file in milliseconds since epoch
 */
time_t cpcou_create_time(const char *name)
{
#ifdef _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	LARGE_INTEGER num;
	num.u.LowPart = dat.ftCreationTime.dwLowDateTime;
	num.u.HighPart = dat.ftCreationTime.dwHighDateTime;
	num.QuadPart -= 116444736000000000;
	return num.QuadPart / 10000;
#else
	return-1;
#endif
}

/**
 * Get the size of all files in a folder
 */
size_t cpcou_folder_size(const char *name)
{
	size_t sz = 0;
	char **stuff;
	size_t ssz = 0, ocapa = 3, capa = 5;
	char **stack = malloc(capa * sizeof(char*));
	stack[ssz++] = (char*)name;
	char *curr;
	size_t currlen;
	while(ssz)
	{
		--ssz;
		curr = stack[ssz];
		if(cpcou_file_type(curr) == CPCOU_DIRECTORY)
		{
			stuff = cpcou_folder_insides(curr);
			currlen = strlen(curr);
			for(char **it = stuff; *it != NULL; ++it)
			{
				if(ssz == capa)
				{
					capa += ocapa;
					ocapa = ssz;
					stack = realloc(stack, capa * sizeof(char*));
				}
				stack[ssz] = malloc(currlen + strlen(*it) + 2);
				strcpy(stack[ssz], curr);
#ifdef __linux__
				stack[ssz][currlen] = '/';
#elif defined _WIN32
				stack[ssz][currlen] = '\\';
#endif
				strcpy(stack[ssz] + currlen + 1, *it);
				++ssz;
			}
			free(stuff);
		}
		else
			sz += cpcou_file_size(curr);
		if(curr != name)
			free(curr);
	}
	free(stack);
	return sz;
}

/**
 * Get the number of files and folders in a folder
 */
size_t cpcou_file_count(const char *name)
{
	size_t sz = 0;
	char **stuff;
	size_t ssz = 0, ocapa = 3, capa = 5;
	char **stack = malloc(capa * sizeof(char*));
	stack[ssz++] = (char*)name;
	char *curr;
	size_t currlen;
	while(ssz)
	{
		--ssz;
		curr = stack[ssz];
		if(cpcou_file_type(curr) == CPCOU_DIRECTORY)
		{
			stuff = cpcou_folder_insides(curr);
			currlen = strlen(curr);
			for(char **it = stuff; *it != NULL; ++it)
			{
				if(ssz == capa)
				{
					capa += ocapa;
					ocapa = ssz;
					stack = realloc(stack, capa * sizeof(char*));
				}
				stack[ssz] = malloc(currlen + strlen(*it) + 2);
				strcpy(stack[ssz], curr);
#ifdef __linux__
				stack[ssz][currlen] = '/';
#elif defined _WIN32
				stack[ssz][currlen] = '\\';
#endif
				strcpy(stack[ssz] + currlen + 1, *it);
				++ssz;
			}
			free(stuff);
		}
		++sz;
		if(curr != name)
			free(curr);
	}
	free(stack);
	return sz;
}

/**
 * Copies a file into another location, whatever was there before is erased for good.
 */
int cpcou_copy_file(const char *from, const char *to)
{
	cpcou_delete_file(to);
#ifdef _WIN32
	return CopyFile(from, to, FALSE) ? 0 : 1;
#else
	size_t size = cpcou_file_size(from);
	char *cbuf = malloc(size);
	FILE *fhand = fopen(from, "rb");
	fread(cbuf, sizeof(char), size, fhand);
	fclose(fhand);
	fhand = fopen(to, "wb");
	size_t r = fwrite(cbuf, sizeof(char), size, fhand);
	fclose(fhand);
	free(cbuf);
	return r == size ? 0 : 1;
#endif
}

/**
 * Creates a folder, parent folder must exist
 */
void cpcou_create_folder(const char *name)
{
#ifdef _WIN32
	CreateDirectoryA(name, NULL);
#else
	mkdir(name, S_IRWXU);
#endif
}


/**
 * Creates a folder, parent folder need not exist
 */
void cpcou_create_folders(const char *name)
{
	char *folders = malloc(strlen(name) + 1);
	strcpy(folders, name);
	char *it = folders + 1;
	while(*it)
	{
#ifdef _WIN32
		if(*it == '\\' || *it == '/')
#else
		if(*it == '/')
#endif
		{
			*it = '\0';
			cpcou_create_folder(folders);
			*it = '/';
		}
		++it;
	}
	cpcou_create_folder(folders);
	free(folders);
}

/**
 * Makes a file into a hidden file
 */
void cpcou_hide_file(const char *name)
{
#ifdef _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	dat.dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
	SetFileAttributesA(name, dat.dwFileAttributes);
#else
	size_t len = strlen(name), idx = len;
	const char *nbegin = name;
	while(idx > 0)
	{
		if(name[idx] == '/')
			nbegin = name + idx + 1;
		--idx;
	}
	if(*nbegin != '.')
	{
		char *new = malloc(len + 2);
		strcpy(new, name);
		char *nnbegin = new + (nbegin - name);
		*nnbegin = '.';
		strcpy(nnbegin + 1, nbegin);
		cpcou_move_file(name, new);
		free(new);
	}
#endif
}

/**
 * Unhides a hidden file
 */
void cpcou_unhide_file(const char *name)
{
#ifdef _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	dat.dwFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
	SetFileAttributesA(name, dat.dwFileAttributes);
#else
	size_t len = strlen(name), idx = len;
	const char *nbegin = name;
	while(idx > 0)
	{
		if(name[idx] == '/')
			nbegin = name + idx + 1;
		--idx;
	}
	if(*nbegin == '.')
	{
		char *new = malloc(len + 1);
		strcpy(new, name);
		char *nnbegin = new + (nbegin - name);
		while(*nnbegin)
		{
			*nnbegin = *(nnbegin + 1);
			++nnbegin;
		}
		cpcou_move_file(name, new);
		free(new);
	}
#endif
}

/**
 * Gets the file extension of a cpcou file info struct
 */
const char *cpcou_get_extension(const struct cpcou_file_info *dat)
{
	const char *ptr = dat->abspth + dat->plen;
	for(int64_t i = dat->plen - 1; i >= 0; --i)
	{
		if(dat->abspth[i] == '.')
		{
			ptr = dat->abspth + i + 1;
			i = -69;
		}
	}
	return ptr;
}

/**
 * Get the file information of files in a folder
 */
struct cpcou_file_info *cpcou_folder_inside_file_info(const char *name, size_t * szp)
{
	struct cpcou_file_info *insides;
	size_t capa = 5, ocapa = 3, lsz = 0;
	struct cpcou_file_info *intmp = malloc(capa * sizeof(struct cpcou_file_info));
	char *path;
	size_t namlen = strlen(name);
	if(name[namlen-1] == '/')
		--namlen;
#ifdef _WIN32
	char *wcs = malloc(namlen + 3);
	strcpy(wcs, name);
	strcpy(wcs + namlen, "\\*");
	WIN32_FIND_DATA fd;
	HANDLE fh = FindFirstFileA(wcs, &fd);
	free(wcs);
#else
	DIR *dir = opendir(name);
	struct dirent *en = readdir(dir);
#endif
#ifdef _WIN32
	do
#else
	while(en)
#endif
	{
		if(lsz == capa)
		{
			capa += ocapa;
			intmp = realloc(intmp, capa * sizeof(struct cpcou_file_info));
			ocapa = lsz;
		}
#ifdef _WIN32
		path = malloc(namlen + 2 + strlen(fd.cFileName));
#else
		path = malloc(namlen + 2 + strlen(en->d_name));
#endif
		strcpy(path, name);
#ifdef _WIN32
		path[namlen] = '\\';
		strcpy(path + namlen + 1, fd.cFileName);
#else
		path[namlen] = '/';
		strcpy(path + namlen + 1, en->d_name);
#endif
		cpcou_file_info(path, intmp + lsz);
		++lsz;
#ifndef _WIN32
		en = readdir(dir);
#endif
		free(path);
	}
#ifdef _WIN32
	while(FindNextFileA(fh, &fd));
#endif
	insides = malloc(lsz * sizeof(struct cpcou_file_info));
	memcpy(insides, intmp, lsz * sizeof(struct cpcou_file_info));
	free(intmp);
	if(szp)
		*szp = lsz;
	return insides;
}

/**
 * Gets information about a file.
 */
void cpcou_file_info(const char *name, struct cpcou_file_info *cfi)
{
#ifdef _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	cfi->last_access_time = ((LONGLONG)dat.ftLastAccessTime.dwHighDateTime << 32) + dat.ftLastAccessTime.dwLowDateTime;
	cfi->last_access_time -= 116444736000000000;
	cfi->last_access_time /= 10000;
	cfi->crtime = ((LONGLONG)dat.ftCreationTime.dwHighDateTime << 32) + dat.ftCreationTime.dwLowDateTime;
	cfi->crtime -= 116444736000000000;
	cfi->crtime /= 10000;
	cfi->last_modify_time = ((LONGLONG)dat.ftLastWriteTime.dwHighDateTime << 32) + dat.ftLastWriteTime.dwLowDateTime;
	cfi->last_modify_time -= 116444736000000000;
	cfi->last_modify_time /= 10000;
	cfi->last_stchange_time = -1;
	if((dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		cfi->file_or_dir = CPCOU_DIRECTORY;
	else
		cfi->file_or_dir = CPCOU_FILE;
#else
	struct stat dat;
	stat(name, &dat);
	cfi->last_access_time = dat.st_atim.tv_sec * 1000 + dat.st_atim.tv_nsec / 1000000;
	cfi->last_modify_time = dat.st_mtim.tv_sec * 1000 + dat.st_mtim.tv_nsec / 1000000;
	cfi->last_stchange_time = dat.st_ctim.tv_sec * 1000 + dat.st_ctim.tv_nsec / 1000000;
	cfi->crtime = -1;
	if((dat.st_mode & S_IFMT) == S_IFDIR)
		cfi->file_or_dir = CPCOU_DIRECTORY;
	else
		cfi->file_or_dir = CPCOU_FILE;
#endif
	if(cfi->file_or_dir == CPCOU_DIRECTORY)
		cfi->insides = cpcou_folder_insides(name);
	else
		cfi->insides = NULL;
#ifdef _WIN32
	GetFullPathNameA(name, MAX_PATH, cfi->abspth, NULL);
#else
	realpath(name, cfi->abspth);
#endif
	cfi->plen = strlen(cfi->abspth);
	if(cfi->file_or_dir == CPCOU_FILE)
	{
#ifdef _WIN32
		cfi->size = ((LONGLONG)dat.nFileSizeHigh << 32) + dat.nFileSizeLow;
		cfi->bsz = 4096;
		cfi->bcnt = (cfi->size + cfi->bsz - 1) / 4096;
#else
		cfi->size = dat.st_size;
		cfi->bsz = dat.st_blksize;
		cfi->bcnt = dat.st_blocks;
#endif
	}
	else
		cfi->size = -1;
}

/**
 * Finds a file in a directory, case sensitive for linux and insensitive for windows
 */
void cpcou_find_file(const char *name, const char *dir, char *path)
{
	char **stuff;
	size_t ssz = 0, ocapa = 3, capa = 5;
	char **stack = malloc(capa * sizeof(char*));
	stack[ssz++] = (char*)dir;
	char *curr;
	size_t currlen;
	*path = '\0';
	while(ssz)
	{
		--ssz;
		curr = stack[ssz];
		if(cpcou_file_type(curr) == CPCOU_DIRECTORY)
		{
			stuff = cpcou_folder_insides(curr);
			currlen = strlen(curr);
			for(char **it = stuff; *it != NULL; ++it)
			{

				if(ssz == capa)
				{
					capa += ocapa;
					ocapa = ssz;
					stack = realloc(stack, capa * sizeof(char*));
				}
				stack[ssz] = malloc(currlen + strlen(*it) + 2);
				strcpy(stack[ssz], curr);
#ifdef __linux__
				stack[ssz][currlen] = '/';
#elif defined _WIN32
				stack[ssz][currlen] = '\\';
#endif
				strcpy(stack[ssz] + currlen + 1, *it);
#ifdef _WIN32
#else
				if(strcmp(*it, name) == 0)
#endif
				{
#ifdef _WIN32
#else
					realpath(stack[ssz], path);
#endif
				}
				++ssz;
			}
			free(stuff);
		}
		if(curr != dir)
			free(curr);
	}
	free(stack);
}

/**
 * Get the user's home directory
 */
void cpcou_get_home(char *cbuf)
{
#ifdef _WIN32
	strcpy(cbuf, getenv("userprofile"));
#else
	strcpy(cbuf, getenv("HOME"));
#endif
}

/**
 * Gets the parent folder of a file or folder
 */
char *cpcou_file_parent(const char *name)
{
	char *pth = cpcou_absolute_path(name);
	size_t len = strlen(pth);
	for(int i = len - 1; i >= 0; --i)
	{
#ifdef __linux__
		if(pth[i] == '/')
#elif defined _WIN32
		if(pth[i] == '\\')
#endif
		{
#ifdef __linux__
			if(i == 0)
#elif defined _WIN32
			if(i == 2)
#endif
				pth[i + 1] = '\0';
			else
				pth[i] = '\0';
			i = -1;
		}
	}
	return pth;
}

/**
 * Gets the size of a file, in bytes
 */
long long unsigned cpcou_file_size(const char *name)
{
#ifdef __linux__
	struct stat fstats;
	stat(name, &fstats);
	return fstats.st_size;
#elif defined _WIN32
	WIN32_FIND_DATA dat;
	FindFirstFileA(name, &dat);
	long long unsigned sz = dat.nFileSizeHigh;
	sz = (sz << 32) + dat.nFileSizeLow;
	return sz;
#endif
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

/**
 * Helper function for sorting folder insides by alphabetical order
 */
int cpcou____ls_strcmp_helper(const void *x, const void *y)
{
	const char *xstr = *(const char *const*)x, *ystr = *(const char *const*)y;
	return cpcou_insens_strcmp(xstr, ystr);
}

#endif
#endif

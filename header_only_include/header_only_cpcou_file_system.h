#ifndef __cplusplus
#ifndef Included_header_only_cpcou_file_system_h
#define Included_header_only_cpcou_file_system_h
#if defined(__unix__) || defined(__APPLE__)
#include<dirent.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<unistd.h>
#include<utime.h>
#elif defined _WIN32
#include<windows.h>
#include<shlwapi.h>
#include<sys/utime.h>
#endif
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<cpcou_file_system.h>
#include<cpcou_str_algo.h>

/**
 * Root directory, it has no parent
 */
#ifndef _WIN32
const char cpcou_root_dir[]="/";
#else
const char cpcou_root_dir[]="C:\\";
#endif

/**
 * Initializes an iterator to specified directory
 * This iterator is immediately ready for use
 * Returns zero on success
 */
int cpcou_dir_iter_begin(struct cpcou_dir_iter *iter, const char *dir)
{
#ifndef _WIN32
	iter->fh = opendir(dir);
	if(iter->fh == NULL)
		return-1;
	else
		return cpcou_dir_iter_next(iter);
#else
	size_t dirlen = strlen(dir);
	char *searchstr = malloc(dirlen + 3);
	memcpy(searchdir, dir, dirlen);
	memcpy(searchdir + dirlen, "\\*", 3);
	iter->fh = FindFirstFileA(searchstr, &iter->dat);
	free(searchstr);
	iter->more = iter->fh != INVALID_HANDLE_VALUE;
	return!iter->more;
#endif
}

/**
 * Moves iterator to the next entry
 * Returns zero on success
 */
int cpcou_dir_iter_next(struct cpcou_dir_iter *iter)
{
#ifndef _WIN32
	errno = 0;
	iter->en = readdir(iter->fh);
	char selfparent = iter->en != NULL && (strcmp(iter->en->d_name, "..") == 0 || strcmp(iter->en->d_name, ".") == 0);
	while(errno == 0 && selfparent)
	{
		iter->en = readdir(iter->fh);
		selfparent = iter->en != NULL && (strcmp(iter->en->d_name, "..") == 0 || strcmp(iter->en->d_name, ".") == 0);
	}
	return(errno != 0) * -1;
#else
	return(iter->more = FindNextFileA(iter->fh, &iter->dat)) ? 0 : -1;
#endif
}

/**
 * Gets the name of the current entry of the iterator
 * Stores the name in buf as a null-terminated string
 * Returns the number of bytes written to buf, or -1 on failure.
 */
int cpcou_dir_iter_get(const struct cpcou_dir_iter *iter, char *buf)
{
#ifndef _WIN32
	if(iter->en == NULL)
		return-1;
	else
	{
		size_t len = strlen(iter->en->d_name) + 1;
		memcpy(buf, iter->en->d_name, len);
		return len;
	}
#else
	if(iter->more)
	{
		size_t len = strlen(iter->dat.cFileName) + 1;
		memcpy(buf, iter->dat.cFileName, len);
		return len;
	}
	else
		return-1;
#endif
}

/**
 * Returns one if the iterator does not point to an entry, zero otherwise
 * After calling the cpcou_dir_iter_next function on an iterator with the final entry, this will return one
 */
int cpcou_dir_iter_ended(const struct cpcou_dir_iter *iter)
{
#ifndef _WIN32
	return iter->en == NULL;
#else
	return iter->more;
#endif
}

/**
 * Frees resource used by iterator
 */
void cpcou_dir_iter_destroy(struct cpcou_dir_iter *iter)
{
#ifndef _WIN32
	if(iter->fh != NULL)
		closedir(iter->fh);
#else
	if(iter->fh != INVALID_HANDLE_VALUE)
		FindClose(iter->fh);
#endif
}

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
#ifndef _WIN32
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
#else
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
 * Moves a file to a new location, replacing the file that was previously there.
 */
int cpcou_move_file(const char *old, const char *new)
{
#ifndef _WIN32
	return rename(old, new);
#else
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
#if defined(__unix__) || defined(__APPLE__)
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
#ifndef _WIN32
						stack[ssz][currlen] = '/';
#else
						stack[ssz][currlen] = '\\';
#endif
						strcpy(stack[ssz] + currlen + 1, *it);
						++ssz;
					}
				}
				free(stuff);
			}
			else
#if defined(__unix__) || defined(__APPLE__)
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
#if defined(__unix__) || defined(__APPLE__)
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
#if defined(__unix__) || defined(__APPLE__)
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
#if defined(__unix__) || defined(__APPLE__)
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
 * Sets the last modify time of a file, returns zero on success
 */
int cpcou_set_modify_time(const char *name, time_t time)
{
	struct stat dat;
	int succ = stat(name, &dat);
	if(succ == 0)
	{
		struct utimbuf utim;
		utim.actime = dat.st_atime;
		utim.modtime = time;
		succ = utime(name, &utim);
	}
	return succ;
}

/**
 * Sets the last access time of a file, returns zero on success
 */
int cpcou_set_access_time(const char *name, time_t time)
{
	struct stat dat;
	int succ = stat(name, &dat);
	if(succ == 0)
	{
		struct utimbuf utim;
		utim.actime = time;
		utim.modtime = dat.st_mtime;
		succ = utime(name, &utim);
	}
	return succ;
}

/**
 * Sets the access and modify time of a file, returns zero on success
 */
int cpcou_set_access_and_modify(const char *name, time_t acc, time_t mod)
{
	struct utimbuf utim;
	utim.actime = acc;
	utim.modtime = mod;
	return utime(name, &utim);
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
#ifndef _WIN32
				stack[ssz][currlen] = '/';
#else
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
#ifndef _WIN32
				stack[ssz][currlen] = '/';
#else
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
#ifdef _WIN32
	return CopyFile(from, to, FALSE) ? 0 : 1;
#else
	int failed = 0;
	size_t bufsz = 1048576;
#if defined(__unix__) || defined(__APPLE__)
	int in = open(from, O_RDONLY);
	if(in != -1)
#else
	FILE *fin = fopen(from, "rb");
	if(fin != NULL)
#endif
	{
#if defined(__unix__) || defined(__APPLE__)
		int out = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if(out != -1)
#else
		FILE* fout = fopen(to, "wb");
		if(fout != NULL)
#endif
		{
#if defined(__unix__) || defined(__APPLE__)
			ssize_t br = 1, r;
#else
			void *cbuf = malloc(bufsz);
			size_t br = 1, r;
#endif
			for(r = 0; br > 0; r += br)
#if defined(__unix__) || defined(__APPLE__)
				br = sendfile(out, in, NULL, bufsz);
#else
				fwrite(cbuf, sizeof(char), br = fread(cbuf, sizeof(char), bufsz, fin), fin);
			failed = ferror(fin) || ferror(fout);
			failed *= -1;
			fclose(fout);
			free(cbuf);
#endif
#if defined(__unix__) || defined(__APPLE__)
			failed = br;
			close(out);
#endif
		}
		else
			failed = -1;
#if defined(__unix__) || defined(__APPLE__)
		close(in);
#else
		fclose(fin);
#endif
	}
	else
		failed = -1;
	return failed;
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
	for(long long i = dat->plen - 1; i >= 0; --i)
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

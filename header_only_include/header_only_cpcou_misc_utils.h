#ifndef __cplusplus
#ifndef Included_header_only_cpcou_misc_utils_h
#define Included_header_only_cpcou_misc_utils_h
#include<stdio.h>
#include<string.h>
#ifdef _WIN32
#include<windows.h>
#include<shlwapi.h>
#include<ws2tcpip.h>
#else
#include<arpa/inet.h>
#include<dirent.h>
#include<fcntl.h>
#include<netdb.h>
#include<sys/statvfs.h>
#include<unistd.h>
#endif
#include<cpcou_misc_utils.h>

#ifndef _WIN32
extern char **environ;
#endif

const char cpcou____digits[37] = "0123456789abcdefghijklmnopqrstuvwxyz";

/**
 * Set the standard input to a file, this file must exist
 */
int cpcou_set_in_file(const char *name)
{
#ifdef _WIN32
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle = FALSE;
	HANDLE fh = CreateFileA(name, GENERIC_READ, 0, &attr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	return fh != INVALID_HANDLE_VALUE ? cpcou_set_in_pipe(fh) : -1;
#else
	int fd = open(name, O_RDONLY);
	int succ = fd == -1 ? -1 : 0;
	if(succ == 0)
		succ = cpcou_set_in_pipe(fd);
	return succ;
#endif
}

/**
 * Set the standard output to a file, this file will be created if it does not exist
 * If append is true, then data will be written to the end of the file
 */
int cpcou_set_out_file(const char *name, int append)
{
#ifdef _WIN32
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle = FALSE;
	HANDLE fh = CreateFileA(name, append ? FILE_APPEND_DATA : GENERIC_WRITE, 0, &attr, PathFileExistsA(name) ? OPEN_EXISTING : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	return fh != INVALID_HANDLE_VALUE ? cpcou_set_out_pipe(fh) : -1;
#else
	int fd = open(name, O_APPEND * append | O_CREAT | O_WRONLY, S_IRWXU);
	int succ = fd == -1 ? -1 : 0;
	if(succ == 0)
		succ = cpcou_set_out_pipe(fd);
	return succ;
#endif
}

/**
 * Set the standard error to a file, this file will be created if it does not exist
 * If append is true, then data will be written to the end of the file
 */
int cpcou_set_err_file(const char *name, int append)
{
#ifdef _WIN32
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	attr.bInheritHandle = FALSE;
	HANDLE fh = CreateFileA(name, append ? FILE_APPEND_DATA : GENERIC_WRITE, 0, &attr, PathFileExistsA(name) ? OPEN_EXISTING : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	return fh != INVALID_HANDLE_VALUE ? cpcou_set_err_pipe(fh) : -1;
#else
	int fd = open(name, O_APPEND * append | O_CREAT | O_WRONLY, S_IRWXU);
	int succ = fd == -1 ? -1 : 0;
	if(succ == 0)
		succ = cpcou_set_err_pipe(fd);
	return succ;
#endif
}

/**
 * Set the standard input to a pipe
 */
int cpcou_set_in_pipe(cpcou_pipe_t pipe)
{
#ifdef _WIN32
	return SetStdHandle(STD_INPUT_HANDLE, pipe) ? 0 : -1;
#else
	return dup2(pipe, STDIN_FILENO) == -1 ? -1 : 0;
#endif
}

/**
 * Set the standard output to a pipe
 */
int cpcou_set_out_pipe(cpcou_pipe_t pipe)
{
#ifdef _WIN32
	return SetStdHandle(STD_OUTPUT_HANDLE, pipe) ? 0 : -1;
#else
	return dup2(pipe, STDOUT_FILENO) == -1 ? -1 : 0;
#endif
}

/**
 * Set the standard error to a pipe
 */
int cpcou_set_err_pipe(cpcou_pipe_t pipe)
{
#ifdef _WIN32
	return SetStdHandle(STD_ERROR_HANDLE, pipe) ? 0 : -1;
#else
	return dup2(pipe, STDERR_FILENO) == -1 ? -1 : 0;
#endif
}

/**
 * Gets the partition size and free space, returns zero on success
 * For windows, part should be a drive letter followed by :\
 * For linux, part should be sd followed by a letter, then a number, the /dev/ is implicit
 */
int cpcou_get_partition_space(const char *part, size_t *fr, size_t *tot)
{
#ifdef _WIN32
	DWORD bytes, sectors;
	DWORD dfr, dtot;
	BOOL succ = GetDiskFreeSpaceA(part, &sectors, &bytes, &dfr, &dtot);
	*fr = (size_t)bytes * sectors * dfr;
	*tot = (size_t)bytes * sectors * dtot;
	return succ ? 0 : -1;
#else
	char cbuf[20000];
	int mf = open("/proc/self/mounts", O_RDONLY);
	read(mf, cbuf, sizeof(cbuf));
	close(mf);
	char search[12];
	strcpy(search, "\n/dev/");
	strcpy(search + 6, part);
	char *ptr = strstr(cbuf, search);
	int succ = -1;
	if(ptr != NULL)
	{
		char *mpt = strchr(ptr, ' ') + 1;
		char *mptend = strchr(mpt, ' ');
		*mptend = '\0';
		struct statvfs pdat;
		statvfs(mpt, &pdat);
		*fr = pdat.f_bsize * pdat.f_bfree;
		*tot = pdat.f_frsize * pdat.f_blocks;
		succ = 0;
	}
	return succ;
#endif
}

#ifndef _WIN32
static inline char **cpcou____dev_sd_filter(int *cnt, size_t mini, size_t maxi)
{
	int len;
	size_t totlen = 0;
	*cnt = 0;
	DIR *devdir = opendir("/dev");
	struct dirent *en = readdir(devdir);
	char *partnames[100];
	while(en != NULL)
	{
		len = strlen(en->d_name);
		if(len >= mini && len <= maxi && strncmp(en->d_name, "sd", 2) == 0)
		{
			partnames[*cnt] = malloc(len + 1);
			strcpy(partnames[*cnt], en->d_name);
			++*cnt;
			totlen += len + 1;
		}
		en = readdir(devdir);
	}
	closedir(devdir);
	char **strs = malloc((*cnt + 1) * sizeof(char *) + totlen);
	char *curr = (char *)(strs + *cnt + 1);
	for(int i = 0; i < *cnt; ++i)
	{
		strs[i] = curr;
		strcpy(curr, partnames[i]);
		for(; *curr != '\0'; ++curr);
		++curr;
		free(partnames[i]);
	}
	return strs;
}
#endif

/**
 * Gets names of storage devices on the computer
 * This will be sd followed by a letter on linux, or \Device\... on windows
 */
char **cpcou_get_devices(void)
{
#ifdef _WIN32
	char devname[2000];
	int len, cnt = 0;
	size_t totlen = 0;
	HANDLE dh = FindFirstVolumeA(devname, sizeof(devname));
	char *devnames[100];
	if(dh != INVALID_HANDLE_VALUE)
	{
		do
		{
			len = strlen(devname);
			totlen += len + 1;
			devnames[cnt] = malloc(len + 1);
			strcpy(devnames[cnt], devname);
			++cnt;
		}
		while(FindNextVolumeA(dh, devname, sizeof(devname)));
	}
	char **strs = malloc((cnt + 1) * sizeof(char *) + totlen);
	char *curr = (char *)(strs + cnt + 1);
	for(int i = 0; i < cnt; ++i)
	{
		strs[i] = curr;
		strcpy(curr, devnames[i]);
		for(; *curr != '\0'; ++curr);
		++curr;
		free(devnames[i]);
	}
#else
	int cnt;
	char **strs = cpcou____dev_sd_filter(&cnt, 3, 3);
#endif
	strs[cnt] = NULL;
	return strs;
}

/**
 * Gets names of partitions of devices on the computer
 * This will be sd.. on linux, or drive letters on windows
 */
char **cpcou_get_partitions(void)
{
#ifdef _WIN32
	int len, cnt = 0;
	size_t totlen = 0;
	char dletters[100];
	GetLogicalDriveStringsA(sizeof(dletters), dletters);
	for(char *it = dletters; *it != '\0';)
	{
		len = 0;
		for(; *it != '\0'; ++it, ++len);
		totlen += len + 1;
		++it;
		++cnt;
	}
	char **strs = malloc((cnt + 1) * sizeof(char *) + totlen);
	char *curr = (char *)(strs + cnt + 1);
	cnt = 0;
	for(char *it = dletters; *it != '\0';)
	{
		strcpy(curr, it);
		strs[cnt] = curr;
		for(; *it != '\0'; ++it, ++curr);
		++it;
		++curr;
		++cnt;
	}
#else
	int cnt;
	char **strs = cpcou____dev_sd_filter(&cnt, 4, 5);
#endif
	strs[cnt] = NULL;
	return strs;
}

/**
 * Converts long long unsigned int to a string
 * Base must be between 2 and 36, inclusive
 */
size_t cpcou_llutoa(cpcou_llu num, char *restrict str, int base)
{
	int dig, ind = 0;
	while(num > 0)
	{
		dig = num % base;
		num /= base;
		str[ind] = cpcou____digits[dig];
		++ind;
	}
	char tmp;
	for(int i = 0; i < ind / 2; ++i)
	{
		tmp = str[i];
		str[i] = str[ind - i - 1];
		str[ind - i - 1] = tmp;
	}
	str[ind] = '\0';
	return ind;
}

/**
 * Converts long long int to a string
 * Base must be between 2 and 36, inclusive
 */
size_t cpcou_lltoa(cpcou_lli num, char *restrict str, int base)
{
	int dig, ind = 0;
	int nega = 0;
	if(num < 0)
	{
		num = -num;
		*str = '-';
		++str;
		nega = 1;
	}
	while(num > 0)
	{
		dig = num % base;
		num /= base;
		str[ind] = cpcou____digits[dig];
		++ind;
	}
	char tmp;
	for(int i = 0; i < ind / 2; ++i)
	{
		tmp = str[i];
		str[i] = str[ind - i - 1];
		str[ind - i - 1] = tmp;
	}
	str[ind] = '\0';
	return ind + nega;
}

/**
 * Get names of environment variables, returned pointer is heap allocated
 * Do not attempt to free the pointers to individual strings
 */
char **cpcou_getenv_names(void)
{
	char **env;
#ifdef _WIN32
	env = _environ;
#else
	env = environ;
#endif
	size_t cnt = 0, len = 0;
	for(char **it = env; *it != NULL; ++it)
		++cnt, len += strchr(*it, '=') - *it;
	char **res = malloc(len + cnt + (cnt + 1) * sizeof(char *));
	char *curr = (char *)(res + cnt + 1);
	size_t ind = 0;
	for(char **it = env; *it != NULL; ++it)
	{
		len = strchr(*it, '=') - *it;
		memcpy(curr, *it, len);
		curr[len] = '\0';
		res[ind] = curr;
		curr += len + 1;
		++ind;
	}
	res[ind] = NULL;
	return res;
}

/**
 * Gets the ip address by hostname, cbuf should be able to store at least sixteen characters
 * Returns zero on success
 */
int cpcou_host_to_ip(const char *restrict hostname, char *restrict cbuf)
{
	struct addrinfo hints;
	struct addrinfo *addresses;
	memset(&hints, 0, sizeof(hints));
	int succ = getaddrinfo(hostname, NULL, &hints, &addresses);
	struct sockaddr_in *addr;
	if(succ == 0)
	{
		for(struct addrinfo *n = addresses; n != NULL; n = n->ai_next)
		{
			if(((struct sockaddr_in *)n->ai_addr)->sin_addr.s_addr)
				addr = (struct sockaddr_in *)n->ai_addr;
		}
		char *addrname = inet_ntoa(addr->sin_addr);
		strcpy(cbuf, addrname);
	}
	return succ;
}

/**
 * Creates a pipe, returns zero on success
 */
int cpcou_pipe(cpcou_pipe_t *r, cpcou_pipe_t *w)
{
#ifdef _WIN32
	return CreatePipe(r, w, NULL, 0) == 0 ? -1 : 0;
#elif defined(__linux__) || defined(__APPLE__)
	int pipes[2];
	int succ = pipe(pipes);
	*r = pipes[0];
	*w = pipes[1];
	return succ;
#else
	cpcou_pipe_t p = malloc(sizeof(*p));
	if(p == NULL)
		return-1;
	else
	{
		*r = p;
		*w = p;
		p->sz = 3 << 20;
		p->dat = malloc(p->sz);
		p->ind = 0;
		p->cnt = 0;
		return p->dat == NULL ? -1 : 0;
	}
#endif
}

/**
 * Read from a pipe, returns number of bytes read
 */
size_t cpcou_pipe_read(cpcou_pipe_t pipe, void *buf, size_t sz)
{
#ifdef _WIN32
	DWORD bc;
	ReadFile(pipe, buf, sz, &bc, NULL);
	return bc;
#elif defined(__linux__) || defined(__APPLE__)
	return read(pipe, buf, sz);
#else
	struct timespec second;
	second.tv_sec = 1;
	second.tv_nsec = 0;
	while(pipe->cnt == 0)
		thrd_sleep(&second, NULL);
	size_t bc = sz < pipe->cnt ? sz : pipe->cnt;
	memcpy(buf, (char*)pipe->dat + pipe->ind, bc);
	pipe->cnt -= bc;
	pipe->ind += bc;
	if(pipe->cnt == 0)
		pipe->ind = 0;
	return bc;
#endif
}

/**
 * Writes to a pipe, returns number of bytes wrote
 */
size_t cpcou_pipe_write(cpcou_pipe_t pipe, const void *buf, size_t sz)
{
#ifdef _WIN32
	DWORD bc;
	WriteFile(pipe, buf, sz, &bc, NULL);
	return bc;
#elif defined(__linux__) || defined(__APPLE__)
	return write(pipe, buf, sz);
#else
	if(pipe->sz - pipe->cnt < sz)
	{
		void *tmp = malloc(pipe->cnt + sz);
		pipe->sz = sz + pipe->cnt;
		memcpy(tmp, (char*)pipe->dat + pipe->ind, pipe->cnt);
		free(pipe->dat);
		pipe->dat = tmp;
		pipe->ind = 0;
	}
	else if(pipe->sz - pipe->cnt - pipe->ind < sz)
	{
		volatile char *cbuf = pipe->dat;
		for(size_t i = 0; i < pipe->cnt; ++i)
			cbuf[i] = cbuf[i + pipe->ind];
		pipe->ind = 0;
	}
	memcpy((char*)pipe->dat + pipe->cnt + pipe->ind, buf, sz);
	pipe->cnt += sz;
	return sz;
#endif
}

/**
 * Closes pipes, return zero on success
 */
int cpcou_close_pipe(cpcou_pipe_t pipe)
{
#ifdef _WIN32
	return CloseHandle(pipe) == 0 ? -1 : 0;
#elif defined(__linux__) || defined(__APPLE__)
	return close(pipe);
#else
	free(pipe->dat);
	free(pipe);
	return 0;
#endif
}

/**
 * Remove characters from the last line of console output
 */
void cpcou_stdout_erase(size_t chars)
{
	for(size_t i = 0; i < chars; ++i)
		fputc('\b', stdout);
	for(size_t i = 0; i < chars; ++i)
		fputc(' ', stdout);
	for(size_t i = 0; i < chars; ++i)
		fputc('\b', stdout);
}

/**
 * Get the name of the running program
 */
size_t cpcou_get_exe(char *buf, size_t sz)
{
#ifdef _WIN32
	return GetModuleFileNameA(GetModuleHandleA(NULL), buf, sz);
#else
	return readlink("/proc/self/exe", buf, sz);
#endif
}

/**
 * Swaps the data in two blocks of memory
 */
void cpcou_swap_mem(void *x, void *y, size_t sz)
{
	if(sz < 1000)
	{
		char unsigned tmp[1000];
		memcpy(tmp, x, sz);
		memcpy(x, y, sz);
		memcpy(y, tmp, sz);
	}
	else
	{
		char unsigned *m = x, *n = y;
		char unsigned tmp;
		for(size_t i = 0; i < sz; i++)
		{
			tmp = m[i];
			m[i] = n[i];
			n[i] = tmp;
		}
	}
}

/**
 * Checks if two char arrays are permutations
 */
int cpcou_arr_perms(const char *x, const char *y)
{
	long cnts[256];
	for(size_t i = 0; x[i]; ++i)
		++cnts[x[i]];
	for(size_t i = 0; y[i]; ++i)
		--cnts[y[i]];
	size_t diff = 0;
	for(int i = 1; i < 256; ++i)
	{
		if(cnts[i] < 0)
			diff -= cnts[i];
		else
			diff += cnts[i];
	}
	return diff;
}

/**
 * Filling a block of memory with a value
 */
void cpcou_fill_mem(void *ptr, const void *val, size_t size, size_t cnt)
{
	char unsigned *cptr = ptr;
	for(size_t i = 0; i < cnt; ++i)
		memcpy(cptr + size * i, val, size);
}

/**
 * Allocates memory for a two-dimensional array
 */
void *cpcou_malloc_2d(size_t width, size_t height)
{
	void *mem = malloc(width * height + sizeof(void*) * height);
	char unsigned **cmem = mem;
	for(size_t i = 0; i < height; ++i)
		cmem[i] = (char unsigned*)(cmem + height) + width * i;
	return mem;
}

/**
 * Initialize a java.util.Random with a seed
 */
struct cpcou_lcg cpcou_init_jur(long seed)
{
	struct cpcou_lcg rand;
	rand.seed = seed ^ 25214903917;
	rand.a = 25214903917;
	rand.m = 48;
	rand.c = 11;
	return rand;
}

/**
 * Gets next random number within [lo, up]
 */
int cpcou_lcg_next_with_bounds(struct cpcou_lcg *gen, int lo, int up)
{
	int x = cpcou_lcg_next(gen);
	int range = up - lo + 1;
	x = (x % range + range) % range;
	return x + lo;
}

/**
 * Gets the next random value of an LCG
 */
int cpcou_lcg_next(struct cpcou_lcg *gen)
{
	gen->seed = gen->seed * gen->a + gen->c;
	gen->seed &= (1ull << gen->m) - 1;
	return gen->seed >> (gen->m - 8 * sizeof(int));
}

/**
 * Get the next random 64-bit integer of an LCG
 */
long long cpcou_lcg_nextl(struct cpcou_lcg *gen)
{
	return((long long) cpcou_lcg_next(gen) << 32) | (unsigned) cpcou_lcg_next(gen);
}

/**
 * Gets the next random float of an LCG
 */
double cpcou_lcg_nextf(struct cpcou_lcg *gen)
{
	long long x = cpcou_lcg_nextl(gen);
	return(double) (x & 0xfffffffffffffll) / 0xfffffffffffffll;
}

/**
 * Generate random bytes and stores them in a buffer
 */
void cpcou_lcg_next_bytes(struct cpcou_lcg *gen, void *restrict buf, size_t sz)
{
	char *restrict cbuf = buf;
	for(size_t i = 0; i < sz; )
	{
		for(unsigned x = cpcou_lcg_next(gen), n = 4 > sz - i ? sz - i : 4; n > 0; x >>= 8, --n, ++i)
			cbuf[i] = x;
	}
}

/**
 * Stable sorts an array of elements
 */
void cpcou_stable_sort(void *buf, size_t cnt, size_t sz, int(*compar)(const void *, const void *))
{
	char unsigned *aux = malloc(cnt * sz);
	char unsigned *cbuf = (char unsigned *)buf;
	size_t lstk[100], rstk[100];
	char mstk[100];
	size_t ssz = 0;
	lstk[ssz] = 0;
	rstk[ssz] = cnt - 1;
	mstk[ssz] = 1;
	++ssz;
	size_t l, r;
	size_t mind, lind, rind;
	char mode;
	while(ssz)
	{
		--ssz;
		l = lstk[ssz];
		r = rstk[ssz];
		if(l == r)
			continue;
		mode = mstk[ssz];
		if(mode)
		{
			mstk[ssz] = 0;
			++ssz;
			lstk[ssz] = l + r + 1 >> 1;
			lstk[ssz + 1] = l;
			rstk[ssz] = r;
			rstk[ssz + 1] = l + r - 1 >> 1;
			mstk[ssz] = 1;
			mstk[ssz + 1] = 1;
			ssz += 2;
			continue;
		}
		mind = l;
		lind = l;
		rind = l + r + 1 >> 1;
		while(lind < l + r + 1 >> 1 && rind < r + 1)
		{
			if(compar(cbuf + lind * sz, cbuf + rind * sz) < 0)
			{
				memcpy(aux + mind * sz, cbuf + lind * sz, sz);
				++lind;
			}
			else
			{
				memcpy(aux + mind * sz, cbuf + rind * sz, sz);
				++rind;
			}
			++mind;
		}
		if(lind < l + r + 1 >> 1)
			memcpy(aux + mind * sz, cbuf + lind * sz, ((l + r + 1 >> 1) - lind) * sz);
		if(rind < r + 1)
			memcpy(aux + mind * sz, cbuf + rind * sz, (r - rind + 1) * sz);
		memcpy(cbuf + l * sz, aux + l * sz, (r - l + 1) * sz);
	}
	free(aux);
}
/**
 * Changes the endianness of a number
 */
size_t cpcou_convert_endianness(size_t bs)
{
	size_t new = 0;
	for(size_t i = 0; i < sizeof(size_t) / 2; ++i)
		new += (bs >> 8 * (sizeof(size_t) - i * 2 - 1) & 0xffull << 8 * i) | (bs << 8 * (sizeof(size_t) - i * 2 - 1) & 0xffull << 8 * (sizeof(size_t) - i - 1));
	return new;
}

/**
 * Get the endianness of your machine, 0 is big and 1 is little
 */
int cpcou_get_endianness(void)
{
	int x = 1;
	char *y = (char *)&x;
	return *y == 1;
}

#endif
#endif

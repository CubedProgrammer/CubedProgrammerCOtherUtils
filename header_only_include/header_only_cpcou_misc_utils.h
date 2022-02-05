#ifndef __cplusplus
#ifndef Included_header_only_cpcou_misc_utils_h
#define Included_header_only_cpcou_misc_utils_h
#include<math.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#ifdef _WIN32
//#include<shlwapi.h>
#include<ws2tcpip.h>
#include<windows.h>
#else
#include<arpa/inet.h>
#include<dirent.h>
#include<fcntl.h>
#include<netdb.h>
#include<sys/ioctl.h>
#include<sys/statvfs.h>
#include<termios.h>
#include<unistd.h>
#endif
#include<cpcou_misc_utils.h>

#ifndef _WIN32
extern char **environ;
#endif

const char cpcou____digits[37] = "0123456789abcdefghijklmnopqrstuvwxyz";

#ifdef _WIN32
#define cpcou____getch _getch
#else
#define cpcou____getch getchar
#endif

// Debug malloc hash table
struct cpcou____ma_hmp_en
{
	void *ptr;
	const char *vname;
	const char *fname;
	size_t ln;
};

struct cpcou____ma_hmp
{
	size_t capa, oc;
	size_t cnt;
	struct cpcou____ma_hmp_en *stuff;
};

struct cpcou____ma_hmp *cpcou____ma_hmp_dat = NULL;

/**
 * Gets the size of the terminal attached to the process, if there is one
 * Returns zero on success, and something else on failure
 */
int cpcou_get_terminal_size(int *restrict width, int *restrict height)
{
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO ci;
	HANDLE ho = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL succ = !GetConsoleScreenBufferInfo(ho, &ci);
	*width = ci.srWindow.Right - ci.srWindow.Left + 1;
	*height = ci.srWindow.Bottom - ci.srWindow.Top + 1;
#else
	struct winsize sz;
	int succ = ioctl(STDIN_FILENO, TIOCGWINSZ, &sz);
	*width = sz.ws_col;
	*height = sz.ws_row;
#endif
	return succ;
}

void cpcou____db_ins_ptr(void *ptr, const char *vname, const char *fname, size_t ln)
{
	intptr_t pn = (intptr_t)ptr;
	pn %= cpcou____ma_hmp_dat->capa;
	while(cpcou____ma_hmp_dat->stuff[pn].ptr != NULL)
	{
		++pn;
		if(pn == cpcou____ma_hmp_dat->capa)
			pn = 0;
	}
	cpcou____ma_hmp_dat->stuff[pn].ptr = ptr;
	cpcou____ma_hmp_dat->stuff[pn].ln = ln;
	cpcou____ma_hmp_dat->stuff[pn].fname = fname;
	cpcou____ma_hmp_dat->stuff[pn].vname = vname;
	++cpcou____ma_hmp_dat->cnt;
	if(cpcou____ma_hmp_dat->capa * 0.6 < cpcou____ma_hmp_dat->cnt)
	{
		struct cpcou____ma_hmp_en *ens = malloc((cpcou____ma_hmp_dat->oc + cpcou____ma_hmp_dat->capa) * sizeof(*ens));
		size_t nc = cpcou____ma_hmp_dat->capa + cpcou____ma_hmp_dat->oc;
		for(size_t i = 0; i < cpcou____ma_hmp_dat->capa; ++i)
		{
			if(cpcou____ma_hmp_dat->stuff[i].ptr != NULL)
			{
				pn = (intptr_t)ptr;
				pn %= nc;
				while(ens[pn].ptr != NULL)
				{
					++pn;
					if(pn == nc)
						pn = 0;
				}
				memcpy(ens + pn, cpcou____ma_hmp_dat + i, sizeof(*ens));
			}
		}
		free(cpcou____ma_hmp_dat->stuff);
		cpcou____ma_hmp_dat->stuff = ens;
		cpcou____ma_hmp_dat->oc = cpcou____ma_hmp_dat->capa;
		cpcou____ma_hmp_dat->capa = nc;
	}
}

int cpcou____db_del_ptr(void *ptr)
{
	intptr_t pn = (intptr_t)ptr;
	pn %= cpcou____ma_hmp_dat->capa;
	intptr_t ogpn = pn;
	int nonexistant = ptr == NULL;
	while(!nonexistant && cpcou____ma_hmp_dat->stuff[pn].ptr != ptr)
	{
		++pn;
		if(pn == cpcou____ma_hmp_dat->capa)
			pn = 0;
		else if(cpcou____ma_hmp_dat->stuff[pn].ptr == NULL || pn == ogpn)
			nonexistant = 1;
	}
	if(!nonexistant)
	{
		cpcou____ma_hmp_dat->stuff[pn].fname = NULL;
		cpcou____ma_hmp_dat->stuff[pn].vname = NULL;
		cpcou____ma_hmp_dat->stuff[pn].ptr = NULL;
		cpcou____ma_hmp_dat->stuff[pn].ln = 0;
		--cpcou____ma_hmp_dat->cnt;
	}
	return nonexistant;
}

/**
 * Debugging malloc and free functions.
 */
void *cpcou_debug_malloc_impl(size_t sz, const char *fname, size_t ln)
{
	void *ptr = malloc(sz);
	if(ptr != NULL)
	{
		if(cpcou____ma_hmp_dat == NULL)
		{
			cpcou____ma_hmp_dat = malloc(sizeof(*cpcou____ma_hmp_dat));
			if(cpcou____ma_hmp_dat == NULL)
				fputs("Could not allocate hashtable for debugging.\n", stderr);
			else
			{
				cpcou____ma_hmp_dat->stuff = malloc(89 * sizeof(struct cpcou____ma_hmp_en));
				cpcou____ma_hmp_dat->capa = 89;
				cpcou____ma_hmp_dat->oc = 55;
				cpcou____ma_hmp_dat->cnt = 0;
				memset(cpcou____ma_hmp_dat->stuff, 0, 89 * sizeof(struct cpcou____ma_hmp_en));
				atexit(cpcou_check_mem_impl);
			}
		}
		cpcou____db_ins_ptr(ptr, NULL, fname, ln);
	}
	return ptr;
}

void cpcou_debug_free_impl(void *ptr)
{
	if(cpcou____db_del_ptr(ptr))
		fputs("Attempt to free pointer that was not heap allocated or was already freed.\n", stderr);
	else
		free(ptr);
}

void *cpcou_debug_realloc_impl(void *ptr, size_t sz, const char *vname, const char *fname, size_t ln)
{
	void *new = NULL;
	if(cpcou____db_del_ptr(ptr))
		fputs("Attempt to reallocate pointer that was not heap allocated or has already been freed.\n", stderr);
	else
	{
		new = realloc(ptr, sz);
		if(new != NULL)
			cpcou____db_ins_ptr(new, vname, fname, ln);
	}
	return new;
}

void cpcou_check_mem_manual(void)
{
	struct cpcou____ma_hmp *map = cpcou____ma_hmp_dat;
	if(map != NULL)
	{
		if(map->cnt != 0)
		{
			fprintf(stderr, "There were %zu heap allocated pointers that were never freed.\n", map->cnt);
			size_t pn;
			for(size_t i = 0; i < map->capa; ++i)
			{
				if(map->stuff[i].ptr != NULL)
				{
					pn = (size_t)map->stuff[i].ptr;
					if(map->stuff[i].vname != NULL)
						fprintf(stderr, "Pointer %s with address 0x%zx allocated at %s on line %zu was never freed.\n", map->stuff[i].vname, pn, map->stuff[i].fname, map->stuff[i].ln);
					else
						fprintf(stderr, "Pointer with address 0x%zx allocated at %s on line %zu was never freed.\n", pn, map->stuff[i].fname, map->stuff[i].ln);
				}
			}
		}
	}
}

void cpcou_check_mem_impl(void)
{
	struct cpcou____ma_hmp *map = cpcou____ma_hmp_dat;
	if(map != NULL)
	{
		cpcou_check_mem_manual();
		free(map->stuff);
		free(map);
	}
}

/**
 * Convert a number to or from Roman numerals
 * If alo is 1, then an overline is allowed to represent multiplication by 1000, and 0 disallows this
 * If alo is 1, M is used when converting to, instead of overlined I
 * Parsing is case insensitive, and allows IIII
 */
int cpcou_to_roman_numeral(int n, char *buf, int alo)
{
	if(n > 3999 + 3996000 * alo && n <= 0)
		return-1;
	else
	{
		if(alo)
		{
			strcpy(buf, "\033\13353m");
			buf += 5;
			for(int i = n / 1000000; i > 0; --i, *buf = 'M', ++buf);
			n %= 1000000;
			if(n >= 900000)
			{
				buf[0] = 'C';
				buf[1] = 'M';
				buf += 2;
				n -= 900000;
			}
			else if(n >= 500000)
			{
				n -= 500000;
				*buf = 'D';
				++buf;
			}
			else if(n >= 400000)
			{
				buf[0] = 'C';
				buf[1] = 'D';
				buf += 2;
				n -= 400000;
			}
			for(int i = n / 100000; i > 0; --i, *buf = 'C', ++buf);
			n %= 100000;
			if(n >= 90000)
			{
				buf[0] = 'X';
				buf[1] = 'C';
				buf += 2;
				n -= 90000;
			}
			else if(n >= 50000)
			{
				n -= 50000;
				*buf = 'L';
				++buf;
			}
			else if(n >= 40000)
			{
				buf[0] = 'X';
				buf[1] = 'L';
				buf += 2;
				n -= 40000;
			}
			for(int i = n / 10000; i > 0; --i, *buf = 'X', ++buf);
			n %= 10000;
			if(n >= 9000)
			{
				strcpy(buf, "\033\1330mM\033\13353mX");
				buf += 11;
				n -= 9000;
			}
			else if(n >= 5000)
			{
				n -= 5000;
				*buf = 'V';
				++buf;
			}
			else if(n >= 4000)
			{
				strcpy(buf, "\033\1330mM\033\13353mV");
				buf += 11;
				n -= 4000;
			}
			strcpy(buf, "\033\1330m");
			buf += 4;
		}
		for(int i = n / 1000; i > 0; --i, *buf = 'M', ++buf);
		n %= 1000;
		if(n >= 900)
		{
			buf[0] = 'C';
			buf[1] = 'M';
			buf += 2;
			n -= 90;
		}
		else if(n >= 500)
		{
			n -= 500;
			*buf = 'D';
			++buf;
		}
		else if(n >= 400)
		{
			buf[0] = 'C';
			buf[1] = 'D';
			buf += 2;
			n -= 400;
		}
		for(int i = n / 100; i > 0; --i, *buf = 'C', ++buf);
		n %= 100;
		if(n >= 90)
		{
			buf[0] = 'X';
			buf[1] = 'C';
			buf += 2;
			n -= 90;
		}
		else if(n >= 50)
		{
			n -= 50;
			*buf = 'L';
			++buf;
		}
		else if(n >= 40)
		{
			buf[0] = 'X';
			buf[1] = 'L';
			buf += 2;
			n -= 40;
		}
		for(int i = n / 10; i > 0; --i, *buf = 'X', ++buf);
		n %= 10;
		if(n == 9)
		{
			buf[0] = 'I';
			buf[1] = 'X';
			buf += 2;
			n -= 9;
		}
		else if(n >= 5)
		{
			n -= 5;
			*buf = 'V';
			++buf;
		}
		else if(n == 4)
		{
			buf[0] = 'I';
			buf[1] = 'V';
			buf += 2;
			n -= 4;
		}
		for(int i = n; i > 0; --i, *buf = 'I', ++buf);
		*buf = '\0';
		return 0;
	}
}

int cpcou_from_roman_numeral(const char *buf, int alo)
{
	int n = 0, l = 99999999, curr;
	int sm = l, mult = 1;
	const char *end;
	for(const char *it = buf; n >= 0 && *it != '\0'; ++it)
	{
		if(*it == 27)
		{
			for(end = ++it; *end != 'm' && *end != '\0'; ++end);
			if(*end == 'm')
			{
				if(*it == 91)
				{
					++it;
					if(end - it == 2 && strncmp(it, "53", 2) == 0)
						mult = 999 * alo + 1;
					else if(end - it == 1 && *it == '0')
						mult = 1;
				}
			}
			else
				n = -1, --end;
			it = end;
		}
		else
		{
			switch(*it)
			{
				case'm':
				case'M':
					curr = 1000 * mult;
					break;
				case'd':
				case'D':
					curr = 500 * mult;
					break;
				case'c':
				case'C':
					curr = 100 * mult;
					break;
				case'l':
				case'L':
					curr = 50 * mult;
					break;
				case'x':
				case'X':
					curr = 10 * mult;
					break;
				case'v':
				case'V':
					curr = 5 * mult;
					break;
				case'i':
				case'I':
					curr = 1 * mult;
					break;
				default:
					n = -curr - 1;
			}
			n += curr;
			if(sm < curr)
			{
				if(l * 5 == curr || l * 10 == curr)
					n -= 2 * l;
				else
					n = -1;
			}
			else
				sm = curr;
			l = curr;
		}
	}
	return n;
}

/**
 * Gets a password from stdin
 * Maximum password length is sz, but array should have size sz + 1 for null character
 * if toggle is 1, then Ctrl+A shows or hides password, if toggle is 2, then Ctrl+B, 3, Ctrl+C, and so on
 * Use zero to disable this feature
 * If sh is zero, then the password is hidden by default, and one for shown
 */
void cpcou_get_password(char *restrict buf, size_t sz, int toggle, int sh)
{
#ifndef _WIN32
	struct termios old, new;
	tcgetattr(STDIN_FILENO, &old);
	memcpy(&new, &old, sizeof(struct termios));
	new.c_lflag &= ~ECHO;
	new.c_lflag &= ~ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
#endif
	char c = cpcou____getch();
	size_t pos = 0, cnt = 0;
#ifdef _WIN32
	while(c != '\r')
#else
	while(c != '\n')
#endif
	{
		if(toggle && c == toggle)
		{
			for(size_t i = 0; i < pos; ++i)
				fputc('\b', stdout);
			if(sh)
			{
				for(size_t i = 0; i < cnt; ++i)
					fputc('*', stdout);
			}
			else
				fwrite(buf, 1, cnt, stdout);
			for(size_t i = pos; i < cnt; ++i)
				fputc('\b', stdout);
			sh = !sh;
		}
#ifdef _WIN32
		else if(c == -040)
#else
		else if(c == 033)
#endif
		{
#ifndef _WIN32
			c = cpcou____getch();
			if(c == 0133)
#endif
			{
				c = cpcou____getch();
				switch(c)
				{
#ifdef _WIN32
					case 0113:
#else
					case 0104:
#endif
						if(pos > 0)
						{
							--pos;
							fputc('\b', stdout);
						}
						else
							fputc('\a', stdout);
						break;
#ifdef _WIN32
					case 0115:
#else
					case 0103:
#endif
						if(pos < cnt)
						{
							fputc(sh ? buf[pos] : '*', stdout);
							++pos;
						}
						else
							fputc('\a', stdout);
						break;
#ifdef _WIN32
					case 0107:
#else
					case 0110:
#endif
						for(size_t i = 0; i < pos; ++i)
							fputc('\b', stdout);
						pos = 0;
						break;
#ifdef _WIN32
					case 0117:
#else
					case 0106:
#endif
						if(sh)
							fwrite(buf + pos, 1, cnt - pos, stdout);
						else
						{
							for(size_t i = pos; i < cnt; ++i)
								fputc('*', stdout);
						}
						pos = cnt;
						break;
#ifdef _WIN32
					case 0123:
#else
					case 063:
#endif
						if(pos != cnt)
						{
							if(pos + 1 != cnt)
								memmove(buf + pos, buf + pos + 1, cnt - pos - 1);
							--cnt;
							if(sh)
								fwrite(buf + pos, 1, cnt - pos, stdout);
							else
							{
								for(size_t i = pos; i < cnt; ++i)
									fputc('*', stdout);
							}
							fputc(' ', stdout);
							for(size_t i = pos; i <= cnt; ++i)
								fputc('\b', stdout);
						}
						else
							fputc('\a', stdout);
#ifndef _WIN32
						cpcou____getch();
#endif
						break;
				}
			}
		}
		else
#ifdef _WIN32
			if(c == '\b')
#else
			if(c == 0177)
#endif
			{
				if(pos != 0)
				{
					if(pos != cnt)
						memmove(buf + pos - 1, buf + pos, cnt - pos);
					--cnt;
					--pos;
					fputc('\b', stdout);
					if(sh)
						fwrite(buf + pos, 1, cnt - pos, stdout);
					else
					{
						for(size_t i = pos; i < cnt; ++i)
							fputc('*', stdout);
					}
					fputc(' ', stdout);
					for(size_t i = pos; i <= cnt; ++i)
						fputc('\b', stdout);
				}
				else
					fputc('\a', stdout);
			}
			else if(c < ' ')
				fputc('\a', stdout);
			else if(cnt < sz)
			{
				if(pos != cnt)
					memmove(buf + pos + 1, buf + pos, cnt - pos);
				buf[pos] = c;
				++cnt;
				if(sh)
					fwrite(buf + pos, 1, cnt - pos, stdout);
				else
				{
					for(size_t i = pos; i < cnt; ++i)
						fputc('*', stdout);
				}
				++pos;
				for(size_t i = pos; i < cnt; ++i)
					fputc('\b', stdout);
			}
			else
				fputc('\a', stdout);
		fflush(stdout);
		c = cpcou____getch();
	}
	buf[cnt] = '\0';
	if(sh)
		fwrite(buf + pos, 1, cnt - pos, stdout);
	else
	{
		for(size_t i = pos; i < cnt; ++i)
			fputc('*', stdout);
	}
#ifndef _WIN32
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
#endif
	cpcou_stdout_erase(cnt);
	fflush(stdout);
}

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
		if(en->d_type == DT_BLK && len >= mini && len <= maxi && strncmp(en->d_name, "loop", 4) != 0)
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
	char **strs = cpcou____dev_sd_filter(&cnt, 3, 10);
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
	char **strs = cpcou____dev_sd_filter(&cnt, 4, 10);
#endif
	strs[cnt] = NULL;
	return strs;
}

/**
 * Converts double to a string
 */
size_t cpcou_ftoa(double num, char *restrict str)
{
	size_t ind = 0;
	if(num < 0)
	{
		str[0] = '-';
		++ind;
		num = -num;
	}
	if(isnan(num))
	{
		strcpy(str + ind, "nan");
		ind += 3;
	}
	else if(isinf(num))
	{
		strcpy(str + ind, "inf");
		ind += 3;
	}
	else
	{
		int exp=0;
		if(num < 0.000000000001)
		{
			while(num < 1)
				num *= 10, --exp;
		}
		else if(num >= 1000000000000)
		{
			while(num >= 10)
				num *= 0.1, ++exp;
		}
		long long ipart = floor(num);
		double r = num - floor(num);
		ind += cpcou_lltoa(ipart, str + ind, 10);
		str[ind++] = '.';
		r *= 10;
		while(r != floor(r))
		{
			if(r<0)
				str[ind++] = '0';
			r*=10;
		}
		ind += cpcou_lltoa(r, str + ind, 10);
		if(exp != 0)
		{
			str[ind++] = 'e';
			ind += cpcou_lltoa(exp, str + ind, 10);
		}
	}
	return ind;
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

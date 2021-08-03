#ifndef __cplusplus
#ifndef Included_header_only_cpcou_misc_utils_h
#define Included_header_only_cpcou_misc_utils_h
#include<string.h>
#ifdef _WIN32
#include<windows.h>
#else
#include<unistd.h>
#endif
#include<cpcou_misc_utils.h>

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

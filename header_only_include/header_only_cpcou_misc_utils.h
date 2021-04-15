#ifndef __cplusplus
#ifndef Included_header_only_cpcou_misc_utils_h
#define Included_header_only_cpcou_misc_utils_h
#include<string.h>
#include<cpcou_misc_utils.h>

/**
 * Gets the next random value of an LCG
 */
int cpcou_lcg_next(struct cpcou_lcg *gen)
{
	gen->seed = gen->seed * gen->a + gen->c;
	gen->seed &= (1 << gen->m) - 1;
	return gen->seed >> (gen->m - 8 * sizeof(int));
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

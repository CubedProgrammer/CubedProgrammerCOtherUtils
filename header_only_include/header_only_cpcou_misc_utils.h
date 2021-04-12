#ifndef __cplusplus
#ifndef Included_header_only_cpcou_misc_utils_h
#define Included_header_only_cpcou_misc_utils_h
#include<string.h>
#include<cpcou_misc_utils.h>

/**
 * Stable sorts an array of elements
 */
void cpcou_stable_sort(void *buf, size_t cnt, size_t sz, int(*compar)(const void *, const void *))
{
	char unsigned *aux = malloc(cnt * sz);
	char unsigned *cbuf = (char unsigned *)buf;
	size_t lstk[100], rstk[100];
	size_t ssz = 0;
	lstk[ssz] = 0;
	rstk[ssz] = sz - 1;
	++ssz;
	size_t l, r;
	size_t mind, lind, rind;
	while(ssz)
	{
		--ssz;
		l = lstk[ssz];
		r = lstk[ssz];
		if(l == r)
			continue;
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
				++lind;
			}
			++mind;
		}
		if(lind < l + r + 1 >> 1)
			memcpy(aux + mind * sz, cbuf + lind * sz, ((l + r + 1 >> 1) - lind) * sz);
		if(rind < r + 1)
			memcpy(aux + mind * sz, cbuf + rind * sz, (r - rind + 1) * sz);
		lstk[ssz] = l + r + 1 >> 1;
		lstk[ssz + 1] = l;
		rstk[ssz] = r;
		rstk[ssz + 1] = l + r - 1 >> 1;
		ssz += 2;
		memcpy(cbuf + l, aux + l, r - l + 1);
	}
	free(aux);
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

#ifndef __cplusplus
#ifndef Included_header_only_cpcou_str_algo_h
#define Included_header_only_cpcou_str_algo_h
#include<stdlib.h>
#include<string.h>

/**
 * Case insensitive comparison
 */
int cpcou_insens_strcmp(const char *x, const char *y)
{
	size_t xlen = strlen(x), ylen = strlen(y);
	size_t len = xlen < ylen ? xlen : ylen;
	char u, v;
	int res = 0;
	for(size_t i = 0; i < len; ++i)
	{
		u = x[i], v = y[i];
		if(u >= 'A' && u <= 'Z')
			u+=0x20;
		if(v >= 'A' && v <= 'Z')
			v+=0x20;
		if(u == v)
			u = x[i], v = y[i];
		if(u < v)
			res = -1, len = 0;
		else if(u > v)
			res = 1, len = 0;
	}
	return res;
}

/**
 * Case insensitive natural string comparison
 */
int cpcou_insense_strnatcmp(const char *x, const char *y);

/**
 * Copies a string into a new heap allocated pointer
 */
char *cpcou_cpy_cstr(const char *x)
{
	char *y = malloc(strlen(x) + 1);
	strcpy(y, x);
	return y;
}

#endif
#endif

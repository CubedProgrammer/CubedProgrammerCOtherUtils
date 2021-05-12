#ifndef __cplusplus
#ifndef Included_header_only_cpcou_str_algo_h
#define Included_header_only_cpcou_str_algo_h
#include<stdlib.h>
#include<string.h>
#include<cpcou_misc_utils.h>
#include<cpcou_str_algo.h>

// for cpcou_sort_strs
static int(*cpcou____sort_strs_compar)(const char *, const char *);

// also for cpcou_sort_strs
static inline int cpcou____strs_void_compar(const void *m, const void *n)
{
	const char *u = *(const char *const *)m, *v = *(const char *const *)n;
	return cpcou____sort_strs_compar(u, v);
}

/**
 * Check if two strings are permutations of each other
 */
int cpcou_is_permutation(const char *x, const char *y)
{
	size_t cnts[256];
	memset(cnts, 0, sizeof(cnts));
	for(size_t i = 0; x[i] != '\0'; ++i)
	{
		++cnts[x[i]];
		--cnts[y[i]];
	}
	int res = 1;
	for(int i = 0; i < 256; ++i)
	{
		if(cnts[i] != 0)
			res = 0;
	}
	return res;
}

/**
 * Sets all letters to lowercase
 */
void cpcou_to_lower(char *str)
{
	for(char *it = str; *it != '\0'; ++it)
	{
		if(*it > 'A' && *it < 'Z')
			*it += 0x20;
	}
}

/**
 * Sets all letters to uppercase
 */
void cpcou_to_upper(char *str)
{
	for(char *it = str; *it != '\0'; ++it)
	{
		if(*it > 'a' && *it < 'z')
			*it -= 0x20;
	}
}

/**
 * Sorts an array of strings
 */
void cpcou_sort_strs(const char **strs, int(*compar)(const char *, const char *))
{
	size_t len = 0;
	while(strs[len])
		++len;
	cpcou____sort_strs_compar = compar;
	qsort(strs, len, sizeof(const char *), cpcou____strs_void_compar);
}

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
 * Sorts an array of strings, this is a stable algorithm
 */
void cpcou_ssort_strs(const char **strs, int(*compar)(const char *, const char *))
{
	size_t len = 0;
	while(strs[len])
		++len;
	cpcou____sort_strs_compar = compar;
	cpcou_stable_sort(strs, len, sizeof(const char *), cpcou____strs_void_compar);
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

/**
 * Get the type of character, digit, lowercase, uppercase, or whitespace
 */
enum cpcou_char_type cpcou_typeof_char(char c)
{
	if(c >= '0' && c <= '9')
		return CPCOU_DIGIT;
	else if(strchr("\n\t ", c) != NULL)
		return CPCOU_WHITESPACE;
	else if(c >= 'a' && c <= 'z')
		return CPCOU_LOWERCASE_ENGLISH;
	else if(c >= 'A' && c <= 'Z')
		return CPCOU_UPPERCASE_ENGLISH;
	else
		return CPCOU_OTHER_CHAR_TYPE;
}

#endif
#endif

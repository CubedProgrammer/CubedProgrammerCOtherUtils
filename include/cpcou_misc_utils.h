#ifndef __cplusplus
#ifndef Included_cpcou_misc_utils_h
#define Included_cpcou_misc_utils_h
#include<stdlib.h>
#define cpcou_memset_macro(typename, arr, val, cnt)do{typename cpcou____tmp = val;cpcou_fill_mem(arr, &cpcou____tmp, sizeof(typename), cnt);}while(0)

typedef struct cpcou_lcg
{
	long long unsigned seed;
	long long unsigned a, m, c;
}cpcou_lcg;

/**
 * Checks if two char arrays are permutations
 */
int cpcou_arr_perms(const char *x, const char *y);

/**
 * Filling a block of memory with a value
 */
void cpcou_fill_mem(void *ptr, const void *val, size_t size, size_t cnt);

/**
 * Allocates memory for a two-dimensional array
 */
void *cpcou_malloc_2d(size_t width, size_t height);

/**
 * Initialize a java.util.Random with a seed
 */
struct cpcou_lcg cpcou_init_jur(long seed);

/**
 * Gets next random number within [lo, up]
 */
int cpcou_lcg_next_with_bounds(struct cpcou_lcg *gen, int lo, int up);

/**
 * Gets the next random value of an LCG
 */
int cpcou_lcg_next(struct cpcou_lcg *gen);

/**
 * Get the next random 64-bit integer of an LCG
 */
int64_t cpcou_lcg_nextl(struct cpcou_lcg *gen);

/**
 * Gets the next random float of an LCG
 */
double cpcou_lcg_nextf(struct cpcou_lcg *gen);

/**
 * Stable sorts an array of elements
 */
void cpcou_stable_sort(void *buf, size_t cnt, size_t sz, int(*compar)(const void *, const void *));

/**
 * Changes the endianness of a number
 */
size_t cpcou_convert_endianness(size_t bs);

/**
 * Get the endianness of your machine, 0 is big and 1 is little
 */
int cpcou_get_endianness(void);

#endif
#endif

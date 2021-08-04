#ifndef __cplusplus
#ifndef Included_cpcou_misc_utils_h
#define Included_cpcou_misc_utils_h
#include<stdlib.h>
#define cpcou_memset_macro(typename, arr, val, cnt)do{typename cpcou____tmp = val;cpcou_fill_mem(arr, &cpcou____tmp, sizeof(typename), cnt);}while(0)
#define cpcou_reverse_arr(arr, size)for(size_t i = 0; i < ((size) / 2);i++)cpcou_swap_mem(arr + i, arr + size - i - 1, sizeof((arr)[0]))
#define cpcou_swap_var(x, y)cpcou_swap_mem(&(x), &(y), sizeof(x))
#define cpcou_next_perm(arr, size, compar)do{size_t ind = 0, next;for(size_t i = size - 1; i > 0; i--){if(compar((arr)[i - 1], (arr)[i])){ind = i; i = 1;}}cpcou_reverse_arr(arr + ind, size - ind);if(ind > 0){--ind;for(size_t i = ind; i < size; i++){if(compar((arr)[ind], (arr)[i])){next = i;i = size;}}cpcou_swap_mem(arr + ind, arr + next, sizeof((arr)[0]));}}while(0)
#define cpcou_compar_macro(x, y)((x)<(y))

typedef struct cpcou_lcg
{
	long long unsigned seed;
	long long unsigned a, m, c;
}cpcou_lcg;

/**
 * Remove characters from the last line of console output
 */
void cpcou_stdout_erase(size_t chars);

/**
 * Get the name of the running program
 */
size_t cpcou_get_exe(char *buf, size_t sz);

/**
 * Swaps the data in two blocks of memory
 */
void cpcou_swap_mem(void *x, void *y, size_t sz);

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
long long cpcou_lcg_nextl(struct cpcou_lcg *gen);

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

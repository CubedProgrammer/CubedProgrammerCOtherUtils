#ifndef __cplusplus
#ifndef Included_cpcou_misc_utils_h
#define Included_cpcou_misc_utils_h
#include<stdlib.h>
#include<string.h>
#define cpcou_memset_macro(typename, __arr__, val, cnt)do{typename cpcou____tmp = val;cpcou_fill_mem(__arr__, &cpcou____tmp, sizeof(typename), cnt);}while(0)
#define cpcou_reverse_arr(__arr__, size)for(size_t i = 0; i < ((size) / 2);i++)cpcou_swap_mem(__arr__ + i, __arr__ + size - i - 1, sizeof((__arr__)[0]))
#define cpcou_swap_var(__x__, __y__)cpcou_swap_mem(&(__x__), &(__y__), sizeof(__x__))
#define cpcou_next_perm(__arr__, size, compar)do{size_t ind = 0, next;for(size_t i = size - 1; i > 0; i--){if(compar((__arr__)[i - 1], (__arr__)[i])){ind = i; i = 1;}}cpcou_reverse_arr(__arr__ + ind, size - ind);if(ind > 0){--ind;for(size_t i = ind; i < size; i++){if(compar((__arr__)[ind], (__arr__)[i])){next = i;i = size;}}cpcou_swap_mem(__arr__ + ind, __arr__ + next, sizeof((__arr__)[0]));}}while(0)
#define cpcou_compar_macro(__x__, __y__)((__x__)<(__y__))
#define cpcou_become_str(__t__)#__t__
#define cpcou_raw_puts(__t__)puts(#__t__)
#define cpcou_raw_printf(__t__, ...)printf(#__t__, __VA_ARGS__)
#define cpcou_raw_print(__t__)printf(#__t__)
#define cpcou_raw_scanf(__t__, ...)scanf(#__t__, __VA_ARGS__)
#define cpcou_raw_fputs(__t__, __stream__)fputs(#__t__, __stream__)
#define cpcou_raw_fprint(__stream__, __t__)fprintf(__stream__, #__t__)
#define cpcou_raw_fprintf(__stream__, __t__, ...)fprintf(__stream__, #__t__, __VA_ARGS__)
#define cpcou_raw_fscanf(__stream__, __t__, ...)fscanf(__stream__, #__t__, __VA_ARGS__)
#define cpcou_zero_var(__v__)memset(&__v__, 0, sizeof(__v__))
#define cpcou_one_var(__v__)memset(&__v__, -1, sizeof(__v__))

typedef struct cpcou_lcg
{
	long long unsigned seed;
	long long unsigned a, m, c;
}cpcou_lcg;

#ifdef _WIN32
typedef void *cpcou_pipe_t;
#elif defined(__linux__) || defined(__APPLE__)
typedef int cpcou_pipe_t;
#else
#include<threads.h>
typedef struct cpcou_pipe
{
	void *dat;
	size_t sz;
	size_t cnt, ind;
}*cpcou_pipe_t;
#endif

/**
 * Get names of environment variables, returned pointer is heap allocated
 * Do not attempt to free the pointers to individual strings
 */
char **cpcou_getenv_names(void);

/**
 * Gets the ip address by hostname, cbuf should be able to store at least sixteen characters
 * Returns zero on success
 */
int cpcou_host_to_ip(const char *hostname, char *cbuf);

/**
 * Creates a pipe, returns zero on success
 */
int cpcou_pipe(cpcou_pipe_t *r, cpcou_pipe_t *w);

/**
 * Read from a pipe, returns number of bytes read
 */
size_t cpcou_pipe_read(cpcou_pipe_t pipe, void *buf, size_t sz);

/**
 * Writes to a pipe, returns number of bytes wrote
 */
size_t cpcou_pipe_write(cpcou_pipe_t pipe, const void *buf, size_t sz);

/**
 * Closes pipes, return zero on success
 */
int cpcou_close_pipe(cpcou_pipe_t pipe);

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
 * Generate random bytes and stores them in a buffer
 */
void cpcou_lcg_next_bytes(struct cpcou_lcg *gen, void *restrict buf, size_t sz);

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

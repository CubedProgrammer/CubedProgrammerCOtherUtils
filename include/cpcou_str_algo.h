#ifndef __cplusplus
#ifndef Included_cpcou_str_algo_h
#define Included_cpcou_str_algo_h

/**
 * Sorts an array of strings
 */
void cpcou_sort_strs(const char **strs, int(*compar)(const char *, const char *));

/**
 * Case insensitive comparison
 */
int cpcou_insens_strcmp(const char *x, const char *y);

/**
 * Case insensitive natural string comparison
 */
int cpcou_insense_strnatcmp(const char *x, const char *y);

/**
 * Copies a string into a new heap allocated pointer
 */
char *cpcou_cpy_cstr(const char *x);

#endif
#endif

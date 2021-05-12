#ifndef __cplusplus
#ifndef Included_cpcou_str_algo_h
#define Included_cpcou_str_algo_h
#define cpcou_strarr_size(__sz__, __strs__)do{__sz__=0;while(__strs__[__sz__]!=NULL)__sz__++;}while(0);

enum cpcou_char_type
{ CPCOU_WHITESPACE = 997, CPCOU_LOWERCASE_ENGLISH, CPCOU_UPPERCASE_ENGLISH, CPCOU_DIGIT, CPCOU_OTHER_CHAR_TYPE };

/**
 * Check if two strings are permutations of each other
 */
int cpcou_is_permutation(const char *x, const char *y);

/**
 * Sets all letters to lowercase
 */
void cpcou_to_lower(char *str);

/**
 * Sets all letters to uppercase
 */
void cpcou_to_upper(char *str);

/**
 * Sorts an array of strings
 */
void cpcou_sort_strs(const char **strs, int(*compar)(const char *, const char *));

/**
 * Sorts an array of strings, this is a stable algorithm
 */
void cpcou_ssort_strs(const char **strs, int(*compar)(const char *, const char *));

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

/**
 * Get the type of character, digit, lowercase, uppercase, or whitespace
 */
enum cpcou_char_type cpcou_typeof_char(char c);

#endif
#endif

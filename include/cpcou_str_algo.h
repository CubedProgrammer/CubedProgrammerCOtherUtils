#ifndef __cplusplus
#ifndef Included_cpcou_str_algo_h
#define Included_cpcou_str_algo_h

enum cpcou_char_type
{ CPCOU_WHITESPACE = 997, CPCOU_LOWERCASE_ENGLISH, CPCOU_UPPERCASE_ENGLISH, CPCOU_DIGIT, CPCOU_OTHER_CHAR_TYPE };

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

/**
 * Get the type of character, digit, lowercase, uppercase, or whitespace
 */
enum cpcou_char_type cpcou_typeof_char(char c);

#endif
#endif

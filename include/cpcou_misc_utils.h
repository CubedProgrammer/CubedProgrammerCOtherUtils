#ifndef __cplusplus
#ifndef Included_cpcou_misc_utils_h
#define Included_cpcou_misc_utils_h
#include<stdlib.h>

/**
 * Stable sorts an array of elements
 */
void cpcou_stable_sort(void *buf, size_t cnt, size_t sz, int(*compar)(const void *, const void *));

#endif
#endif

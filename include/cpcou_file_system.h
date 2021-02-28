#ifndef __cplusplus
#ifndef Included_cpcou_file_system_h
#define Included_cpcou_file_system_h
#define CPCOU_FILE 997
#define CPCOU_DIRECTORY 998
#include<time.h>

/**
 * Gets what is inside a folder, returned pointer will be heap allocated
 * So are pointers pointed to by the returned pointer
 * The last string in the array of strings will be NULL;
 * DO NOT free the individual char pointers, ONLY free the char **
 */
char **cpcou_folder_insides(const char *name);

/**
 * Moves a file to a new location, overwriting anything that was previously there.
 */
int cpcou_move_file(const char *old, const char *new);

/**
 * Deletes a file or a directory
 */
int cpcou_delete_file(const char *file);

/**
 * Gets the last access time, in milliseconds since epoch
 */
time_t cpcou_last_access(const char *name);

/**
 * Gets the last modify time
 */
time_t cpcou_last_modify(const char *name);

/**
 * Gets the last status change time.
 */
time_t cpcou_last_stchange(const char *name);

/**
 * Gets the creation time
 */
time_t cpcou_create_time(const char *name);

/**
 * Gets the parent folder of a file or folder
 */
char *cpcou_file_parent(const char *name);

/**
 * Gets the size of a file, in bytes
 */
long long unsigned cpcou_file_size(const char *name);

/**
 * Get if the path points to a file or directory
 */
int cpcou_file_type(const char *name);

/**
 * Get the absolute file name of a file, returned string is heap allocated
 */
char *cpcou_absolute_path(const char *name);

#endif
#endif

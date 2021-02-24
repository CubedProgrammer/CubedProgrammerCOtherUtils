#ifndef __cplusplus
#ifndef Included_cpcou_file_system_h
#define Included_cpcou_file_system_h
#define CPCOU_FILE 997
#define CPCOU_DIRECTORY 998

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

#ifndef __cplusplus
#ifndef Included_cpcou_file_system_h
#define Included_cpcou_file_system_h
#define CPCOU_FILE 997
#define CPCOU_DIRECTORY 998
#include<time.h>
#ifdef _WIN32
#define MAX_PATH_LEN 260
#else
#include<dirent.h>
#include<limits.h>
#include<sys/types.h>
#define MAX_PATH_LEN PATH_MAX
#endif

typedef struct cpcou_file_info
{
	size_t plen;
	char abspth[MAX_PATH_LEN + 1];
	time_t last_access_time;
	time_t last_modify_time;
	time_t last_stchange_time;
	time_t crtime;
	int file_or_dir;
	size_t size;
	size_t bsz; // block size
	size_t bcnt; // number of blocks
	// heap allocated
	char **insides;
}*cpcou_file_infoptr;

/**
 * Root directory, it has no parent
 */
extern const char cpcou_root_dir[];

struct cpcou_dir_iter
{
#ifndef _WIN32
	DIR *fh;
	struct dirent *en;
#endif
};

/**
 * Initializes an iterator to specified directory
 * This iterator is immediately ready for use
 * Returns zero on success
 */
int cpcou_dir_iter_begin(struct cpcou_dir_iter *iter, const char *dir);

/**
 * Moves iterator to the next entry
 * Returns zero on success
 */
int cpcou_dir_iter_next(struct cpcou_dir_iter *iter);

/**
 * Gets the name of the current entry of the iterator
 * Stores the name in buf as a null-terminated string
 * Returns the number of bytes written to buf, or -1 on failure.
 */
int cpcou_dir_iter_get(const struct cpcou_dir_iter *iter, char *buf);

/**
 * Returns one if the iterator does not point to an entry, zero otherwise
 * After calling the cpcou_dir_iter_next function on an iterator with the final entry, this will return one
 */
int cpcou_dir_iter_ended(const struct cpcou_dir_iter *iter);

/**
 * Gets what is inside a folder, returned pointer will be heap allocated
 * So are pointers pointed to by the returned pointer
 * The last string in the array of strings will be NULL;
 * DO NOT free the individual char pointers, ONLY free the char **
 */
char **cpcou_folder_insides(const char *name);

/**
 * Moves a file to a new location, replacing the file that was previously there.
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
 * Sets the last modify time of a file, returns zero on success
 */
int cpcou_set_modify_time(const char *name, time_t time);

/**
 * Sets the last access time of a file, returns zero on success
 */
int cpcou_set_access_time(const char *name, time_t time);

/**
 * Sets the access and modify time of a file, returns zero on success
 */
int cpcou_set_access_and_modify(const char *name, time_t acc, time_t mod);

/**
 * Get the size of all files in a folder
 */
size_t cpcou_folder_size(const char *name);

/**
 * Get the number of files and folders in a folder
 */
size_t cpcou_file_count(const char *name);

/**
 * Copies a file into another location, whatever was there before is erased for good.
 */
int cpcou_copy_file(const char *from, const char *to);

/**
 * Creates a folder, parent folder must exist
 */
void cpcou_create_folder(const char *name);

/**
 * Creates a folder, parent folder need not exist
 */
void cpcou_create_folders(const char *name);

/**
 * Makes a file into a hidden file
 */
void cpcou_hide_file(const char *name);

/**
 * Unhides a hidden file
 */
void cpcou_unhide_file(const char *name);

/**
 * Gets the file extension of a cpcou file info struct
 */
const char *cpcou_get_extension(const struct cpcou_file_info *dat);

/**
 * Get the file information of files in a folder
 */
struct cpcou_file_info *cpcou_folder_inside_file_info(const char *name, size_t *szp);

/**
 * Gets information about a file.
 */
void cpcou_file_info(const char *name, struct cpcou_file_info *cfi);

/**
 * Finds a file in a directory, case sensitive for linux and insensitive for windows
 */
void cpcou_find_file(const char *name, const char *dir, char *path);

/**
 * Get the user's home directory
 */
void cpcou_get_home(char *cbuf);

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

/**
 * Helper function for sorting folder insides by alphabetical order
 */
int cpcou____ls_strcmp_helper(const void *x, const void *y);

#endif
#endif

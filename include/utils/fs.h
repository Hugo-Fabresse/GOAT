/*
 * File: fs.h
 * Description: Filesystem utility interface for GOAT.
 *              Provides abstractions for checking the existence of files and directories,
 *              and for creating them with specific permissions.
 *              Centralizes low-level filesystem interactions to simplify error handling,
 *              unit testing, and potential portability improvements.
 *              Functions return 0 on success, -1 on error.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef FS_H
#define FS_H

#include <stdbool.h>

bool fs_dir_exists(const char *path);
bool fs_file_exists(const char *path);
int fs_create_dir(const char *path, int mode);
int fs_create_file(const char *path, int mode);
int mkdir_recursive(const char *path);

#endif // FS_H

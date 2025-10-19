/*
 * File: fs.c
 * Description: Implementation of filesystem utility functions for GOAT.
 *              Provides abstractions for checking the existence of files and directories,
 *              and for creating them with specific permissions.
 *              Centralizes all low-level filesystem interactions to simplify error handling,
 *              testing, and future portability.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "utils/fs.h"
#include "utils/repo.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

bool fs_dir_exists(const char *path)
{
    struct stat st;

    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool fs_file_exists(const char *path)
{
    struct stat st;

    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

int fs_create_dir(const char *path, int mode)
{
    if (mkdir(path, mode) == -1) {
        if (errno == EEXIST) {
            return 0;
        }
        perror("mkdir");
        return -1;
    }
    return 0;
}

int fs_create_file(const char *path, int mode)
{
    FILE *f = fopen(path, "w");

    if (!f) {
        perror("fopen");
        return -1;
    }
    fclose(f);
    if (chmod(path, mode) == -1) {
        perror("chmod");
        return -1;
    }
    return 0;
}

static void sanitize_path(char *dest, const char *path)
{
    size_t len;

    snprintf(dest, PATH_MAX, "%s", path);
    len = strlen(dest);
    if (len > 0 && dest[len - 1] == '/')
        dest[len - 1] = '\0';
}

static int create_subdirs(char *path)
{
    char *p;

    for (p = path + 1; *p; p++) {
        if (*p != '/')
            continue;
        *p = '\0';
        if (mkdir(path, S_IRWXU) != 0 && errno != EEXIST) {
            perror("mkdir_recursive");
            return -1;
        }
        *p = '/';
    }
    return 0;
}

int mkdir_recursive(const char *path)
{
    char tmp[PATH_MAX];

    sanitize_path(tmp, path);
    if (create_subdirs(tmp) != 0)
        return -1;
    if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
        perror("mkdir_recursive");
        return -1;
    }
    return 0;
}

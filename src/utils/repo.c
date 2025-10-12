/*
 * File: repo.c
 * Description: GOAT repository utility implementation.
 *              Provides high-level functions for repository state and management.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "utils/repo.h"
#include "utils/fs.h"
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

bool check_already_initialized(void)
{
    return fs_dir_exists(GOAT_DIR);
}

bool find_goat_repo(char *repo_path, size_t size)
{
    char cwd[PATH_MAX];
    struct stat st;
    char *slash;

    if (!getcwd(cwd, sizeof(cwd))) {
        return false;
    }
    while (1) {
        snprintf(repo_path, size, "%s/.goat", cwd);
        if (stat(repo_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            return true;
        }
        slash = strrchr(cwd, '/');
        if (!slash || cwd == slash)
            break;
        *slash = '\0';
    }
    return false;
}

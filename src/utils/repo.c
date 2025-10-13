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

static char *build_abs_path(const char *base, const char *sub)
{
    static char abs_path[PATH_MAX];
    int written = snprintf(abs_path, sizeof(abs_path), "%s/%s", base, sub);

    if (written < 0 || (size_t)written >= sizeof(abs_path)) {
        return NULL;
    }
    return abs_path;
}

char *find_abs_path(const char *rel_path)
{
    char repo_root[PATH_MAX];

    if (!find_goat_repo(repo_root, sizeof(repo_root))) {
        return NULL;
    }
    if (rel_path[0] == '/') {
        return build_abs_path("", rel_path + 1);
    }
    if (strncmp(rel_path, ".goat/", 6) == 0) {
        return build_abs_path(repo_root, rel_path + 6);
        }
    return build_abs_path(repo_root, rel_path);
}

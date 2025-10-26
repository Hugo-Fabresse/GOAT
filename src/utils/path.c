/*
 * File: path.c
 * Description: Path resolution utilities for GOAT.
 *              Provides functions to construct and normalize absolute paths
 *              relative to the repository root, ensuring consistent path handling
 *              regardless of the current working directory.
 * Date: 11/10/2025
 * Author: Aliago
 */

#include "utils/repo.h"
#include <stdio.h>
#include <string.h>

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
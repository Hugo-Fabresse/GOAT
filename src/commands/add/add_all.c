/*
 * File: add_all.c
 * Description: Implementation of recursive file listing for the 'add --all' command in GOAT.
 *              Provides functionality to recursively traverse directories and process all
 *              regular files for staging, excluding the .goat directory and special entries.
 *              Used to stage all modified files in the repository working directory.
 * Date: 23/10/2025
 * Author: Aliago
 */

#include "commands/add/add.h"
#include "utils/repo.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>

void list_all_files(const char *dir, const char *repo_root, index_content_t **content_list, const index_content_t *current_index)
{
    DIR *d = opendir(dir);
    struct dirent *entry;
    char path[PATH_MAX];
    struct stat st;

    if (!d)
        return;
    while ((entry = readdir(d))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".goat") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        if (stat(path, &st) == -1)
            continue;
        if (S_ISDIR(st.st_mode))
            list_all_files(path, repo_root, content_list, current_index);
        else if (S_ISREG(st.st_mode))
            process_file(path, repo_root, content_list, current_index);
    }
    closedir(d);
}

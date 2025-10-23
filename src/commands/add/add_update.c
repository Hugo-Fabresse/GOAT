/*
 * File: add_update.c
 * Description: ${DESCRIPTION}
 * Date: 23/10/2025
 * Author: Aliago
 */

#include "commands/add/add.h"
#include "utils/repo.h"
#include <unistd.h>
#include <stdio.h>

int process_tracked_file(const char *repo_path, index_content_t *current_entry, index_content_t **content, const index_content_t *current_index)
{
    char full_path[PATH_MAX];
    int written;

    written = snprintf(full_path, sizeof(full_path), "%s/%s", repo_path, current_entry->rel_path);
    if (written < 0 || (size_t)written >= sizeof(full_path)) {
        fprintf(stderr, "Skipped (path too long): %s/%s\n", repo_path, current_entry->rel_path);
        return -1;
    }
    if (access(full_path, F_OK) == 0) {
        process_file(full_path, repo_path, content, current_index);
    } else {
        printf("Skipped (not found): %s\n", current_entry->rel_path);
    }
    return 0;
}

void process_all_tracked_files(const char *repo_path, index_content_t *current_index, index_content_t **content)
{
    index_content_t *current_entry = current_index;

    while (current_entry) {
        process_tracked_file(repo_path, current_entry, content, current_index);
        current_entry = current_entry->next;
    }
}

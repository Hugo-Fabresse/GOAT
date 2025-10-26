/*
 * File: add_update.c
 * Description: Implementation of tracked file processing for the 'add --update' command in GOAT.
 *              Handles updating only files that are already tracked in the index,
 *              skipping new or untracked files. Provides functions to process individual
 *              tracked files and iterate through all tracked files in the current index.
 * Date: 23/10/2025
 * Author: Aliago
 */

#include "commands/add/add.h"
#include "utils/repo.h"
#include "ui/messages.h"
#include <unistd.h>
#include <stdio.h>

int process_tracked_file(const char *repo_path, index_content_t *entry, add_context_t *ctx)
{
    char full_path[PATH_MAX];
    int written;

    written = snprintf(full_path, sizeof(full_path), "%s/%s", repo_path, entry->rel_path);
    if (written < 0 || (size_t)written >= sizeof(full_path)) {
        MSG_FILE_SKIPPED_PATH_TOO_LONG(repo_path, entry->rel_path);
        ctx->stats->errors++;
        return -1;
    }
    if (access(full_path, F_OK) == 0) {
        process_file(full_path, ctx);
    } else {
        MSG_FILE_SKIPPED_NOT_FOUND(entry->rel_path);
        ctx->stats->errors++;
    }
    return 0;
}

void process_all_tracked_files(const char *repo_path, add_context_t *ctx)
{
    index_content_t *current_entry = (index_content_t *)ctx->current_index;

    while (current_entry) {
        process_tracked_file(repo_path, current_entry, ctx);
        current_entry = current_entry->next;
    }
}

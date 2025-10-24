/*
 * File: add_handlers.c
 * Description: Implementation of wrapper functions for the 'add' command in GOAT.
 *              Contains the main execution logic for different add operations:
 *              - add_all_files_wrapper: handles '--all' flag functionality
 *              - update_tracked_files_wrapper: handles '--update' flag functionality
 *              Separated from add_internal.c for better code organization.
 * Date: 23/10/2025
 * Author: Aliago
 */

#include "commands/add/add.h"
#include "core/index.h"
#include "utils/repo.h"
#include "ui/messages.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int update_tracked_files_wrapper(void)
{
    char repo_path[PATH_MAX];
    index_content_t *content = NULL;
    index_content_t *current_index = NULL;
    add_stats_t stats;
    add_context_t ctx;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_FOUND;
        return 1;
    }
    repo_path[strlen(repo_path)-5] = '\0';
    MSG_ADD_UPDATE_HEADER;
    init_add_stats(&stats);
    current_index = read_current_index();
    init_add_context(&ctx, &stats, current_index);
    set_context_paths(&ctx, repo_path, &content);
    process_all_tracked_files(repo_path, &ctx);
    update_index(content);
    display_add_summary(&stats, false);
    free_index_content(current_index);
    free_index_content(content);
    return 0;
}

int add_all_files_wrapper(void)
{
    char repo_path[PATH_MAX];
    index_content_t *content = NULL;
    index_content_t *current_index = NULL;
    add_stats_t stats;
    add_context_t ctx;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_FOUND;
        return 1;
    }
    repo_path[strlen(repo_path)-5] = '\0';
    MSG_ADD_ALL_HEADER;
    init_add_stats(&stats);
    current_index = read_current_index();
    init_add_context(&ctx, &stats, current_index);
    set_context_paths(&ctx, repo_path, &content);
    list_all_files(repo_path, &ctx);
    update_index(content);
    display_add_summary(&stats, true);
    free_index_content(current_index);
    free_index_content(content);
    return 0;
}

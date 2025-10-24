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

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_FOUND;
        return 1;
    }
    repo_path[strlen(repo_path)-5] = '\0';
    current_index = read_current_index();
    process_all_tracked_files(repo_path, current_index, &content);
    update_index(content);
    free_index_content(current_index);
    free_index_content(content);
    return 0;
}

int add_all_files_wrapper(void)
{
    char repo_path[PATH_MAX];
    index_content_t *content = NULL;
    index_content_t *current_index = NULL;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_FOUND;
        return 1;
    }
    repo_path[strlen(repo_path)-5] = '\0';
    current_index = read_current_index();
    list_all_files(repo_path, repo_path, &content, current_index);
    update_index(content);
    free_index_content(current_index);
    free_index_content(content);
    return 0;
}

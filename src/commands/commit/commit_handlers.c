/*
 * File: commit_handlers.c
 * Description: Implementation of wrapper functions for the 'commit' command in GOAT.
 *              Contains the main execution logic for commit operations.
 *              Separated for better code organization following clean code principles.
 * Date: 30/10/2025
 * Author: Aliago
 */

#include "commands/commit/commit.h"
#include "ui/messages.h"
#include <stdlib.h>

static int execute_commit_process(commit_data_t *data)
{
    int result;

    result = write_commit_object(data);
    if (result != 0) {
        return result;
    }
    return update_head_reference(data);
}

static void cleanup_commit_data(commit_data_t *data)
{
    if (data->tree_content) {
        free(data->tree_content);
        data->tree_content = NULL;
    }
}

int commit_with_message_wrapper(const char *message)
{
    commit_data_t data = {0};
    int result;

    result = validate_commit_preconditions();
    if (result != 0)
        return result;
    result = prepare_commit_data(&data, message);
    if (result != 0)
        return result;
    result = execute_commit_process(&data);
    if (result != 0) {
        cleanup_commit_data(&data);
        return result;
    }
    MSG_COMMIT_SUCCESS(data.commit_hash, message);
    cleanup_commit_data(&data);
    return 0;
}

/*
 * File: commit.h
 * Description: Public interface for the 'commit' command in GOAT.
 *              Handles commit creation from staged files with message validation.
 *              Supports clean code architecture with small, focused functions.
 * Date: 30/10/2025
 * Author: Aliago
 */

#ifndef COMMIT_H
#define COMMIT_H

#include "core/command.h"

// Commit data structures
typedef struct {
    char *message;
    char *tree_content;
    char parent_hash[65];
    char timestamp[64];
    char commit_hash[65];
} commit_data_t;

typedef struct {
    char repo_path[512];
    char index_path[512];
    char head_path[512];
    char objects_dir[512];
} commit_paths_t;

// Public interface
int cmd_commit(cmd_opts_t *opts);
int parse_commit_options(int argc, char **argv, cmd_opts_t *opts);

// Command-specific option handlers
void set_commit_message(cmd_opts_t *opts);
void set_commit_help(cmd_opts_t *opts);

// Internal functions (exposed for testing)
int validate_commit_preconditions(void);
int prepare_commit_data(commit_data_t *data, const char *message);
int write_commit_object(const commit_data_t *data);
int update_head_reference(const commit_data_t *data);

// Wrapper functions
int commit_with_message_wrapper(const char *message);

#endif // COMMIT_H

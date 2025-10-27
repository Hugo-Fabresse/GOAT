/*
 * File: add.h
 * Description: Public interface for the 'add' command in GOAT.
 *              Handles file staging and index updates.
 *              Declares 'cmd_add' and 'parse_add_options', supporting
 *              '--all' and '--update' flags.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef ADD_H
#define ADD_H

#include "core/command.h"
#include "core/index.h"

// Number of handlers
#define ADD_HANDLERS_COUNT 2

// Structure for tracking add operation statistics
typedef struct add_stats {
    int added;
    int modified;
    int errors;
} add_stats_t;

// Structure for grouping add operation context
typedef struct add_context {
    const char *repo_root;
    index_content_t **content_list;
    add_stats_t *stats;
    const index_content_t *current_index;
} add_context_t;

// Structure for file data to reduce parameter count
typedef struct file_data {
    const char *rel_path;
    const char *hash_hex;
    const char *timestamp;
    const index_content_t *metadata;
} file_data_t;

// Structure for add option handlers
typedef struct add_handler {
    bool *flag;
    int (*func)(void);
} add_handler_t;

// Public interface
int cmd_add(cmd_opts_t *opts);
int parse_add_options(int argc, char **argv, cmd_opts_t *opts);

// Command-specific option handlers
void set_add_all(cmd_opts_t *opts);
void set_add_update(cmd_opts_t *opts);
void set_add_help(cmd_opts_t *opts);

// Wrapper functions (moved to add_handlers.c)
int update_tracked_files_wrapper(void);
int add_all_files_wrapper(void);
void set_handlers_flags(add_handler_t *handlers, cmd_opts_t *opts);

// Table of function pointers for add options (static to avoid multiple definitions)
static const add_handler_t add_handlers[] = {
        {NULL, add_all_files_wrapper},
        {NULL, update_tracked_files_wrapper}
};

// Functions used by wrappers
void list_all_files(const char *dir, add_context_t *ctx);
void process_file(const char *path, add_context_t *ctx);
void process_all_tracked_files(const char *repo_path, add_context_t *ctx);
int process_tracked_file(const char *repo_path, index_content_t *entry, add_context_t *ctx);

// Statistics and display functions
void init_add_stats(add_stats_t *stats);
void display_add_summary(const add_stats_t *stats, bool show_added);
void init_add_context(add_context_t *ctx, add_stats_t *stats, const index_content_t *index);
void set_context_paths(add_context_t *ctx, const char *repo_root, index_content_t **content);

#endif // ADD_H

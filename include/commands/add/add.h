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

// Structure for add option handlers
typedef struct add_handler {
    bool *flag;
    int (*func)(void);
} add_handler_t;

// Public interface
int cmd_add(cmd_opts_t *opts);
int parse_add_options(int argc, char **argv, cmd_opts_t *opts);

// Command-specific option handlers
void set_all(cmd_opts_t *opts);
void set_update(cmd_opts_t *opts);

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
void list_all_files(const char *dir, const char *repo_root, index_content_t **content_list, const index_content_t *current_index);
void process_file(const char *path, const char *repo_root, index_content_t **content_list, const index_content_t *current_index);
void process_all_tracked_files(const char *repo_path, index_content_t *current_index, index_content_t **content);
int process_tracked_file(const char *repo_path, index_content_t *current_entry, index_content_t **content, const index_content_t *current_index);

#endif // ADD_H

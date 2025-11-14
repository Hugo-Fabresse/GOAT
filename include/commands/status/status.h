/*
 * File: status.h
 * Description: Public interface for the 'status' command of GOAT.
 *              Displays the current state of the working directory:
 *              - Modified files (tracked but changed)
 *              - Staged files (ready to commit)
 *              - Untracked files (new files not tracked)
 * Date: 13/11/2025
 * Author: Aliago
 */

#ifndef STATUS_H
#define STATUS_H

#include <stdbool.h>
#include "core/goat.h"

// Status categories for files
typedef enum {
    FILE_UNTRACKED,
    FILE_MODIFIED,
    FILE_STAGED,
    FILE_UNMODIFIED
} file_status_t;

// File entry in status list
typedef struct file_entry {
    char *path;
    file_status_t status;
    struct file_entry *next;
} file_entry_t;

// Public interface
int cmd_status(cmd_opts_t *opts);
int parse_status_options(int argc, char **argv, cmd_opts_t *opts);

// Option handlers
void set_short_format(cmd_opts_t *opts);
void set_verbose(cmd_opts_t *opts);
void set_status_help(cmd_opts_t *opts);

// File list management
file_entry_t *create_file_entry(const char *path, file_status_t status);
void free_file_list(file_entry_t *head);
void add_file_entry(file_entry_t **head, const char *path, file_status_t status);

#endif // STATUS_H
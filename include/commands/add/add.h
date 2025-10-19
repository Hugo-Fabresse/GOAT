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

#endif // ADD_H

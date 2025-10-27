/*
 * File: init.h
 * Description: Public interface for the 'init' command in GOAT.
 *              Initializes a new repository (.goat/) with core structure.
 *              Declares 'cmd_init' and 'parse_init_options', supporting
 *              '--force' and '--quiet' flags.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef INIT_H
#define INIT_H

#include "core/command.h"

// Public interface
int cmd_init(cmd_opts_t *opts);
int parse_init_options(int argc, char **argv, cmd_opts_t *opts);

// Command-specific option handlers
void set_init_force(cmd_opts_t *opts);
void set_init_quiet(cmd_opts_t *opts);
void set_init_help(cmd_opts_t *opts);

#endif // INIT_H

/*
 * File: init.h
 * Description: Public interface for the 'init' command of GOAT.
 *              Provides the entry point to initialize a new GOAT repository.
 *              Exposes the command function 'cmd_init' and the options struct 'cmd_opts_t'
 *              which supports flags such as 'force' and 'quiet'.
 *              Ensures modularity and testability by separating the CLI interface from internal logic.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef INIT_H
#define INIT_H

#include <stdbool.h>

typedef struct cmd_opts {
    bool force;
    bool quiet;
} cmd_opts_t;

int cmd_init(const cmd_opts_t *opts);
int parse_init_options(int argc, char **argv, cmd_opts_t *opts);

#endif // INIT_H

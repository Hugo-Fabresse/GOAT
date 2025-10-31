/*
 * File: status.h
 * Description: ${DESCRIPTION}
 * Date: 31/10/2025
 * Author: Aliago
 */

#ifndef STATUS_H
#define STATUS_H

#include "core/command.h"

// Public interface
int cmd_status(cmd_opts_t *opts);
int parse_status_options(int argc, char **argv, cmd_opts_t *opts);

// Command-specific option handlers
void set_status_help(cmd_opts_t *opts);
void set_status_short(cmd_opts_t *opts);
void set_status_branch(cmd_opts_t *opts);

#endif // STATUS_H
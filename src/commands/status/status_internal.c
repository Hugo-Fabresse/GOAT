/*
 * File: status_internal.c
 * Description: Implementation of option handlers for the 'status' command.
 *              Provides modular functions to set specific flags in the command
 *              options structure.
 * Date: 13/11/2025
 * Author: Aliago
 */

#include "commands/status/status.h"

void set_short_format(cmd_opts_t *opts)
{
    if (opts) {
        opts->cmd_specific.status.short_format = true;
    }
}

void set_verbose(cmd_opts_t *opts)
{
    if (opts) {
        opts->cmd_specific.status.branch = true;
    }
}

void set_status_help(cmd_opts_t *opts)
{
    if (opts) {
        opts->cmd_specific.status.help = true;
    }
}
/*
 * File: status_internal.c
 * Description: ${DESCRIPTION}
 * Date: 31/10/2025
 * Author: Aliago
 */

#include "commands/status/status.h"

void set_status_help(cmd_opts_t *opts)
{
    opts->cmd_specific.status.help = true;
}

void set_status_short(cmd_opts_t *opts)
{
    opts->cmd_specific.status.short_format = true;
}

void set_status_branch(cmd_opts_t *opts)
{
    opts->cmd_specific.status.branch = true;
}

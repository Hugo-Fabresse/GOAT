/*
 * File: status.c
 * Description: ${DESCRIPTION}
 * Date: 31/10/2025
 * Author: Aliago
 */

#include "../../../include/commands/status/status.h"
#include "../../../include/core/command.h"
#include "../../../include/ui/messages.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Option definitions for status command
static const option_entry_t status_options[] = {
    {"--help", "Show this help message", set_status_help},
    {"--short", "Show status in short format", set_status_short},
    {"--branch", "Show branch information", set_status_branch}
};

// Command options structure
static const command_options_t status_cmd_opts = {
    .command_name = "status",
    .options = status_options,
    .num_options = sizeof(status_options)/sizeof(status_options[0])
};

int parse_status_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.status.help = false;
    opts->cmd_specific.status.short_format = false;
    opts->cmd_specific.status.branch = false;
    return parse_options(argc, argv, &status_cmd_opts, opts);
}

int cmd_status(cmd_opts_t *opts)
{
    // TODO: Implement status command logic
    return 0;
}

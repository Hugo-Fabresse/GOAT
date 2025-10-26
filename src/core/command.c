/*
 * File: command.c
 * Description: Implementation of common command utilities for GOAT.
 *              Provides option parsing and common option handlers.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "core/command.h"
#include "ui/messages.h"
#include "utils/log.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

void print_command_usage(const command_options_t *cmd_opts)
{
    MSG_COMMAND_USAGE_HEADER(cmd_opts->command_name);
    for (size_t i = 0; i < cmd_opts->num_options; i++) {
        MSG_COMMAND_OPTION(cmd_opts->options[i].name, cmd_opts->options[i].description);
    }
}

static int find_option_index(const command_options_t *cmd_opts, const char *option_name)
{
    for (size_t j = 0; j < cmd_opts->num_options; j++) {
        if (strcmp(option_name, cmd_opts->options[j].name) == 0) {
            return (int)j;
        }
    }
    return -1;
}

static int handle_option(const command_options_t *cmd_opts, cmd_opts_t *opts, const char *option_name)
{
    int idx = find_option_index(cmd_opts, option_name);

    if (idx >= 0) {
        cmd_opts->options[idx].set_option(opts);
        return 0;
    } else {
        MSG_UNKNOWN_OPTION(option_name);
        print_command_usage(cmd_opts);
        return -1;
    }
}

int parse_options(int argc, char **argv, const command_options_t *cmd_opts, cmd_opts_t *opts)
{
    for (int i = 2; i < argc; i++) {
        if (handle_option(cmd_opts, opts, argv[i]) < 0) {
            return -1;
        }
    }
    return 0;
}

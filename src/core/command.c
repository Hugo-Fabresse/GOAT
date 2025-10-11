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

// Print usage for a specific command
void print_command_usage(const char *command_name, const option_entry_t *options, size_t num_options) {
    MSG_COMMAND_USAGE_HEADER(command_name);

    for (size_t i = 0; i < num_options; i++) {
        MSG_COMMAND_OPTION(options[i].name, options[i].description);
    }
}

// Common option parsing utility
int parse_options(int argc, char **argv, const char *command_name,
                  const option_entry_t *options, size_t num_options,
                  cmd_opts_t *opts) {
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] != '-') {
            MSG_UNKNOWN_OPTION(argv[i]);
            print_command_usage(command_name, options, num_options);
            return -1;
        }

        const char *option_name = argv[i] + 2; // Skip "--"
        bool found = false;

        for (size_t j = 0; j < num_options; j++) {
            if (strcmp(option_name, options[j].name) == 0) {
                options[j].set_option(opts);
                found = true;
                break;
            }
        }

        if (!found) {
            MSG_UNKNOWN_OPTION(argv[i]);
            print_command_usage(command_name, options, num_options);
            return -1;
        }
    }
    return 0;
}

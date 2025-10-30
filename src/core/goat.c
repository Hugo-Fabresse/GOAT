/*
 * File: goat.c
 * Description: GOAT command dispatcher and core CLI logic.
 *              Contains the modular command table and the main goat() function for routing commands.
 *              Easily extensible: add new commands by updating the table.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/goat.h"
#include "commands/init/init.h"
#include "commands/add/add.h"
#include "commands/commit/commit.h"
#include "ui/messages.h"
#include <string.h>
#include <stddef.h>

// Table of supported commands
const command_entry_t commands[] = {
        {"init", parse_init_options, cmd_init},
        {"add",  parse_add_options,  cmd_add},
        {"commit", parse_commit_options, cmd_commit}
};

// Number of commands
static const size_t num_commands = sizeof(commands) / sizeof(commands[0]);

int print_usage(const char *progname)
{
    MSG_USAGE(progname);
    return 1;
}

int goat_exec_cmd(int argc, char **argv)
{
    cmd_opts_t opts = {0};

    for (size_t i = 0; i < num_commands; i++) {
        if (strcmp(argv[1], commands[i].name) == 0) {
            if (commands[i].parse_options(argc, argv, &opts) < 0) {
                return 1;
            }
            return commands[i].execute(&opts);
        }
    }
    MSG_UNKNOWN_COMMAND(argv[1]);
    return print_usage(argv[0]);
}

int goat(int argc, char **argv)
{
    if (argc < 2) {
        return print_usage(argv[0]);
    }
    return goat_exec_cmd(argc, argv);
}

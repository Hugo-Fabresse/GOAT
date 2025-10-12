/*
 * File: add.c
 * Description: Core implementation of the 'add' command in GOAT.
 *              Handles file staging and index updates within an existing repository.
 *              Supports '--all' to stage all changes and '--update' to refresh tracked files only.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "commands/add/add.h"
#include "core/command.h"
#include "utils/fs.h"
#include "utils/repo.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdbool.h>

// Option definitions for add command
static const option_entry_t add_options[] = {
    {"--all", "Add all modified files to the staging area", set_all},
    {"--update", "Update only files that are already tracked", set_update}
};

// Command options structure
static const command_options_t add_cmd_opts = {
    .command_name = "add",
    .options = add_options,
    .num_options = sizeof(add_options)/sizeof(add_options[0])
};

int parse_add_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.add.all = false;
    opts->cmd_specific.add.update = false;
    return parse_options(argc, argv, &add_cmd_opts, opts);
}

int cmd_add(const cmd_opts_t *opts)
{
    (void)opts;

    if (!check_already_initialized()) {
        MSG_NO_REPO_EXISTS;
        return 1;
    }
    return 0;
}

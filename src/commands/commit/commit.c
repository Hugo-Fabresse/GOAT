/*
 * File: commit.c
 * Description: CLI interface for the 'commit' command in GOAT.
 *              Handles command line parsing and creates commit objects from staged files.
 *              Integrates with existing index, hash, and filesystem utilities.
 * Date: 30/10/2025
 * Author: Aliago
 */

#include "commands/commit/commit.h"
#include "core/command.h"
#include "core/index.h"
#include "core/hash.h"
#include "utils/fs.h"
#include "utils/path.h"
#include "utils/timestamp.h"
#include "utils/repo.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variable to store commit message
static char *commit_message = NULL;

// Option definitions for commit command
static const option_entry_t commit_options[] = {
    {"-m", "Commit message", set_commit_message},
    {"--help", "Show this help message", set_commit_help}
};

// Command options structure
static const command_options_t commit_cmd_opts = {
    .command_name = "commit",
    .options = commit_options,
    .num_options = sizeof(commit_options)/sizeof(commit_options[0])
};

int parse_commit_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.commit.message = false;
    opts->cmd_specific.commit.help = false;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            commit_message = argv[i + 1];
            opts->cmd_specific.commit.message = true;
            i++;
        } else if (strcmp(argv[i], "--help") == 0) {
            opts->cmd_specific.commit.help = true;
        }
    }
    return 0;
}

int cmd_commit(cmd_opts_t *opts)
{
    if (opts->cmd_specific.commit.help) {
        print_command_usage(&commit_cmd_opts);
        return 0;
    }
    if (!opts->cmd_specific.commit.message || !commit_message) {
        MSG_COMMIT_NO_MESSAGE;
        return 4;
    }
    return commit_with_message_wrapper(commit_message);
}

/*
 * File: init.c
 * Description: Core implementation of the 'init' command in GOAT.
 *              Initializes a new repository by creating the .goat/ structure
 *              with all required subdirectories and files.
 *              Supports '--force' for reinitialization and '--quiet' for silent mode.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "commands/init/init.h"
#include "core/command.h"
#include "utils/fs.h"
#include "utils/repo.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Paths to create with their modes and types
static const struct path_mode {
    const char *path;
    int mode;
    bool is_dir;
} init_paths[] = {
        {GOAT_DIR,    0755, true},
        {OBJECTS_DIR, 0755, true},
        {REFS_DIR,    0755, true},
        {HEADS_DIR,   0755, true},
        {HEAD_FILE,   0644, false},
        {CONFIG_FILE, 0644, false},
        {INDEX_FILE,  0644, false}
};

// Option definitions for init command
static const option_entry_t init_options[] = {
    {"--force", "Reinitialize and overwrite an existing repository", set_init_force},
    {"--quiet", "Suppress output messages", set_init_quiet},
    {"--help", "Show this help message", set_init_help}
};

// Command options structure
static const command_options_t init_cmd_opts = {
    .command_name = "init",
    .options = init_options,
    .num_options = sizeof(init_options)/sizeof(init_options[0])
};

static int create_path(const struct path_mode *p)
{
    int res = p->is_dir ? fs_create_dir(p->path, p->mode)
                        : fs_create_file(p->path, p->mode);

    if (res < 0) {
        if (p->is_dir) {
            MSG_CREATE_DIR_FAILED(p->path);
        }
        else {
            MSG_CREATE_FILE_FAILED(p->path);
        }
        return -1;
    }
    return 0;
}

static int create_structure(void)
{
    size_t init_count = sizeof(init_paths)/sizeof(init_paths[0]);

    for (size_t i = 0; i < init_count; i++) {
        if (create_path(&init_paths[i]) < 0) {
            return -1;
        }
    }
    return 0;
}

int parse_init_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.init.force = false;
    opts->cmd_specific.init.quiet = false;
    opts->cmd_specific.init.help = false;
    return parse_options(argc, argv, &init_cmd_opts, opts);
}

int cmd_init(cmd_opts_t *opts)
{
    if (opts->cmd_specific.init.help) {
        print_command_usage(&init_cmd_opts);
        return 0;
    }
    if (check_already_initialized() && !opts->cmd_specific.init.force) {
        MSG_REPO_EXISTS;
        return 1;
    }
    if (create_structure() < 0) {
        MSG_INIT_FAILURE;
        return 2;
    }
    if (!opts->cmd_specific.init.quiet) {
        MSG_INIT_SUCCESS;
    }
    return 0;
}

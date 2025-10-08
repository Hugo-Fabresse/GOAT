/*
 * File: init.c
 * Description: Implementation of the 'init' command for GOAT.
 *              Provides the entry point to initialize a new GOAT repository.
 *              Creates the repository structure including .goat/, objects/, refs/heads/,
 *              HEAD, config, and index, with proper error handling.
 *              Supports command options such as 'force' and 'quiet'.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "commands/init/init.h"
#include "utils/fs.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Repository structure constants
#define GOAT_DIR       ".goat"
#define OBJECTS_DIR    ".goat/objects"
#define REFS_DIR       ".goat/refs"
#define HEADS_DIR      ".goat/refs/heads"
#define HEAD_FILE      ".goat/HEAD"
#define CONFIG_FILE    ".goat/config"
#define INDEX_FILE     ".goat/index"

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

// Option mapping array
static const option_entry_t option_map[] = {
        {"--force", set_force},
        {"--quiet", set_quiet}
};

static bool check_already_initialized(void)
{
    return fs_dir_exists(GOAT_DIR);
}

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

static int handle_option(const char *arg, cmd_opts_t *opts)
{
    size_t option_count = sizeof(option_map)/sizeof(option_map[0]);

    for (size_t j = 0; j < option_count; j++) {
        if (strcmp(arg, option_map[j].name) == 0) {
            option_map[j].set_option(opts);
            return 0;
        }
    }
    MSG_UNKNOWN_OPTION(arg);
    return -1;
}

int parse_init_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->force = false;
    opts->quiet = false;

    for (int i = 2; i < argc; i++) {
        if (handle_option(argv[i], opts) < 0) {
            return -1;
        }
    }
    return 0;
}

int cmd_init(const cmd_opts_t *opts)
{
    if (check_already_initialized() && !opts->force) {
        MSG_REPO_EXISTS;
        return 1;
    }
    if (create_structure() < 0) {
        MSG_INIT_FAILURE;
        return 2;
    }
    if (!opts->quiet) {
        MSG_INIT_SUCCESS;
    }
    return 0;
}

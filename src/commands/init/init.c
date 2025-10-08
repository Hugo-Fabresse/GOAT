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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define GOAT_DIR       ".goat"
#define OBJECTS_DIR    ".goat/objects"
#define REFS_DIR       ".goat/refs"
#define HEADS_DIR      ".goat/refs/heads"
#define HEAD_FILE      ".goat/HEAD"
#define CONFIG_FILE    ".goat/config"
#define INDEX_FILE     ".goat/index"

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

static bool check_already_initialized(void)
{
    return fs_dir_exists(GOAT_DIR);
}

static int create_structure(void)
{
    int count = sizeof(init_paths) / sizeof(init_paths[0]);
    int res = 0;

    for (int i = 0; i < count; i++) {
        res = init_paths[i].is_dir
                  ? fs_create_dir(init_paths[i].path, init_paths[i].mode)
                  : fs_create_file(init_paths[i].path, init_paths[i].mode);
        if (res < 0) {
            fprintf(stderr, "Failed to create %s\n", init_paths[i].path);
            return -1;
        }
    }
    return 0;
}

int parse_init_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->force = false;
    opts->quiet = false;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--force") == 0) {
            opts->force = true;
        } else if (strcmp(argv[i], "--quiet") == 0) {
            opts->quiet = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return -1;
        }
    }
    return 0;
}

int cmd_init(const cmd_opts_t *opts)
{
    if (check_already_initialized() && !opts->force) {
        fprintf(stderr, "GOAT repository already exists.\n");
        return 1;
    }
    if (create_structure() < 0) {
        fprintf(stderr, "Failed to initialize GOAT repository.\n");
        return 2;
    }
    if (!opts->quiet) {
        printf("Initialized empty GOAT repository in ./.goat\n");
    }
    return 0;
}

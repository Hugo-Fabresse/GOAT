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
#include "core/hash.h"
#include "core/index.h"
#include "utils/fs.h"
#include "utils/repo.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>

void list_all_files(const char *dir, const char *repo_root);

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

static void process_file(const char *path, const char *repo_root)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    const char *rel_path;

    if (access(path, R_OK) != 0) {
        fprintf(stderr, "Unable to read : %s\n", path + strlen(repo_root) + 1);
        return;
    }
    if (hash_file_sha256(path, hash) != 0) {
        fprintf(stderr, "Hash error : %s\n", path);
        return;
    }
    rel_path = path + strlen(repo_root) + 1;
    printf("Staged: %s\n", rel_path);
    update_index(rel_path, hash);
}

static void process_entry(const char *dir, const struct dirent *entry, const char *repo_root)
{
    char path[PATH_MAX];
    struct stat st;

    snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
    if (stat(path, &st) == -1) {
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        list_all_files(path, repo_root);
    } else if (S_ISREG(st.st_mode)) {
        process_file(path, repo_root);
    }
}

void list_all_files(const char *dir, const char *repo_root)
{
    DIR *d = opendir(dir);
    struct dirent *entry;

    if (!d) {
        return;
    }
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, ".goat") == 0)
            continue;
        process_entry(dir, entry, repo_root);
    }
    closedir(d);
}

int parse_add_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.add.all = false;
    opts->cmd_specific.add.update = false;
    return parse_options(argc, argv, &add_cmd_opts, opts);
}

int cmd_add(const cmd_opts_t *opts)
{
    char repo_path[PATH_MAX];

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_EXISTS;
        return 1;
    }
    repo_path[strlen(repo_path) - 5] = '\0';
    if (opts->cmd_specific.add.all) {
        clean_index(repo_path);
        list_all_files(repo_path, repo_path);
        return 0;
    }
    return 0;
}

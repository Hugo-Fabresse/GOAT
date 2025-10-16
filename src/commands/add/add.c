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
#include "utils/std_ext.h"
#include "utils/timestamp.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <stdlib.h>

void list_all_files(const char *dir, const char *repo_root, index_content_t **content_list);

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

static int validate_file_access(const char *path, const char *repo_root)
{
    if (access(path, R_OK) != 0) {
        fprintf(stderr, "Unable to read : %s\n", path + strlen(repo_root) + 1);
        return -1;
    }
    return 0;
}

static int prepare_file_data(const char *path, unsigned char *hash, char *timestamp_buffer, size_t bufsize)
{
    if (get_file_timestamp(path, timestamp_buffer, bufsize) != 0) {
        fprintf(stderr, "Timestamp error : %s\n", path);
        return -1;
    }
    if (hash_file_sha256(path, hash) != 0) {
        fprintf(stderr, "Hash error : %s\n", path);
        return -1;
    }
    return 0;
}

static index_content_t *create_index_node(const char *rel_path, const unsigned char *hash, const char *timestamp)
{
    index_content_t *new_node = malloc(sizeof(index_content_t));

    if (!new_node) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    new_node->rel_path = strdup(rel_path);
    memcpy(new_node->hash, hash, SHA256_DIGEST_LENGTH);
    new_node->timestamp = strdup(timestamp);
    new_node->next = NULL;
    return new_node;
}

static void process_file(const char *path, const char *repo_root, index_content_t **content_list)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char timestamp_buffer[32];
    const char *rel_path;
    index_content_t *new_node;

    if (validate_file_access(path, repo_root) != 0)
        return;
    if (prepare_file_data(path, hash, timestamp_buffer, sizeof(timestamp_buffer)) != 0)
        return;
    rel_path = path + strlen(repo_root) + 1;
    printf("Staged: %s\n", rel_path);
    new_node = create_index_node(rel_path, hash, timestamp_buffer);
    if (!new_node)
        return;
    new_node->next = *content_list;
    *content_list = new_node;
}

static void process_entry(const char *dir, const struct dirent *entry, const char *repo_root, index_content_t **content_list)
{
    char path[PATH_MAX];
    struct stat st;

    snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
    if (stat(path, &st) == -1) {
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        list_all_files(path, repo_root, content_list);
    } else if (S_ISREG(st.st_mode)) {
        process_file(path, repo_root, content_list);
    }
}

void list_all_files(const char *dir, const char *repo_root, index_content_t **content_list)
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
        process_entry(dir, entry, repo_root, content_list);
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
    index_content_t *content = NULL;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_EXISTS;
        return 1;
    }
    repo_path[strlen(repo_path) - 5] = '\0';
    if (opts->cmd_specific.add.all) {
        list_all_files(repo_path, repo_path, &content);
        update_index(content);
        return 0;
    }
    return 0;
}

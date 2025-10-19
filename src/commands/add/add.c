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
#include "core/blob.h"
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

static void fill_metadata(index_content_t *metadata, const struct stat *st)
{
    metadata->mode   = st->st_mode;
    metadata->size   = st->st_size;
    metadata->mtime  = st->st_mtime;
    metadata->ctime  = st->st_ctime;
    metadata->device = st->st_dev;
    metadata->inode  = st->st_ino;
}

static int prepare_file(index_content_t *metadata, const char *path, char *hash_hex, char *timestamp)
{
    struct stat st;
    unsigned char hash_bin[SHA256_DIGEST_LENGTH];

    if (stat(path, &st) != 0)
        return -1;
    fill_metadata(metadata, &st);
    if (get_file_timestamp(path, timestamp, 32) != 0)
        return -1;
    if (hash_file_sha256(path, hash_bin) != 0)
        return -1;
    hash_to_hex(hash_bin, hash_hex);
    if (store_blob(hash_bin, path) != 0)
        return -1;
    return 0;
}

static bool file_changed(const char *path, const index_content_t *cached)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return true;
    return st.st_mtime != cached->mtime ||
           st.st_ctime != cached->ctime ||
           st.st_size != cached->size ||
           st.st_dev != cached->device ||
           st.st_ino != cached->inode ||
           st.st_mode != cached->mode;
}

static index_content_t *create_index_node(const char *rel_path, const char *hash_hex, const char *timestamp, const index_content_t *metadata)
{
    index_content_t *node = malloc(sizeof(index_content_t));

    if (!node)
        return NULL;
    node->rel_path = strdup(rel_path);
    node->timestamp = strdup(timestamp);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sscanf(hash_hex + i*2, "%2hhx", &node->hash[i]);
    node->mode = metadata->mode;
    node->size = metadata->size;
    node->mtime = metadata->mtime;
    node->ctime = metadata->ctime;
    node->device = metadata->device;
    node->inode = metadata->inode;
    node->next = NULL;
    return node;
}

static int validate_file_access(const char *path, const char *repo_root)
{
    if (access(path, R_OK) != 0) {
        fprintf(stderr, "Unable to read: %s\n", path + strlen(repo_root) + 1);
        return -1;
    }
    return 0;
}

static void process_file(const char *path, const char *repo_root, index_content_t **content_list, const index_content_t *current_index)
{
    char hash_hex[HASH_HEX_SIZE], timestamp[32];
    const char *rel_path = path + strlen(repo_root) + 1;
    index_content_t metadata = {0};
    index_content_t *node;
    index_content_t *cached = find_index_entry(current_index, rel_path);

    if (validate_file_access(path, repo_root) != 0)
        return;
    if (cached && !file_changed(path, cached)) {
        memcpy(metadata.hash, cached->hash, SHA256_DIGEST_LENGTH);
        strncpy(timestamp, cached->timestamp, sizeof(timestamp)-1);
        timestamp[sizeof(timestamp)-1] = '\0';
    } else if (prepare_file(&metadata, path, hash_hex, timestamp) != 0)
        return;
    node = create_index_node(rel_path, hash_hex, timestamp, &metadata);
    if (!node)
        return;
    node->next = *content_list;
    *content_list = node;
    printf("Staged: %s\n", rel_path);
}

void list_all_files(const char *dir, const char *repo_root, index_content_t **content_list, const index_content_t *current_index)
{
    DIR *d = opendir(dir);
    struct dirent *entry;
    char path[PATH_MAX];
    struct stat st;

    if (!d)
        return;
    while ((entry = readdir(d))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".goat") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        if (stat(path, &st) == -1)
            continue;
        if (S_ISDIR(st.st_mode))
            list_all_files(path, repo_root, content_list, current_index);
        else if (S_ISREG(st.st_mode))
            process_file(path, repo_root, content_list, current_index);
    }
    closedir(d);
}

int parse_add_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.add.all = false;
    opts->cmd_specific.add.update = false;
    return parse_options(argc, argv, &add_cmd_opts, opts);
}

static int add_all_files_wrapper(void)
{
    char repo_path[PATH_MAX];
    index_content_t *content = NULL;
    index_content_t *current_index = NULL;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        fprintf(stderr, "No repository found.\n");
        return 1;
    }
    repo_path[strlen(repo_path)-5] = '\0';
    current_index = read_current_index();
    list_all_files(repo_path, repo_path, &content, current_index);
    update_index(content);
    free_index_content(current_index);
    free_index_content(content);
    return 0;
}

// Table of function pointers for add options
const add_handler_t add_handlers[] = {
        { NULL, add_all_files_wrapper }
};

static void set_handlers_flags(add_handler_t *handlers, cmd_opts_t *opts)
{
    handlers[0].flag = &opts->cmd_specific.add.all;
}

int cmd_add(cmd_opts_t *opts)
{
    add_handler_t handlers[sizeof(add_handlers)/sizeof(add_handlers[0])];

    memcpy(handlers, add_handlers, sizeof(handlers));
    set_handlers_flags(handlers, opts);
    for (size_t i = 0; i < sizeof(handlers)/sizeof(handlers[0]); i++) {
        if (*(handlers[i].flag))
            return handlers[i].func();
    }
    return 0;
}

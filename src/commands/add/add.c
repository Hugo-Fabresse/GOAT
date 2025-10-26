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
    metadata->mode = st->st_mode;
    metadata->size = st->st_size;
    metadata->mtime = st->st_mtime;
    metadata->ctime = st->st_ctime;
    metadata->device = st->st_dev;
    metadata->inode = st->st_ino;
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

void init_add_stats(add_stats_t *stats)
{
    stats->added = 0;
    stats->modified = 0;
    stats->errors = 0;
}

void display_add_summary(const add_stats_t *stats, bool show_added)
{
    if (show_added && stats->added > 0) {
        MSG_ADD_SUMMARY_WITH_ADDED(stats->added, stats->modified);
    } else {
        MSG_ADD_SUMMARY_SIMPLE(stats->modified);
    }
    if (stats->errors > 0) {
        MSG_ADD_ERRORS(stats->errors);
    }
}

void init_add_context(add_context_t *ctx, add_stats_t *stats, const index_content_t *index)
{
    ctx->stats = stats;
    ctx->current_index = index;
}

void set_context_paths(add_context_t *ctx, const char *repo_root, index_content_t **content)
{
    ctx->repo_root = repo_root;
    ctx->content_list = content;
}

static int validate_file_access(const char *path, add_context_t *ctx)
{
    if (access(path, R_OK) != 0) {
        MSG_FILE_ACCESS_ERROR(path + strlen(ctx->repo_root) + 1);
        ctx->stats->errors++;
        return -1;
    }
    return 0;
}

static void prepare_metadata_from_cache(index_content_t *metadata, char *timestamp, const index_content_t *cached)
{
    memcpy(metadata->hash, cached->hash, SHA256_DIGEST_LENGTH);
    strncpy(timestamp, cached->timestamp, sizeof(timestamp)-1);
    timestamp[sizeof(timestamp)-1] = '\0';
}

static int add_node_to_list(const file_data_t *file_data, add_context_t *ctx)
{
    index_content_t *node = create_index_node(file_data->rel_path, file_data->hash_hex, file_data->timestamp, file_data->metadata);

    if (!node) {
        ctx->stats->errors++;
        return -1;
    }
    node->next = *(ctx->content_list);
    *(ctx->content_list) = node;
    return 0;
}

static void update_file_stats(const char *rel_path, bool is_new_file, add_context_t *ctx)
{
    if (is_new_file) {
        MSG_FILE_ADDED(rel_path);
        ctx->stats->added++;
    } else {
        MSG_FILE_MODIFIED(rel_path);
        ctx->stats->modified++;
    }
}

void process_file(const char *path, add_context_t *ctx)
{
    char hash_hex[HASH_HEX_SIZE], timestamp[32];
    const char *rel_path = path + strlen(ctx->repo_root) + 1;
    index_content_t metadata = {0};
    index_content_t *cached = find_index_entry(ctx->current_index, rel_path);
    bool is_new_file = (cached == NULL);
    file_data_t file_data;

    if (validate_file_access(path, ctx) != 0)
        return;
    if (cached && !file_changed(path, cached)) {
        prepare_metadata_from_cache(&metadata, timestamp, cached);
    } else if (prepare_file(&metadata, path, hash_hex, timestamp) != 0) {
        ctx->stats->errors++;
        return;
    }
    file_data.rel_path = rel_path;
    file_data.hash_hex = hash_hex;
    file_data.timestamp = timestamp;
    file_data.metadata = &metadata;
    if (add_node_to_list(&file_data, ctx) != 0)
        return;
    update_file_stats(rel_path, is_new_file, ctx);
}

int parse_add_options(int argc, char **argv, cmd_opts_t *opts)
{
    opts->cmd_specific.add.all = false;
    opts->cmd_specific.add.update = false;
    return parse_options(argc, argv, &add_cmd_opts, opts);
}

int cmd_add(cmd_opts_t *opts)
{
    add_handler_t handlers[ADD_HANDLERS_COUNT];

    memcpy(handlers, add_handlers, sizeof(handlers));
    set_handlers_flags(handlers, opts);
    for (size_t i = 0; i < ADD_HANDLERS_COUNT; i++) {
        if (*(handlers[i].flag))
            return handlers[i].func();
    }
    return 0;
}

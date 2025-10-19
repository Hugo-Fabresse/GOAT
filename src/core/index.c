/*
 * File: index.c
 * Description: Index management implementation for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/index.h"
#include "core/hash.h"
#include "utils/repo.h"
#include "utils/path.h"
#include "utils/fs.h"
#include "utils/std_ext.h"
#include "utils/timestamp.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static int setup_index_file_path(char *index_path, size_t path_size)
{
    char repo_root[PATH_MAX];

    if (!find_goat_repo(repo_root, sizeof(repo_root))) {
        MSG_NO_REPO_EXISTS;
        return -1;
    }
    if (snprintf(index_path, path_size, "%s/index", repo_root) >= (int)path_size) {
        MSG_UPDATE_INDEX_FAILED(index_path);
        return -1;
    }
    return 0;
}

static void parse_hash(const char *hash_str, unsigned char *hash)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH && i*2 < (int)strlen(hash_str); i++)
        sscanf(hash_str + i*2, "%2hhx", &hash[i]);
}

static index_content_t* create_index_entry(const char *rel_path, const unsigned char *hash, const char *timestamp)
{
    index_content_t *entry = malloc(sizeof(index_content_t));

    if (!entry)
        return NULL;
    entry->rel_path = strdup(rel_path);
    entry->timestamp = strdup(timestamp);
    memcpy(entry->hash, hash, SHA256_DIGEST_LENGTH);
    entry->mode = 0; entry->size = 0;
    entry->mtime = 0; entry->ctime = 0;
    entry->device = 0; entry->inode = 0;
    entry->next = NULL;
    return entry;
}

static index_content_t* parse_index_line(const char *line)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    index_content_t *entry;
    char *copy = strdup(line);
    char *path;
    char *hash_str;
    char *timestamp_str;

    if (!copy)
        return NULL;
    path = strtok(copy, " ");
    hash_str = strtok(NULL, " ");
    timestamp_str = strtok(NULL, "\n");
    if (!path || !hash_str || !timestamp_str) {
        free(copy);
        return NULL;
    }
    parse_hash(hash_str, hash);
    entry = create_index_entry(path, hash, timestamp_str);
    free(copy);
    return entry;
}

static index_content_t* append_index_entry(index_content_t *head, index_content_t *node)
{
    index_content_t *cur;

    if (!head)
        return node;
    cur = head;
    while (cur->next)
        cur = cur->next;
    cur->next = node;
    return head;
}

static void write_index_entries(FILE *index_file, const index_content_t *content)
{
    const index_content_t *current = content;
    char hex_hash[HASH_HEX_SIZE];
    char readable_time[64];

    while (current) {
        hash_to_hex(current->hash, hex_hash);
        format_timestamp(current->timestamp, readable_time, sizeof(readable_time));
        fprintf(index_file, "%s %s %s\n",
                current->rel_path ? current->rel_path : "no_path",
                hex_hash,
                readable_time);
        current = current->next;
    }
}

void update_index(const index_content_t *content)
{
    char index_path[PATH_MAX];
    FILE *index_file;

    if (setup_index_file_path(index_path, sizeof(index_path)) != 0)
        return;
    index_file = fopen(index_path, "w");
    if (!index_file) {
        MSG_INDEX_OPEN_FAILED(index_path);
        return;
    }
    write_index_entries(index_file, content);
    fclose(index_file);
}

index_content_t* read_current_index(void)
{
    char index_path[PATH_MAX];
    FILE *index_file;
    char line[1024];
    index_content_t *head = NULL;
    index_content_t *entry;

    if (setup_index_file_path(index_path, sizeof(index_path)) != 0)
        return NULL;
    index_file = fopen(index_path, "r");
    if (!index_file)
        return NULL;
    while (fgets(line, sizeof(line), index_file)) {
        entry = parse_index_line(line);
        if (entry)
            head = append_index_entry(head, entry);
    }
    fclose(index_file);
    return head;
}

index_content_t* find_index_entry(const index_content_t *index_list, const char *rel_path)
{
    const index_content_t *current = index_list;

    while (current) {
        if (current->rel_path && strcmp(current->rel_path, rel_path) == 0)
            return (index_content_t*)current;
        current = current->next;
    }
    return NULL;
}

void free_index_content(index_content_t *content)
{
    index_content_t *next;
    while (content) {
        next = content->next;
        free((void*)content->rel_path);
        free((void*)content->timestamp);
        free(content);
        content = next;
    }
}

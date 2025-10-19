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

static void write_index_entries(FILE *index_file, const index_content_t *content)
{
    const index_content_t *current = content;
    char hex_hash[HASH_HEX_SIZE];
    char readable_time[64];

    while (current != NULL) {
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

    if (setup_index_file_path(index_path, sizeof(index_path)) != 0) {
        return;
    }
    index_file = fopen(index_path, "w");
    if (!index_file) {
        MSG_INDEX_OPEN_FAILED(index_path);
        return;
    }
    write_index_entries(index_file, content);
    fclose(index_file);
}

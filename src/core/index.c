/*
 * File: index.c
 * Description: Index management implementation for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/index.h"
#include "utils/repo.h"
#include "utils/path.h"
#include "utils/fs.h"
#include "utils/std_ext.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

static void free_index_table(char ***table, int num_lines, int *num_words_per_line)
{
    if (!table || !num_words_per_line)
        return;
    for (int i = 0; i < num_lines; ++i) {
        for (int j = 0; j < num_words_per_line[i]; ++j) {
            free(table[i][j]);
        }
        free(table[i]);
    }
    free(table);
    free(num_words_per_line);
}

static int find_path_in_index(char ***table, int num_lines, int *num_words_per_line, const char *rel_path)
{
    for (int i = 0; i < num_lines; ++i) {
        if (num_words_per_line[i] >= 2 && strcmp(table[i][0], rel_path) == 0) {
            return i;
        }
    }
    return -1;
}

static void write_index_table(FILE *f, char ***table, int num_lines, int *num_words_per_line)
{
    for (int i = 0; i < num_lines; ++i) {
        for (int j = 0; j < num_words_per_line[i]; ++j) {
            fprintf(f, "%s%s", table[i][j], (j < num_words_per_line[i] - 1) ? " " : "");
        }
        fprintf(f, "\n");
    }
}

static void make_hash_str(const unsigned char *hash, char *hash_str)
{
    for (int i = 0; i < 32; ++i)
        sprintf(hash_str + 2*i, "%02x", hash[i]);
    hash_str[64] = '\0';
}

static void make_timestamp(char *timestamp, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timestamp, size, "%Y-%m-%dT%H:%M:%S", tm_info);
}

static char **make_index_line(const char *rel_path, const char *hash_str, const char *timestamp)
{
    char **line = malloc(3 * sizeof(char *));

    line[0] = strdup(rel_path);
    line[1] = strdup(hash_str);
    line[2] = strdup(timestamp);
    return line;
}

static int realloc_index_table(index_context_t *ctx)
{
    int num_lines = *ctx->pnum_lines;
    char ***table = *ctx->ptable;
    int *num_words = *ctx->pnum_words;
    char ***new_table = realloc(table, (num_lines + 1) * sizeof(char **));
    int *new_num_words = realloc(num_words, (num_lines + 1) * sizeof(int));
    if (!new_table || !new_num_words) {
        return -1;
    }
    *ctx->ptable = new_table;
    *ctx->pnum_words = new_num_words;
    return 0;
}

static int add_new_index_entry(index_context_t *ctx, const char *rel_path, const unsigned char *hash, const char *timestamp)
{
    char hash_str[65];
    char **new_line;
    int num_lines = 0;
    char ***table;
    int *num_words;

    make_hash_str(hash, hash_str);
    new_line = make_index_line(rel_path, hash_str, timestamp);
    if (realloc_index_table(ctx) == -1) {
        for (int i = 0; i < 3; ++i)
            free(new_line[i]);
        free(new_line);
        return -1;
    }
    num_lines = *ctx->pnum_lines;
    table = *ctx->ptable;
    num_words = *ctx->pnum_words;
    table[num_lines] = new_line;
    num_words[num_lines] = 3;
    (*ctx->pnum_lines)++;
    return 0;
}

static void update_index_entry(char ***table, int idx, const unsigned char *hash, const char *timestamp)
{
    char hash_str[65];

    make_hash_str(hash, hash_str);
    free(table[idx][1]);
    table[idx][1] = strdup(hash_str);
    if (table[idx][2]) {
        free(table[idx][2]);
        table[idx][2] = strdup(timestamp);
    }
}

static void create_new_index_file(const char *index_path, const char *rel_path, const unsigned char *hash, const char *timestamp)
{
    FILE *f = fopen(index_path, "a");
    char hash_str[65];

    if (!f) {
        fprintf(stderr, MSG_UPDATE_INDEX_FAILED, index_path);
        return;
    }
    make_hash_str(hash, hash_str);
    fprintf(f, "%s %s %s\n", rel_path, hash_str, timestamp);
    fclose(f);
}

static void save_index_table(const char *index_path, char ***table, int num_lines, int *num_words_per_line)
{
    FILE *f = fopen(index_path, "w");

    if (!f) {
        free_index_table(table, num_lines, num_words_per_line);
        fprintf(stderr, MSG_UPDATE_INDEX_FAILED, index_path);
        return;
    }
    write_index_table(f, table, num_lines, num_words_per_line);
    fclose(f);
    free_index_table(table, num_lines, num_words_per_line);
}

static void remove_index_entry(index_context_t *ctx, int idx)
{
    char ***table = *ctx->ptable;
    int *num_words = *ctx->pnum_words;
    int num_lines = *ctx->pnum_lines;

    for (int j = 0; j < num_words[idx]; ++j) {
        free(table[idx][j]);
    }
    free(table[idx]);
    for (int i = idx; i < num_lines - 1; ++i) {
        table[i] = table[i + 1];
        num_words[i] = num_words[i + 1];
    }
    (*ctx->pnum_lines)--;
}

static bool file_exists_in_repo(const char *repo_root, const char *rel_path)
{
    char full_path[PATH_MAX];

    if (snprintf(full_path, sizeof(full_path), "%s/%s", repo_root, rel_path) >= (int)sizeof(full_path)) {
        return false;
    }
    return access(full_path, F_OK) == 0;
}

void clean_index(const char *repo_root)
{
    FILE *f;
    index_context_t ctx;
    char index_path[PATH_MAX];
    int num_lines = 0;
    int *num_words_per_line = NULL;
    char ***table = NULL;

    if (snprintf(index_path, sizeof(index_path), "%s/.goat/index", repo_root) >= (int)sizeof(index_path)) {
        return;
    }
    table = read_words_from_file(index_path, &num_lines, &num_words_per_line);
    if (!table) {
        return;
    }
    ctx.ptable = &table;
    ctx.pnum_words = &num_words_per_line;
    ctx.pnum_lines = &num_lines;
    for (int i = num_lines - 1; i >= 0; --i) {
        if (num_words_per_line[i] >= 1) {
            if (!file_exists_in_repo(repo_root, table[i][0])) {
                remove_index_entry(&ctx, i);
            }
        }
    }
    f = fopen(index_path, "w");
    if (f) {
        write_index_table(f, table, *ctx.pnum_lines, *ctx.pnum_words);
        fclose(f);
    }
    free_index_table(table, *ctx.pnum_lines, *ctx.pnum_words);
}

void update_index(const char *rel_path, const unsigned char *hash)
{
    char repo_root[PATH_MAX];
    char index_path[PATH_MAX];
    int num_lines = 0;
    int *num_words_per_line = NULL;
    char ***table = NULL;
    int idx;
    char timestamp[32];
    index_context_t ctx;

    if (!find_goat_repo(repo_root, sizeof(repo_root))) {
        MSG_NO_REPO_EXISTS;
        return;
    }
    if (snprintf(index_path, sizeof(index_path), "%s/index", repo_root) >= (int)sizeof(index_path)) {
        fprintf(stderr, MSG_UPDATE_INDEX_FAILED, index_path);
        return;
    }
    table = read_words_from_file(index_path, &num_lines, &num_words_per_line);
    make_timestamp(timestamp, sizeof(timestamp));
    if (!table) {
        create_new_index_file(index_path, rel_path, hash, timestamp);
        return;
    }
    idx = find_path_in_index(table, num_lines, num_words_per_line, rel_path);
    if (idx >= 0) {
        update_index_entry(table, idx, hash, timestamp);
    } else {
        ctx.ptable = &table;
        ctx.pnum_lines = &num_words_per_line;
        ctx.pnum_lines = &num_lines;
        add_new_index_entry(&ctx, rel_path, hash, timestamp);
    }
    save_index_table(index_path, table, num_lines, num_words_per_line);
}

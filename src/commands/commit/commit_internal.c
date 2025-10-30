/*
 * File: commit_internal.c
 * Description: Internal implementation functions for the 'commit' command in GOAT.
 *              Contains core commit logic separated into small, focused functions.
 *              Follows clean code principles with functions under 20 lines and 4 parameters.
 * Date: 30/10/2025
 * Author: Aliago
 */

#include "commands/commit/commit.h"
#include "core/index.h"
#include "core/hash.h"
#include "utils/fs.h"
#include "utils/repo.h"
#include "utils/timestamp.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_commit_message(cmd_opts_t *opts)
{
    opts->cmd_specific.commit.message = true;
}

void set_commit_help(cmd_opts_t *opts)
{
    opts->cmd_specific.commit.help = true;
}

static int check_repository_exists(void)
{
    char repo_path[512];

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        MSG_NO_REPO_EXISTS;
        return 1;
    }
    return 0;
}

static int check_index_has_content(void)
{
    char repo_path[512];
    char index_path[512];
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        return 1;
    }
    written = snprintf(index_path, sizeof(index_path), "%s/index", repo_path);
    if (written < 0 || (size_t)written >= sizeof(index_path)) {
        return 1;
    }
    if (!fs_file_exists(index_path)) {
        MSG_COMMIT_NOTHING_TO_COMMIT;
        return 2;
    }
    return 0;
}

int validate_commit_preconditions(void)
{
    int result = check_repository_exists();

    if (result != 0)
        return result;
    return check_index_has_content();
}

static FILE* open_index_file(char *index_path)
{
    char repo_path[512];
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        return NULL;
    }
    written = snprintf(index_path, 512, "%s/index", repo_path);
    if (written < 0 || written >= 512) {
        return NULL;
    }
    return fopen(index_path, "r");
}

static long get_file_size(FILE *file)
{
    long size;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}

static int read_file_content(FILE *file, long size, char **content)
{
    *content = malloc(size + 1);
    if (!*content) {
        return -1;
    }
    fread(*content, 1, size, file);
    (*content)[size] = '\0';
    return 0;
}

static int load_tree_content(char **tree_content)
{
    char index_path[512];
    FILE *file;
    long size;

    file = open_index_file(index_path);
    if (!file) return -1;
    size = get_file_size(file);
    if (size <= 0) {
        fclose(file);
        MSG_COMMIT_NOTHING_TO_COMMIT;
        return -2;
    }
    if (read_file_content(file, size, tree_content) < 0) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

static FILE* open_head_file(char *head_path)
{
    char repo_path[512];
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        return NULL;
    }
    written = snprintf(head_path, 512, "%s/HEAD", repo_path);
    if (written < 0 || written >= 512) {
        return NULL;
    }
    return fopen(head_path, "r");
}

static int get_main_ref_path(char *main_ref_path, size_t size)
{
    char repo_path[512];
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        return -1;
    }
    written = snprintf(main_ref_path, size, "%s/refs/heads/main", repo_path);
    if (written < 0 || (size_t)written >= size) {
        return -1;
    }
    return 0;
}

static int read_hash_from_file(FILE *file, char *parent_hash)
{
    if (fgets(parent_hash, 65, file)) {
        parent_hash[strcspn(parent_hash, "\n")] = '\0';
        return 1;
    }
    return 0;
}

static int read_branch_ref(char *parent_hash)
{
    char main_ref_path[512];
    FILE *file;

    if (get_main_ref_path(main_ref_path, sizeof(main_ref_path)) < 0) {
        parent_hash[0] = '\0';
        return 0;
    }
    file = fopen(main_ref_path, "r");
    if (!file) {
        parent_hash[0] = '\0';
        return 0;
    }
    if (read_hash_from_file(file, parent_hash)) {
        fclose(file);
        return 1;
    }
    fclose(file);
    parent_hash[0] = '\0';
    return 0;
}

static int extract_direct_hash(const char *line, char *parent_hash)
{
    strncpy(parent_hash, line, 64);
    parent_hash[strcspn(parent_hash, "\n")] = '\0';
    return 1;
}

static int process_head_line(const char *line, char *parent_hash)
{
    if (strncmp(line, "ref: refs/heads/main", 20) == 0) {
        return read_branch_ref(parent_hash);
    }
    if (strlen(line) >= 40) {
        return extract_direct_hash(line, parent_hash);
    }
    return 0;
}

static int load_parent_hash(char *parent_hash)
{
    char head_path[512];
    FILE *file;
    char line[512];

    file = open_head_file(head_path);
    if (!file) {
        parent_hash[0] = '\0';
        return 0;
    }
    if (fgets(line, sizeof(line), file)) {
        fclose(file);
        return process_head_line(line, parent_hash);
    }
    fclose(file);
    parent_hash[0] = '\0';
    return 0;
}

static int build_commit_content(commit_data_t *data, char *content)
{
    return snprintf(content, 4096,
        "tree:\n%s%sparent: %s\nauthor: hugo <user@goat>\ndate: %s\nmessage: %s\n",
        data->tree_content,
        data->tree_content[strlen(data->tree_content)-1] == '\n' ? "" : "\n",
        data->parent_hash,
        data->timestamp,
        data->message);
}

int prepare_commit_data(commit_data_t *data, const char *message)
{
    int result;

    data->message = (char*)message;
    result = load_tree_content(&data->tree_content);
    if (result < 0)
        return result;
    load_parent_hash(data->parent_hash);
    if (get_iso_timestamp(data->timestamp, sizeof(data->timestamp)) < 0) {
        free(data->tree_content);
        return -1;
    }
    return 0;
}

static int create_objects_directory(void)
{
    char repo_path[512];
    char objects_dir[512];
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path))) {
        return -1;
    }
    written = snprintf(objects_dir, sizeof(objects_dir), "%s/objects", repo_path);
    if (written < 0 || (size_t)written >= sizeof(objects_dir)) {
        return -1;
    }
    if (!fs_dir_exists(objects_dir)) {
        return fs_create_dir(objects_dir, 0755);
    }
    return 0;
}

static int write_commit_file(const commit_data_t *data, const char *content)
{
    char repo_path[512];
    char object_path[512];
    FILE *file;
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path)))
        return -1;
    written = snprintf(object_path, sizeof(object_path), "%s/objects/%s", repo_path, data->commit_hash);
    if (written < 0 || (size_t)written >= sizeof(object_path))
        return -1;
    file = fopen(object_path, "w");
    if (!file)
        return -1;
    fprintf(file, "%s", content);
    fclose(file);
    return 0;
}

int write_commit_object(const commit_data_t *data)
{
    char commit_content[4096];
    int result;

    result = build_commit_content((commit_data_t*)data, commit_content);
    if (result < 0)
        return result;
    if (hash_string_to_hex(commit_content, (char*)data->commit_hash) < 0)
        return -1;
    result = create_objects_directory();
    if (result < 0)
        return result;
    return write_commit_file(data, commit_content);
}

static int update_branch_reference(const commit_data_t *data)
{
    char repo_path[512];
    char main_ref_path[512];
    FILE *file;
    int written;

    if (!find_goat_repo(repo_path, sizeof(repo_path)))
        return -1;
    written = snprintf(main_ref_path, sizeof(main_ref_path), "%s/refs/heads/main", repo_path);
    if (written < 0 || (size_t)written >= sizeof(main_ref_path))
        return -1;
    file = fopen(main_ref_path, "w");
    if (!file)
        return -1;
    fprintf(file, "%s\n", data->commit_hash);
    fclose(file);
    return 0;
}

int update_head_reference(const commit_data_t *data)
{
    char repo_path[512];
    char head_path[512];
    FILE *file;
    int written;
    int result;

    if (!find_goat_repo(repo_path, sizeof(repo_path)))
        return -1;
    written = snprintf(head_path, sizeof(head_path), "%s/HEAD", repo_path);
    if (written < 0 || (size_t)written >= sizeof(head_path))
        return -1;
    result = update_branch_reference(data);
    if (result != 0)
        return result;
    file = fopen(head_path, "w");
    if (!file)
        return -1;
    fprintf(file, "ref: refs/heads/main\n");
    fclose(file);
    return 0;
}

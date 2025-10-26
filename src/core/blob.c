/*
 * File: blob.c
 * Description: Blob management implementation for GOAT object storage.
 *              Handles storing, retrieving, and managing file blobs in .goat/objects/.
 * Date: 19/10/2025
 * Author: Aliago
 */

#include "core/blob.h"
#include "core/hash.h"
#include "utils/fs.h"
#include "utils/repo.h"
#include "utils/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static int create_blob_path(const unsigned char *hash, char *object_dir, char *object_path)
{
    char hash_hex[65];
    char repo_root[PATH_MAX];
    int ret1;
    int ret2;

    hash_to_hex(hash, hash_hex);
    if (!find_goat_repo(repo_root, sizeof(repo_root))) {
        return -1;
    }
    ret1 = snprintf(object_dir, PATH_MAX, "%s/objects/%.2s", repo_root, hash_hex);
    ret2 = snprintf(object_path, PATH_MAX, "%s/%s", object_dir, hash_hex + 2);
    if (ret1 >= PATH_MAX || ret2 >= PATH_MAX) {
        return -1;
    }
    return 0;
}

static int copy_loop(FILE *src, FILE *dst, const char *object_path, unsigned char *buffer)
{
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, 8192, src)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dst) != bytes_read) {
            fclose(src);
            fclose(dst);
            unlink(object_path);
            return -1;
        }
    }
    return 0;
}

static int copy_file_to_blob(const char *filepath, const char *object_path)
{
    FILE *src, *dst;
    unsigned char buffer[8192];

    src = fopen(filepath, "rb");
    if (!src)
        return -1;
    dst = fopen(object_path, "wb");
    if (!dst) {
        fclose(src);
        return -1;
    }
    if (copy_loop(src, dst, object_path, buffer) != 0)
        return -1;
    fclose(src);
    fclose(dst);
    return 0;
}

int store_blob(const unsigned char *hash, const char *filepath)
{
    char object_dir[PATH_MAX];
    char object_path[PATH_MAX];

    if (create_blob_path(hash, object_dir, object_path) != 0) {
        return -1;
    }
    if (access(object_path, F_OK) == 0) {
        return 0;
    }
    if (mkdir_recursive(object_dir) != 0) {
        return -1;
    }
    return copy_file_to_blob(filepath, object_path);
}

int blob_exists(const unsigned char *hash)
{
    char object_dir[PATH_MAX];
    char object_path[PATH_MAX];

    if (create_blob_path(hash, object_dir, object_path) != 0) {
        return -1;
    }
    return (access(object_path, F_OK) == 0) ? 1 : 0;
}

int get_blob_path(const unsigned char *hash, char *path_buffer, size_t buffer_size)
{
    char object_dir[PATH_MAX];
    char object_path[PATH_MAX];

    if (create_blob_path(hash, object_dir, object_path) != 0) {
        return -1;
    }
    if (strlen(object_path) >= buffer_size) {
        return -1;
    }
    strcpy(path_buffer, object_path);
    return 0;
}

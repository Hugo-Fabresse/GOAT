/*
 * File: index.h
 * Description: Index management utilities for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef INDEX_H
#define INDEX_H

#include <openssl/sha.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

typedef struct index_content {
    const char *rel_path;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    const char *timestamp;
    mode_t mode;
    off_t size;
    time_t mtime;
    time_t ctime;
    dev_t device;
    ino_t inode;
    struct index_content *next;
} index_content_t;

void update_index(const index_content_t *content);
index_content_t* read_current_index(void);
index_content_t* find_index_entry(const index_content_t *index_list, const char *rel_path);
void free_index_content(index_content_t *content);

#endif // INDEX_H

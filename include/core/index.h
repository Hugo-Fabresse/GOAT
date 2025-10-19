/*
 * File: index.h
 * Description: Index management utilities for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef INDEX_H
#define INDEX_H

#include "core/hash.h"
#include <openssl/sha.h>

typedef struct index_content {
    const char *rel_path;
    unsigned char hash[HASH_HEX_SIZE];
    const char *timestamp;
    struct index_content *next;
} index_content_t;

void update_index(const index_content_t *content);

#endif // INDEX_H

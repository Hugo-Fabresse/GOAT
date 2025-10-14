/*
 * File: index.h
 * Description: Index management utilities for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef INDEX_H
#define INDEX_H

#include <openssl/sha.h>

typedef struct index_context {
    char ****ptable;
    int **pnum_words;
    int *pnum_lines;
} index_context_t;

void update_index(const char *rel_path, const unsigned char *hash);
void clean_index(const char *repo_root);

#endif // INDEX_H

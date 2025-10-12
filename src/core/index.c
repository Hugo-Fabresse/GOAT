/*
 * File: index.c
 * Description: Index management implementation for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/index.h"
#include "utils/repo.h"
#include <stdio.h>

void update_index(const char *rel_path, const unsigned char *hash)
{
    FILE *idx = fopen(INDEX_FILE, "a");

    if (!idx) {
        fprintf(stderr, "Error: Unable to update index file %s\n", INDEX_FILE);
        return;
    }
    fprintf(idx, "%s ", rel_path);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        fprintf(idx, "%02x", hash[i]);
    }
    fprintf(idx, "\n");
    fclose(idx);
}

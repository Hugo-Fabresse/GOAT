/*
 * File: index.c
 * Description: Index management implementation for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/index.h"
#include "utils/repo.h"
#include "utils/path.h"
#include <stdio.h>

void update_index(const char *rel_path, const unsigned char *hash)
{
    FILE *idx;
    char *index_path = find_abs_path(INDEX_FILE);

    if (!index_path) {
        return;
    }
    idx = fopen(index_path, "a");
    if (!idx) {
        MSG_UPDATE_INDEX_FAILED(index_path);
        return;
    }
    fprintf(idx, "%s ", rel_path);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        fprintf(idx, "%02x", hash[i]);
    }
    fprintf(idx, "\n");
    fclose(idx);
}

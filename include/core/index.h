/*
 * File: index.h
 * Description: Index management utilities for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef INDEX_H
#define INDEX_H

#include <openssl/sha.h>

void update_index(const char *rel_path, const unsigned char *hash);

#endif // INDEX_H


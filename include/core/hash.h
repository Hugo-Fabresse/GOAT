/*
 * File: hash.h
 * Description: Hashing utilities for GOAT (SHA-256).
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <openssl/sha.h>

int hash_file_sha256(const char *path, unsigned char *out_hash);

#endif // HASH_H


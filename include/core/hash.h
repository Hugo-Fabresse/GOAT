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

#define HASH_HEX_SIZE 65

int hash_file_sha256(const char *path, unsigned char *out_hash);
int hash_file_to_hex(const char *path, char *hex_output);
void hash_to_hex(const unsigned char *hash, char *hex_output);

#endif // HASH_H


/*
 * File: blob.h
 * Description: Blob management utilities for GOAT object storage.
 *              Handles storing, retrieving, and managing file blobs in .goat/objects/.
 * Date: 19/10/2025
 * Author: Aliago
 */

#ifndef GOAT_BLOB_H
#define GOAT_BLOB_H

#include <openssl/sha.h>

int store_blob(const unsigned char *hash, const char *filepath);
int blob_exists(const unsigned char *hash);
int get_blob_path(const unsigned char *hash, char *path_buffer, size_t buffer_size);

#endif // GOAT_BLOB_H

/*
 * File: hash.c
 * Description: SHA-256 file hashing implementation for GOAT.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/hash.h"
#include <openssl/evp.h>
#include <stdio.h>

static EVP_MD_CTX *init_sha256_ctx(void)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    if (ctx && EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        EVP_MD_CTX_free(ctx);
        return NULL;
    }
    return ctx;
}

static int update_hash(EVP_MD_CTX *ctx, FILE *f)
{
    unsigned char buf[4096];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (EVP_DigestUpdate(ctx, buf, n) != 1)
            return -1;
    }
    return 0;
}

int hash_file_sha256(const char *path, unsigned char *out)
{
    FILE *f;
    EVP_MD_CTX *ctx;
    int ok = 0;

    f = fopen(path, "rb");
    if (!f) {
        return -1;
    }
    ctx = init_sha256_ctx();
    if (!ctx) {
        fclose(f); return -1;
    }
    ok = update_hash(ctx, f);
    if (ok == 0) {
        ok = (EVP_DigestFinal_ex(ctx, out, NULL) == 1) ? 0 : -1;
    }
    EVP_MD_CTX_free(ctx);
    fclose(f);
    return ok;
}

void hash_to_hex(const unsigned char *hash, char *hex_output)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex_output + (i * 2), "%02x", hash[i]);
    }
    hex_output[HASH_HEX_SIZE - 1] = '\0';
}

int hash_file_to_hex(const char *path, char *hex_output)
{
    unsigned char hash_binary[SHA256_DIGEST_LENGTH];

    if (hash_file_sha256(path, hash_binary) != 0) {
        return -1;
    }
    hash_to_hex(hash_binary, hex_output);
    return 0;
}

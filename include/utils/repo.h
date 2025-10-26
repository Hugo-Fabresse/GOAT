/*
 * File: repo.h
 * Description: GOAT repository utility interface.
 *              Provides high-level functions for repository state and management.
 *              Contains repository structure constants and logic helpers for GOAT commands.
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef REPO_H
#define REPO_H

#include <stdbool.h>
#include <stddef.h>

// Repository structure constants
#define GOAT_DIR       ".goat"
#define OBJECTS_DIR    ".goat/objects"
#define REFS_DIR       ".goat/refs"
#define HEADS_DIR      ".goat/refs/heads"
#define HEAD_FILE      ".goat/HEAD"
#define CONFIG_FILE    ".goat/config"
#define INDEX_FILE     ".goat/index"

#define PATH_MAX 4096

bool check_already_initialized(void);
bool find_goat_repo(char *repo_path, size_t size);

#endif // REPO_H

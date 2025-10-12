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

// Repository structure constants
#define GOAT_DIR       ".goat"
#define OBJECTS_DIR    ".goat/objects"
#define REFS_DIR       ".goat/refs"
#define HEADS_DIR      ".goat/refs/heads"
#define HEAD_FILE      ".goat/HEAD"
#define CONFIG_FILE    ".goat/config"
#define INDEX_FILE     ".goat/index"

bool check_already_initialized(void);

#endif // REPO_H

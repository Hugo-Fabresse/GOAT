/*
 * File: repo.c
 * Description: GOAT repository utility implementation.
 *              Provides high-level functions for repository state and management.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "utils/repo.h"
#include "utils/fs.h"

bool check_already_initialized(void)
{
    return fs_dir_exists(GOAT_DIR);
}

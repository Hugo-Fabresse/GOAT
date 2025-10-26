/*
 * File: path.h
 * Description: Path handling interface for GOAT.
 *              Declares functions for resolving relative paths
 *              into absolute, repository-consistent paths.
 * Date: 11/10/2025
 * Author: Aliago
 */

#ifndef PATH_H
#define PATH_H

char *find_abs_path(const char *rel_path);

#endif // PATH_H

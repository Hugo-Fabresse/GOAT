/*
 * File: timestamp.h
 * Description: Timestamp utilities for file modification times and formatting.
 * Date: 16/10/2025
 * Author: Aliago
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stddef.h>

void format_timestamp(const char *timestamp_str, char *readable_time, size_t bufsize);
int get_file_timestamp(const char *path, char *timestamp_buffer, size_t bufsize);
int get_iso_timestamp(char *timestamp_buffer, size_t bufsize);

#endif // TIMESTAMP_H

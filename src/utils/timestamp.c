/*
 * File: timestamp.c
 * Description: Timestamp utilities for file modification times and formatting.
 * Date: 16/10/2025
 * Author: Aliago
 */

#include "utils/timestamp.h"
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void format_timestamp(const char *timestamp_str, char *readable_time, size_t bufsize)
{
    time_t timestamp;
    struct tm *tm_info;

    if (timestamp_str) {
        timestamp = (time_t)atol(timestamp_str);
        tm_info = localtime(&timestamp);
        strftime(readable_time, bufsize, "%Y-%m-%dT%H:%M:%S", tm_info);
    } else {
        strcpy(readable_time, "no_timestamp");
    }
}

int get_file_timestamp(const char *path, char *timestamp_buffer, size_t bufsize)
{
    struct stat st;

    if (stat(path, &st) != 0) {
        return -1;
    }
    snprintf(timestamp_buffer, bufsize, "%ld", st.st_mtime);
    return 0;
}

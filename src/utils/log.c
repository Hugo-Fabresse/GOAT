/*
 * File: log.c
 * Description: Logging utility functions for printing messages with different log levels.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "utils/log.h"
#include "ui/messages.h"
#include <stdio.h>
#include <stdarg.h>

void log_msg(log_level_t level, const char *fmt, ...)
{
    va_list args;
    FILE *out = stdout;

    va_start(args, fmt);
    switch(level) {
        case LOG_INFO:
            fprintf(out, "[INFO] ");
            vprintf(fmt, args);
            break;
        case LOG_WARN:
            fprintf(out, "[WARN] ");
            vprintf(fmt, args);
            break;
        case LOG_ERROR:
            out = stderr;
            fprintf(out, COLOR_RED "[ERROR] " COLOR_RESET);
            vfprintf(out, fmt, args);
            break;
        default:
            vprintf(fmt, args);
            break;
    }
    va_end(args);
}

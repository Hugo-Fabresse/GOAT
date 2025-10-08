/*
 * File: log.c
 * Description: ${DESCRIPTION}
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "utils/log.h"
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
            fprintf(out, "[ERROR] ");
            vfprintf(out, fmt, args);
            break;
        default:
            vprintf(fmt, args);
            break;
    }
    va_end(args);
}

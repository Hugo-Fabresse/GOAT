/*
 * File: log.h
 * Description: Simple logging utility for GOAT.
 *              Supports different log levels: INFO, WARN, ERROR.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef LOG_H
#define LOG_H

typedef enum {
    LOG_DEFAULT,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

void log_msg(log_level_t level, const char *fmt, ...);

#endif // LOG_H

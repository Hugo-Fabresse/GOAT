/*
 * File: messages.h
 * Description: Centralized colored messages for GOAT CLI, sorted by category.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include "utils/log.h"

// ANSI color codes
#define COLOR_RED    "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_BLUE   "\033[1;34m"
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_RESET  "\033[0m"

// ===== General Messages =====
#define MSG_USAGE(progname) \
    log_msg(LOG_DEFAULT, COLOR_BLUE "Usage: %s <command>\n" COLOR_RESET, progname); \
    log_msg(LOG_DEFAULT, COLOR_YELLOW "Hint: Available commands are: init\n" COLOR_RESET)
#define MSG_UNKNOWN_COMMAND(cmd) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Unknown command '%s'\n" COLOR_RESET, cmd)

// ===== Init Command Messages =====
#define MSG_INIT_USAGE \
    COLOR_BLUE "usage: goat init [<options>]\n" COLOR_RESET \
    "\n" \
    "    --force        Reinitialize and overwrite an existing repository\n" \
    "    --quiet        Suppress output messages\n"
#define MSG_UNKNOWN_OPTION(opt) \
    log_msg(LOG_DEFAULT, COLOR_RED "error: unknown option '%s'\n" COLOR_RESET, opt); \
    log_msg(LOG_DEFAULT, MSG_INIT_USAGE)
#define MSG_REPO_EXISTS log_msg(LOG_DEFAULT, COLOR_RED "Error: A GOAT repository already exists at ./.goat\n" COLOR_YELLOW "Hint: Use '--force' to reinitialize and overwrite the existing repository.\n" COLOR_RESET)
#define MSG_INIT_SUCCESS log_msg(LOG_DEFAULT, COLOR_GREEN "Initialized empty GOAT repository in ./.goat\n" COLOR_RESET)
#define MSG_INIT_FAILURE log_msg(LOG_DEFAULT, COLOR_RED "Error: Failed to initialize GOAT repository.\n" COLOR_RESET)

// ===== Filesystem Error Messages =====
#define MSG_CREATE_DIR_FAILED(path) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Failed to create directory '%s'\n" COLOR_YELLOW "Cause: Missing parent directories or insufficient permissions.\nHint: Ensure the parent directories exist or use a recursive mkdir_p().\n" COLOR_RESET, path)
#define MSG_CREATE_FILE_FAILED(path) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Failed to create file '%s'\n" COLOR_YELLOW "Cause: Insufficient write permissions or missing parent directory.\n" COLOR_RESET, path)

#endif // MESSAGES_H

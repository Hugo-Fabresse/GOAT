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
    log_msg(LOG_DEFAULT, COLOR_YELLOW "Hint: Available commands are: init - add - commit\n" COLOR_RESET)
#define MSG_UNKNOWN_COMMAND(cmd) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Unknown command '%s'\n" COLOR_RESET, cmd)

// ===== Init Command Messages =====
#define MSG_UNKNOWN_OPTION(opt) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Unknown option '%s'\n" COLOR_RESET, opt);
#define MSG_NO_OPTION_SPECIFIED(cmd) \
    log_msg( \
        LOG_DEFAULT, \
        COLOR_RED "Error: No options specified for '%s' command.\n" \
        COLOR_YELLOW "Hint: Use '--help' to see available options.\n" COLOR_RESET, cmd \
    )
#define MSG_REPO_EXISTS \
    log_msg( \
        LOG_DEFAULT, \
        COLOR_RED "Error: A GOAT repository already exists at ./.goat\n" \
        COLOR_YELLOW "Hint: Use '--force' to reinitialize and overwrite the existing repository.\n" COLOR_RESET \
    )
#define MSG_INIT_SUCCESS log_msg(LOG_DEFAULT, COLOR_GREEN "Initialized empty GOAT repository in ./.goat\n" COLOR_RESET)
#define MSG_INIT_FAILURE log_msg(LOG_DEFAULT, COLOR_RED "Error: Failed to initialize GOAT repository.\n" COLOR_RESET)

// ===== Command Usage Messages =====
#define MSG_COMMAND_USAGE_HEADER(cmd) \
    log_msg(LOG_DEFAULT, COLOR_BLUE "usage: goat %s [<options>]\n" COLOR_RESET "\n", cmd)
#define MSG_COMMAND_OPTION(option, description) \
    log_msg(LOG_DEFAULT, "    %-12s %s\n", option, description)

// ===== Add Command Messages =====
#define MSG_NO_REPO_EXISTS \
    log_msg( \
        LOG_DEFAULT, \
        COLOR_RED "Error: No GOAT repository found in the current directory.\n" \
        COLOR_YELLOW "Hint: Run 'goat init' first to initialize a repository.\n" COLOR_RESET \
    )
#define MSG_NO_REPO_FOUND \
    log_msg(LOG_ERROR, "No repository found.\n")

// ===== Status Command Messages =====
#define MSG_STATUS_USAGE \
    COLOR_BLUE "usage: goat status [<options>]\n" COLOR_RESET \
    "\n" \
    "    -s, --short    Show status in short format\n" \
    "    -v, --verbose  Show verbose output\n"

// Add operation headers
#define MSG_ADD_ALL_HEADER \
    log_msg(LOG_DEFAULT, COLOR_BLUE "Adding all files to staging area...\n" COLOR_RESET)
#define MSG_ADD_UPDATE_HEADER \
    log_msg(LOG_DEFAULT, COLOR_BLUE "Updating tracked files...\n" COLOR_RESET)

// File status messages
#define MSG_FILE_ADDED(file) \
    log_msg(LOG_DEFAULT, COLOR_GREEN "add" COLOR_RESET " '%s'\n", file)
#define MSG_FILE_MODIFIED(file) \
    log_msg(LOG_DEFAULT, COLOR_YELLOW "modify" COLOR_RESET " '%s'\n", file)
#define MSG_FILE_ACCESS_ERROR(file) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Unable to read '%s'\n" COLOR_RESET, file)
#define MSG_FILE_SKIPPED_PATH_TOO_LONG(repo, file) \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: path too long: %s/%s\n" COLOR_RESET, repo, file)
#define MSG_FILE_SKIPPED_NOT_FOUND(file) \
    log_msg(LOG_DEFAULT, COLOR_YELLOW "Skipped (not found): %s\n" COLOR_RESET, file)

// Summary messages
#define MSG_ADD_SUMMARY_SIMPLE(modified) \
    log_msg(LOG_DEFAULT, COLOR_GREEN "\nSummary: %d file(s) modified\n" COLOR_RESET, modified)
#define MSG_ADD_SUMMARY_WITH_ADDED(added, modified) \
    log_msg(LOG_DEFAULT, COLOR_GREEN "\nSummary: %d file(s) added, %d file(s) modified\n" COLOR_RESET, added, modified)
#define MSG_ADD_ERRORS(errors) \
    log_msg(LOG_DEFAULT, COLOR_RED "Errors: %d file(s) could not be processed\n" COLOR_RESET, errors)

// ===== Filesystem Error Messages =====
#define MSG_CREATE_DIR_FAILED(path) \
    log_msg( \
        LOG_DEFAULT, \
        COLOR_RED "Error: Failed to create directory '%s'\n" \
        COLOR_YELLOW "Cause: Missing parent directories or insufficient permissions.\n" \
                     "Hint: Ensure the parent directories exist or use a recursive mkdir_p().\n" COLOR_RESET, path \
    )
#define MSG_CREATE_FILE_FAILED(path) \
    log_msg( \
        LOG_DEFAULT, \
        COLOR_RED "Error: Failed to create file '%s'\n" \
        COLOR_YELLOW "Cause: Insufficient write permissions or missing parent directory.\n" COLOR_RESET, path \
    )

// ===== Index Messages =====
#define MSG_UPDATE_INDEX_FAILED(path) \
    log_msg(LOG_ERROR, COLOR_RED "Error: Unable to update index file %s\n" COLOR_RESET, path)
#define MSG_INDEX_OPEN_FAILED(path) \
    log_msg(LOG_ERROR, COLOR_RED "Error: Unable to open index file for writing: %s\n" COLOR_RESET, path)

// ===== Commit Command Messages =====
#define MSG_COMMIT_SUCCESS(hash, message) \
    log_msg(LOG_DEFAULT, COLOR_GREEN "Committed %s - \"%s\"\n" COLOR_RESET, hash, message)
#define MSG_COMMIT_NO_MESSAGE \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Missing commit message. Use -m \"message\"\n" COLOR_RESET)
#define MSG_COMMIT_NOTHING_TO_COMMIT \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Nothing to commit (index is empty)\n" COLOR_RESET)
#define MSG_COMMIT_FAILED \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Failed to create commit\n" COLOR_RESET)

// Dans messages.h, après les autres macros
#define MSG_NOT_A_REPO \
    log_msg(LOG_DEFAULT, COLOR_RED "Error: Not a GOAT repository\n" COLOR_YELLOW \
            "Hint: Run 'goat init' to initialize a repository\n" COLOR_RESET)

#endif // MESSAGES_H

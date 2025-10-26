/*
 * File: command.h
 * Description: Common types and structures for all GOAT commands.
 *              Defines the base command option structure and option parsing utilities.
 *              Provides a unified interface for command implementations.
 * Date: 08/10/2025
 * Author: Aliago
 */

#ifndef CORE_COMMAND_H
#define CORE_COMMAND_H

#include <stdbool.h>
#include <stddef.h>

// Base command options structure
// Each command can extend this with specific options
typedef struct cmd_opts {
    // Command-specific flags (union for memory efficiency)
    union {
        struct {
            bool force;
            bool quiet;
        } init;

        struct {
            bool all;
            bool update;
        } add;

    } cmd_specific;
} cmd_opts_t;

// Option entry for parsing
typedef struct option_entry {
    const char *name;
    const char *description;
    void (*set_option)(cmd_opts_t *);
} option_entry_t;

// Common structure for control options
typedef struct {
    const char *command_name;
    const option_entry_t *options;
    size_t num_options;
} command_options_t;

void print_command_usage(const command_options_t *cmd_opts);
int parse_options(int argc, char **argv, const command_options_t *cmd_opts, cmd_opts_t *opts);

#endif // CORE_COMMAND_H

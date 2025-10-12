/*
 * File: goat.h
 * Description: Public interface for the GOAT CLI dispatcher.
 *              Declares the goat() function for command routing and usage printing.
 *              Used by main.c and any CLI entry point.
 * Date: 12/10/2025
 * Author: Aliago
 */

#ifndef GOAT_H
#define GOAT_H

#include "core/command.h"

// Structure for a control input
typedef struct {
    const char *name;
    int (*parse_options)(int argc, char **argv, cmd_opts_t *opts);
    int (*execute)(const cmd_opts_t *opts);
} command_entry_t;

int goat(int argc, char **argv);

#endif // GOAT_H

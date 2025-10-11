/*
 * File: add_internal.c
 * Description: Implementation of the option handlers for the 'add' command in GOAT.
 *              Provides modular functions to set specific flags in the command options
 *              structure, such as 'all' and 'update', to allow flexible parsing of CLI arguments.
 *              Separating handlers into this file improves maintainability and clarity of the code.
 * Date: 11/10/2025
 * Author: Aliago
 */

#include "commands/add/add.h"

void set_all(cmd_opts_t *opts)
{
    opts->cmd_specific.add.all = true;
}

void set_update(cmd_opts_t *opts)
{
    opts->cmd_specific.add.update = true;
}

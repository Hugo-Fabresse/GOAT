/*
 * File: init_handlers.c
 * Description: Implementation of the option handlers for the 'init' command in GOAT.
 *              Provides modular functions to set specific flags in the command options
 *              structure, such as 'force' and 'quiet', to allow flexible parsing of CLI arguments.
 *              Separating handlers into this file improves maintainability and clarity of the code.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include "commands/init/init.h"

void set_init_force(cmd_opts_t *opts)
{
    opts->cmd_specific.init.force = true;
}

void set_init_quiet(cmd_opts_t *opts)
{
    opts->cmd_specific.init.quiet = true;
}

void set_init_help(cmd_opts_t *opts)
{
    opts->cmd_specific.init.help = true;
}


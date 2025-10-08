/*
 * File: main.c
 * Description: Entry point for the GOAT CLI.
 *              Parses command-line arguments and dispatches to the appropriate command.
 *              Uses the public interface of commands such as init.h.
 *              Sets up default command options and handles unknown commands.
 * Date: 08/10/2025
 * Author: Aliago
 */

#include <stdio.h>
#include <string.h>
#include "commands/init/init.h"

int main(int argc, char **argv)
{
    cmd_opts_t opts = {0, 0};

    if (argc < 2) {
        fprintf(stderr, "Usage: goat <command>\n");
        return 1;
    }
    if (strcmp(argv[1], "init") == 0) {
        return cmd_init(&opts);
    }
    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    return 1;
}

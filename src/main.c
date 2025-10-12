/*
 * File: main.c
 * Description: Entry point for the GOAT CLI application.
 *              Handles argument parsing and delegates to the core command dispatcher.
 *              This file should remain minimal and only call goat() from goat.c.
 * Date: 12/10/2025
 * Author: Aliago
 */

#include "core/goat.h"
#include <string.h>
#include <stddef.h>

int main(int argc, char **argv)
{
    return goat(argc, argv);
}

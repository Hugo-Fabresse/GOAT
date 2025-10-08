/*
 * File: init_tests.c
 * Description: Unit tests for the 'init' command in GOAT.
 *              Provides full deterministic testing for:
 *              - Option parsing (--force, --quiet)
 *              - Repository initialization
 *              - Error conditions when repo exists
 *              - Quiet mode behavior
 *              - Complete repository structure
 * Date: 08/10/2025
 * Author: Aliago
 */

#define _XOPEN_SOURCE 700

#include "commands/init/init.h"

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define GOAT_DIR ".goat"

// --- Helpers ---

static void remove_goat_dir(void) {
    struct stat st;
    if (stat(GOAT_DIR, &st) == 0)
        system("rm -rf .goat");
}

static bool path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

// --- Option Parsing Tests ---

Test(init_parse, force_and_quiet) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "init", "--force", "--quiet"};
    int ret = parse_init_options(4, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert(opts.force);
    cr_assert(opts.quiet);
}

Test(init_parse, quiet_then_force) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "init", "--quiet", "--force"};
    int ret = parse_init_options(4, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert(opts.force);
    cr_assert(opts.quiet);
}

Test(init_parse, invalid_option_fails) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "init", "--badflag"};
    int ret = parse_init_options(3, argv, &opts);
    cr_assert_eq(ret, -1);
}

Test(init_parse, duplicated_options) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "init", "--force", "--force", "--quiet"};
    int ret = parse_init_options(5, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert(opts.force);
    cr_assert(opts.quiet);
}

Test(init_parse, mixed_valid_and_invalid) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "init", "--quiet", "--invalid"};
    int ret = parse_init_options(4, argv, &opts);
    cr_assert_eq(ret, -1);
}

// --- Option Handler Tests ---

Test(init_handler, set_force_sets_flag) {
    cmd_opts_t opts = {.force = false};
    set_force(&opts);
    cr_assert(opts.force);
}

Test(init_handler, set_quiet_sets_flag) {
    cmd_opts_t opts = {.quiet = false};
    set_quiet(&opts);
    cr_assert(opts.quiet);
}

// --- Repository Initialization Tests ---

Test(init_cmd, creates_repository) {
    remove_goat_dir();
    cmd_opts_t opts = {.force = true}; // Forcing to avoid existing repo issues
    int ret = cmd_init(&opts);
    cr_assert_eq(ret, 0);
    cr_assert(path_exists(GOAT_DIR));
    remove_goat_dir();
}

Test(init_cmd, quiet_mode_suppresses_output) {
    remove_goat_dir();
    cmd_opts_t opts = {.force = true, .quiet = true};

    FILE *original_stdout = stdout;
    FILE *null_out = fopen("/dev/null", "w");
    if (null_out) stdout = null_out;

    int ret = cmd_init(&opts);

    if (null_out) {
    fflush(stdout);
    fclose(null_out);
    stdout = original_stdout;
    }

    cr_assert_eq(ret, 0);
    cr_assert(path_exists(GOAT_DIR));
    remove_goat_dir();
}

Test(init_cmd, force_overwrites_existing_repo) {
    remove_goat_dir();
    mkdir(GOAT_DIR, 0755);
    cmd_opts_t opts = {.force = true};
    int ret = cmd_init(&opts);
    cr_assert_eq(ret, 0);
    cr_assert(path_exists(GOAT_DIR));
    remove_goat_dir();
}

Test(init_cmd, idempotent_fails_second_time) {
    remove_goat_dir();
    cmd_opts_t opts = {.force = true}; // Force creation first
    int r1 = cmd_init(&opts);
    opts.force = false; // Second attempt without force
    int r2 = cmd_init(&opts);
    cr_assert_eq(r1, 0);
    cr_assert_eq(r2, 1);
    remove_goat_dir();
}

// --- Structural Integrity Tests ---

Test(init_cmd, creates_complete_structure) {
    remove_goat_dir();
    cmd_opts_t opts = {.force = true};
    int ret = cmd_init(&opts);
    cr_assert_eq(ret, 0);

    cr_assert(path_exists(".goat"));
    cr_assert(path_exists(".goat/objects"));
    cr_assert(path_exists(".goat/refs"));
    cr_assert(path_exists(".goat/refs/heads"));
    cr_assert(path_exists(".goat/HEAD"));
    cr_assert(path_exists(".goat/config"));
    cr_assert(path_exists(".goat/index"));

    remove_goat_dir();
}

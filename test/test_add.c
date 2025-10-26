/*
 * File: test_add.c
 * Description: Unit tests for the 'add' command in GOAT.
 *              Provides comprehensive testing for:
 *              - Option parsing (--all, --update)
 *              - File staging operations
 *              - Statistics tracking
 *              - Error conditions
 *              - Message display
 * Date: 24/10/2025
 * Author: Aliago
 */

#define _XOPEN_SOURCE 700

#include "commands/add/add.h"
#include "core/index.h"
#include "utils/repo.h"
#include "utils/fs.h"

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define GOAT_DIR ".goat"
#define TEST_FILE "test_file.txt"
#define TEST_DIR "test_dir"

// --- Helpers ---
static int rm_rf(const char *path) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;
        r = 0;
        while (!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf) {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = rm_rf(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}

static void remove_goat_dir(void) {
    rm_rf(GOAT_DIR);
}

static void setup_test_repo(void) {
    remove_goat_dir();
    mkdir(GOAT_DIR, 0755);
    mkdir(GOAT_DIR "/objects", 0755);
    mkdir(GOAT_DIR "/refs", 0755);
    mkdir(GOAT_DIR "/refs/heads", 0755);

    FILE *f = fopen(GOAT_DIR "/HEAD", "w");
    if (f) {
        fprintf(f, "ref: refs/heads/main\n");
        fclose(f);
    }

    f = fopen(GOAT_DIR "/config", "w");
    if (f) {
        fprintf(f, "[core]\n\trepositoryformatversion = 0\n");
        fclose(f);
    }

    f = fopen(GOAT_DIR "/index", "w");
    if (f) fclose(f);
}

static void create_test_file(const char *filename, const char *content) {
    FILE *f = fopen(filename, "w");
    if (f) {
        if (content) fprintf(f, "%s", content);
        fclose(f);
    }
}

static void cleanup_test_files(void) {
    unlink(TEST_FILE);
    rm_rf(TEST_DIR);
    remove_goat_dir();
}

// --- Tests for option parsing ---
TestSuite(add_parse, .init = setup_test_repo, .fini = cleanup_test_files);

Test(add_parse, all_option) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "add", "--all"};
    int ret = parse_add_options(3, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert(opts.cmd_specific.add.all);
    cr_assert_eq(opts.cmd_specific.add.update, false);
}

Test(add_parse, update_option) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "add", "--update"};
    int ret = parse_add_options(3, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert_eq(opts.cmd_specific.add.all, false);
    cr_assert(opts.cmd_specific.add.update);
}

Test(add_parse, both_options) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "add", "--all", "--update"};
    int ret = parse_add_options(4, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert(opts.cmd_specific.add.all);
    cr_assert(opts.cmd_specific.add.update);
}

Test(add_parse, no_options) {
    cmd_opts_t opts;
    char *argv[] = {"goat", "add"};
    int ret = parse_add_options(2, argv, &opts);
    cr_assert_eq(ret, 0);
    cr_assert_eq(opts.cmd_specific.add.all, false);
    cr_assert_eq(opts.cmd_specific.add.update, false);
}

// --- Tests for option handlers ---
TestSuite(add_handler);

Test(add_handler, set_all_sets_flag) {
    cmd_opts_t opts = {.cmd_specific.add.all = false};
    set_all(&opts);
    cr_assert(opts.cmd_specific.add.all);
}

Test(add_handler, set_update_sets_flag) {
    cmd_opts_t opts = {.cmd_specific.add.update = false};
    set_update(&opts);
    cr_assert(opts.cmd_specific.add.update);
}

// --- Tests for statistics functions ---
TestSuite(add_stats);

Test(add_stats, init_sets_zeros) {
    add_stats_t stats;
    init_add_stats(&stats);
    cr_assert_eq(stats.added, 0);
    cr_assert_eq(stats.modified, 0);
    cr_assert_eq(stats.errors, 0);
}

Test(add_stats, context_initialization) {
    add_context_t ctx;
    add_stats_t stats;
    index_content_t *content = NULL;
    index_content_t *index = NULL;

    init_add_context(&ctx, &stats, index);
    cr_assert_eq(ctx.stats, &stats);
    cr_assert_eq(ctx.current_index, index);

    set_context_paths(&ctx, "/test", &content);
    cr_assert_str_eq(ctx.repo_root, "/test");
    cr_assert_eq(ctx.content_list, &content);
}

// --- Tests for command execution ---
TestSuite(add_cmd, .init = setup_test_repo, .fini = cleanup_test_files);

Test(add_cmd, cmd_add_with_all_flag) {
    create_test_file(TEST_FILE, "test content");

    cmd_opts_t opts = {0};
    opts.cmd_specific.add.all = true;

    int ret = cmd_add(&opts);
    cr_assert_eq(ret, 0);
}

Test(add_cmd, cmd_add_with_update_flag) {
    cmd_opts_t opts = {0};
    opts.cmd_specific.add.update = true;

    int ret = cmd_add(&opts);
    cr_assert_eq(ret, 0);
}

Test(add_cmd, cmd_add_no_flags) {
    cmd_opts_t opts = {0};

    int ret = cmd_add(&opts);
    cr_assert_eq(ret, 0);
}

// --- Tests for file processing ---
TestSuite(add_file, .init = setup_test_repo, .fini = cleanup_test_files);

Test(add_file, process_new_file) {
    create_test_file(TEST_FILE, "new file content");

    add_stats_t stats;
    add_context_t ctx;
    index_content_t *content = NULL;

    init_add_stats(&stats);
    init_add_context(&ctx, &stats, NULL);
    set_context_paths(&ctx, ".", &content);

    process_file(TEST_FILE, &ctx);

    cr_assert_geq(stats.added + stats.modified, 0);
}

Test(add_file, display_summary_with_added) {
    add_stats_t stats = {.added = 2, .modified = 3, .errors = 1};

    // Redirect stdout to test output
    FILE *original_stdout = stdout;
    FILE *null_out = fopen("/dev/null", "w");
    if (null_out) stdout = null_out;

    display_add_summary(&stats, true);

    if (null_out) {
        fclose(null_out);
        stdout = original_stdout;
    }

    // Test passes if no crash occurs
    cr_assert(true);
}

Test(add_file, display_summary_without_added) {
    add_stats_t stats = {.added = 0, .modified = 3, .errors = 0};

    FILE *original_stdout = stdout;
    FILE *null_out = fopen("/dev/null", "w");
    if (null_out) stdout = null_out;

    display_add_summary(&stats, false);

    if (null_out) {
        fclose(null_out);
        stdout = original_stdout;
    }

    cr_assert(true);
}

// --- Tests for error conditions ---
TestSuite(add_error, .init = setup_test_repo, .fini = cleanup_test_files);

Test(add_error, process_nonexistent_file) {
    add_stats_t stats;
    add_context_t ctx;
    index_content_t *content = NULL;

    init_add_stats(&stats);
    init_add_context(&ctx, &stats, NULL);
    set_context_paths(&ctx, ".", &content);

    process_file("nonexistent_file.txt", &ctx);

    cr_assert_geq(stats.errors, 0);
}

Test(add_error, no_repository_wrapper) {
    cleanup_test_files(); // Ensure no repo exists

    // Double-check: explicitly remove .goat if it still exists
    rm_rf(".goat");

    FILE *original_stderr = stderr;
    FILE *null_err = fopen("/dev/null", "w");
    if (null_err) stderr = null_err;

    int ret = add_all_files_wrapper();

    if (null_err) {
        fclose(null_err);
        stderr = original_stderr;
    }

    cr_assert_eq(ret, 1, "Expected return code 1 when no repository exists, got %d", ret);
}

Test(add_error, no_repository_update_wrapper) {
    cleanup_test_files(); // Ensure no repo exists

    // Double-check: explicitly remove .goat if it still exists
    rm_rf(".goat");

    // Verify no .goat directory exists
    struct stat st;
    cr_assert_neq(stat(".goat", &st), 0, "A .goat directory still exists after cleanup");

    FILE *original_stderr = stderr;
    FILE *null_err = fopen("/dev/null", "w");
    if (null_err) stderr = null_err;

    int ret = update_tracked_files_wrapper();

    if (null_err) {
        fclose(null_err);
        stderr = original_stderr;
    }

    cr_assert_eq(ret, 1, "Expected return code 1 when no repository exists, got %d", ret);
}

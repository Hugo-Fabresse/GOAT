/*
 * File: file_utils.c
 * Description: Utility functions for reading files and splitting lines into word arrays.
 *              Each function has a single responsibility and is kept under 20 lines.
 * Date: 13/10/2025
 * Author: Aliago
 */

#include "utils/std_ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------ STRING HELPERS ------------------------------ */

static char *my_strdup(const char *s)
{
    size_t len;
    char *copy = "";

    if (!s)
        return NULL;
     len = strlen(s) + 1;
    copy = malloc(len);
    if (copy)
        memcpy(copy, s, len);
    return copy;
}

static size_t count_tokens(const char *line, const char *sep)
{
    char *tmp = my_strdup(line);
    size_t count = 0;
    char *tok = strtok(tmp, sep);

    while (tok) {
        count++;
        tok = strtok(NULL, sep);
    }
    free(tmp);
    return count;
}

static char **alloc_token_array(size_t count)
{
    char **res = malloc((count + 1) * sizeof(char *));

    if (!res)
        return NULL;
    res[count] = NULL;
    return res;
}

static void fill_token_array(char **res, const char *line, const char *sep)
{
    char *tmp = my_strdup(line);
    char *tok = strtok(tmp, sep);
    size_t i = 0;

    while (tok) {
        res[i++] = my_strdup(tok);
        tok = strtok(NULL, sep);
    }
    free(tmp);
}

/* ------------------------------ SPLIT LINE ---------------------------------- */

char **split_line_into_words(const char *line, const char *sep)
{
    size_t count;
    char **res;

    if (!line)
        return NULL;
    count = count_tokens(line, sep);
    if (count == 0)
        return NULL;
    res = alloc_token_array(count);
    if (!res)
        return NULL;
    fill_token_array(res, line, sep);
    return res;
}

/* ------------------------------ COUNT LINES -------------------------------- */

int count_lines(const char *path)
{
    FILE *f = fopen(path, "r");
    int c = 0;
    int count = 0;
    char last = '\n';

    if (!f)
        return -1;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n')
            count++;
        last = c;
    }
    fclose(f);
    return (last != '\n' && last != EOF) ? count + 1 : count;
}

/* ------------------------------ READ LINES --------------------------------- */

static char *read_line(FILE *f)
{
    char buf[1024];

    if (!fgets(buf, sizeof(buf), f))
        return NULL;
    buf[strcspn(buf, "\n")] = '\0';
    return my_strdup(buf);
}

static void fill_lines_array(char **lines, FILE *f, int n)
{
    for (int i = 0; i < n; i++)
        lines[i] = read_line(f);
    lines[n] = NULL;
}

char **read_lines_from_file(const char *path)
{
    FILE *f;
    int n = count_lines(path);
    char **lines;

    if (n <= 0)
        return NULL;
    f = fopen(path, "r");
    if (!f)
        return NULL;
    lines = malloc((n + 1) * sizeof(char *));
    if (!lines) {
        fclose(f);
        return NULL;
    }
    fill_lines_array(lines, f, n);
    fclose(f);
    return lines;
}

/* ------------------------------ FILE → WORD ARRAY --------------------------- */

static char ***alloc_word_table(int n, int **words_per_line)
{
    char ***array = malloc(n * sizeof(char **));

    *words_per_line = malloc(n * sizeof(int));
    if (!array || !*words_per_line) {
        free(array);
        free(*words_per_line);
        *words_per_line = NULL;
        return NULL;
    }
    return array;
}

static void fill_word_table(char ***array, char **lines, int n, int *words_per_line)
{
    int count = 0;

    for (int i = 0; i < n; i++) {
        array[i] = split_line_into_words(lines[i], " \t");
        count = 0;
        if (array[i]) {
            while (array[i][count])
                count++;
        }
        words_per_line[i] = count;
    }
}

static void free_lines_array(char **lines, int n)
{
    for (int i = 0; i < n; i++)
        free(lines[i]);
    free(lines);
}

static void free_word_table_on_error(char **lines, int n, char ***array, int *words_per_line)
{
    free_lines_array(lines, n);
    free(array);
    free(words_per_line);
}

char ***read_words_from_file(const char *path, int *num_lines, int **num_words_per_line)
{
    int n = count_lines(path);
    if (n <= 0) {
        *num_lines = 0;
        *num_words_per_line = NULL;
        return NULL;
    }

    char **lines = read_lines_from_file(path);
    if (!lines) {
        *num_lines = 0;
        *num_words_per_line = NULL;
        return NULL;
    }

    char ***array = alloc_word_table(n, num_words_per_line);
    if (!array) {
        free_word_table_on_error(lines, n, array, *num_words_per_line);
        *num_lines = 0;
        *num_words_per_line = NULL;
        return NULL;
    }

    fill_word_table(array, lines, n, *num_words_per_line);
    free_lines_array(lines, n);
    *num_lines = n;
    return array;
}

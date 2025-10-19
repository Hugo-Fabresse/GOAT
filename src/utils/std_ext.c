/*
 * File: std_ext.c
 * Description: ${DESCRIPTION}
 * Date: 14/10/2025
 * Author: Aliago
 */

#include "utils/std_ext.h"
#include <stdlib.h>
#include <string.h>

char *strdup(const char *s)
{
    size_t len = 0;
    char *copy = "";

    if (!s) {
        return NULL;
    }
    len = strlen(s) + 1;
    copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

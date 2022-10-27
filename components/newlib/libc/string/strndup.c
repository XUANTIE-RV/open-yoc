/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>

char *strndup(const char *str, size_t n)
{
    char *dup = NULL;

    dup = malloc(n + 1);
    if (dup) {
        memcpy(dup, str, n);
        dup[n] = '\0';
    }

    return dup;
}


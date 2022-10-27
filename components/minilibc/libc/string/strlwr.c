/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *strlwr(char *str)
{
    char *orig = str;

    for (; *str != '\0'; str++) {
        *str = tolower(*str);
    }

    return orig;
}

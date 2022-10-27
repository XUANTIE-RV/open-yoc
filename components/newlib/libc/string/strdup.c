/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>

char *strdup(const char *s)
{
    if (s != NULL) {
        char *ptr;
        int   len;

        len = strlen(s) + 1;
        ptr = (char *)malloc(len);

        if (ptr) {
            memcpy(ptr, s, len);
            return ptr;
        }
    }

    return NULL;
}

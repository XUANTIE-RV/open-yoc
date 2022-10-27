/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/kernel.h>
#include <aos/debug.h>


void *malloc(size_t size)
{
    return aos_malloc(size);
}

void free(void *ptr)
{
    if (ptr)
        aos_free(ptr);
}

void *realloc(void *ptr, size_t size)
{
    return aos_realloc(ptr, size);
}

void *calloc(size_t nmemb, size_t size)
{
    int   n   = size * nmemb;
    void *ptr = aos_malloc(n);

    if (ptr) {
        memset(ptr, 0, n);
    }

    return ptr;
}

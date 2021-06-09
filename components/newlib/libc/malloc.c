/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/kernel.h>
#include <aos/debug.h>

void *yoc_realloc(void *ptr, size_t size, void *caller);
void *yoc_malloc(int32_t size, void *caller);
void yoc_free(void *ptr, void *unsed);

void *malloc(size_t size)
{
    return yoc_malloc(size, __builtin_return_address(0));
}

void free(void *ptr)
{
    if (ptr)
        yoc_free(ptr, __builtin_return_address(0));
}

void *realloc(void *ptr, size_t size)
{
    return yoc_realloc(ptr, size, __builtin_return_address(0));
}

void *calloc(size_t nmemb, size_t size)
{
    int   n   = size * nmemb;
    void *ptr = yoc_malloc(n, __builtin_return_address(0));

    if (ptr) {
        memset(ptr, 0, n);
    }

    return ptr;
}

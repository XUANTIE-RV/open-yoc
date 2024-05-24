/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <csi_config.h>
#include <string.h>
#include <umm_heap.h>

#ifndef MALLOC_WEAK
#define MALLOC_WEAK __attribute__((weak))
#endif

MALLOC_WEAK void *malloc(size_t size)
{
    void *ret;

    ret = mm_malloc(USR_HEAP, size, __builtin_return_address(0));

    return ret;
}

MALLOC_WEAK void free(void *ptr)
{
    mm_free(USR_HEAP, ptr, __builtin_return_address(0));
}

MALLOC_WEAK void *realloc(void *ptr, size_t size)
{
    void *new_ptr;

    new_ptr = mm_malloc(USR_HEAP, size, __builtin_return_address(0));

    if (new_ptr == NULL) {
        return new_ptr;
    }

    if (ptr) {
        memcpy(new_ptr, ptr, size);

        mm_free(USR_HEAP, ptr, __builtin_return_address(0));
    }

    return new_ptr;
}

MALLOC_WEAK void *calloc(size_t nmemb, size_t size)
{
    void *ptr = NULL;

    ptr = mm_malloc(USR_HEAP, size * nmemb, __builtin_return_address(0));
    if (ptr) {
        memset(ptr, 0, size * nmemb);
    }

    return ptr;
}


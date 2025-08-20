 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdlib.h>
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

MALLOC_WEAK void *memalign(size_t alignment, size_t size)
{
    return NULL;
}


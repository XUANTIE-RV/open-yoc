/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

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

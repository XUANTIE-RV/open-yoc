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
#include <stdio.h>
#include <csi_core.h>
#include "app_main.h"

#if CONFIG_USE_FASTMEM
#include <umm_heap.h>
struct mm_heap_s g_fast_mmheap;
void fast_mm_init(void)
{
    mm_initialize(&g_fast_mmheap, (void *)CONFIG_FASTMEM_ADDR, (size_t)CONFIG_FASTMEM_SIZE);
}
#endif

void *fastmalloc(size_t size)
{
#if CONFIG_USE_FASTMEM
    void *ptr = mm_malloc(&g_fast_mmheap, size, __builtin_return_address(0));
    if (!ptr)
    {
        printf("oom size=%d\n", (int)size);
        while (1)
            ;
    }
    memset(ptr, 0, size);
    return ptr;
#else
    void *ptr = 0;
    // int iRet = posix_memalign(&ptr, 64, size);
    ptr = malloc(size);
    if (!ptr)
    {
        printf("oom size=%d\n", (int)size);
        while (1)
            ;
    }
    // printf("ptr=%p\n", ptr);
    return ptr;
#endif
}

void fastfree(void *ptr)
{
#if CONFIG_USE_FASTMEM
    mm_free(&g_fast_mmheap, ptr, __builtin_return_address(0));
#else
    free(ptr);
#endif
}

void *my_aligned_fastmalloc(size_t alignment, size_t size)
{
    void *original = fastmalloc(size + alignment - 1 + sizeof(void *));
    if (original == NULL)
    {
        return NULL;
    }

    uintptr_t orig_addr = (uintptr_t)original + sizeof(void *);
    uintptr_t aligned_addr =
        (orig_addr + alignment - 1) & ~(uintptr_t)(alignment - 1);

    ((void **)aligned_addr)[-1] = original;
    return (void *)aligned_addr;
}

void *align_fast_malloc(size_t size)
{
    return my_aligned_fastmalloc(128, size);
}

void align_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    void *original = ((void **)ptr)[-1];
    fastfree(original);
}

int example_ai_main(void)
{
#if CONFIG_USE_FASTMEM
    fast_mm_init();
#endif

    printf("===vlen=%d\n", csi_vlenb_get_value() * 8);

    printf("--------- add start ---------\n");
    add_main(0, NULL);
    printf("--------- add end ---------\n");

    printf("--------- mul start ---------\n");
    mul_main(0, NULL);
    printf("--------- mul end ---------\n");

    printf("--------- sub start ---------\n");
    sub_main(0, NULL);
    printf("--------- sub end ---------\n");

    printf("--------- exp start ---------\n");
    exp_main(0, NULL);
    printf("--------- exp end ---------\n");

    printf("--------- matmul start ---------\n");
    matmul_main(0, NULL);
    printf("--------- matmul end ---------\n");

    printf("--------- gather start ---------\n");
    gather_main(0, NULL);
    printf("--------- gather end ---------\n");

    printf("--------- slice start ---------\n");
    slice_main(0, NULL);
    printf("--------- slice end ---------\n");

    printf("--------- where start ---------\n");
    where_main(0, NULL);
    printf("--------- where end ---------\n");

    printf("--------- softmax start ---------\n");
    softmax_main(0, NULL);
    printf("--------- softmax end ---------\n");

    printf("--------- layer_norm start ---------\n");
    layer_norm_main(0, NULL);
    printf("--------- layer_norm end ---------\n");

    return 0;
}
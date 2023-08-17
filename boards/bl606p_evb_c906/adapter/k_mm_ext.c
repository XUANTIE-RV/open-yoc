/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"

extern size_t __heap2_start;
extern size_t __heap2_end;
static k_mm_head *g_kmm_head_sram;
void k_mm_init_sram(void)
{
    k_mm_region_t mm_region[] = {
    {(uint8_t *)&__heap2_start, (size_t)0},};
    mm_region[0].len = (size_t)(&__heap2_end) - (size_t)(&__heap2_start);
    /* init memory region */
    (void)krhino_init_mm_head(&g_kmm_head_sram, mm_region[0].start, mm_region[0].len);
}

static void *krhino_mm_alloc_sram(size_t size)
{
    void *tmp;
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    uint32_t app_malloc = size & AOS_UNSIGNED_INT_MSB;
    size = size & (~AOS_UNSIGNED_INT_MSB);
#endif
    if (size == 0) {
        printf("WARNING, malloc size = 0\r\n");
        return NULL;
    }
    tmp = k_mm_alloc(g_kmm_head_sram, size);
    if (tmp == NULL) {
#if (RHINO_CONFIG_MM_DEBUG > 0)
        static int32_t dumped;
        int32_t freesize;
        freesize = g_kmm_head_sram->free_size;
        printf("WARNING, malloc failed!!!! need size:%lu, but free size:%d\r\n", (unsigned long)size, freesize);
        if (dumped) {
            return tmp;
        }
        dumped = 1;
        debug_cpu_stop();
        kmm_error(KMM_ERROR_UNLOCKED);
#endif
    }
#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_mm_alloc_hook(tmp, size);
#endif
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    if (app_malloc == 0) {
        krhino_owner_return_addr(tmp);
    }
#endif
    return tmp;
}

static void krhino_mm_free_sram(void *ptr)
{
    k_mm_free(g_kmm_head_sram, ptr);
}

#if 1
void* bl_malloc(size_t size) 
{
    void *tmp = NULL;
    if (size == 0) {
        return NULL;
    }
    tmp = krhino_mm_alloc_sram(size);
    return tmp;
}

void bl_free(void* ptr) {
   krhino_mm_free_sram(ptr);
   return;
}
#endif

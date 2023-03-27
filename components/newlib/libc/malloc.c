/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/kernel.h>
#include <aos/debug.h>

#if (CONFIG_MEMALIGN_SUPPORT)
struct align_struct {
    int   used;
    void  *ptr;
};

static struct {
#define ALIGN_TOTAL_CNT_INIT (8)
    int                   total_cnt;
    int                   used_cnt;
    struct align_struct   *ptrs;
    aos_mutex_t           lock;
} g_align_cb;

#define align_is_init() (g_align_cb.total_cnt)
#define align_lock()    (aos_mutex_lock(&g_align_cb.lock, AOS_WAIT_FOREVER))
#define align_unlock()  (aos_mutex_unlock(&g_align_cb.lock))

void *memalign(size_t alignment, size_t size)
{
    void *rc = NULL;
    size_t old_size, new_size;

    if (!align_is_init()) {
        aos_mutex_new(&g_align_cb.lock);
        g_align_cb.ptrs      = aos_calloc(ALIGN_TOTAL_CNT_INIT, sizeof(struct align_struct) * ALIGN_TOTAL_CNT_INIT);
        g_align_cb.total_cnt = ALIGN_TOTAL_CNT_INIT;
    }

    align_lock();
    if (g_align_cb.used_cnt >= g_align_cb.total_cnt) {
        old_size = sizeof(struct align_struct) * g_align_cb.total_cnt;
        g_align_cb.total_cnt += ALIGN_TOTAL_CNT_INIT;
        new_size = sizeof(struct align_struct) * g_align_cb.total_cnt;
        g_align_cb.ptrs = aos_realloc((void*)g_align_cb.ptrs, new_size);
        memset((uint8_t*)g_align_cb.ptrs + old_size, 0, new_size - old_size);
    }

    rc = aos_malloc_align(alignment, size);
    if (rc) {
        for (int i = 0; i < g_align_cb.total_cnt; i++) {
            if (g_align_cb.ptrs[i].used == 0) {
                g_align_cb.used_cnt++;
                g_align_cb.ptrs[i].used = 1;
                g_align_cb.ptrs[i].ptr  = rc;
                break;
            }
        }
    }
    align_unlock();

    return rc;
}

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
    int rc = -1;
    void *ptr;

    ptr = memalign(alignment, size);
    if (ptr) {
        rc      = 0;
        *memptr = ptr;
    }

    return rc;
}
#endif

void *malloc(size_t size)
{
    return aos_malloc(size);
}

void free(void *ptr)
{
    if (ptr) {
#if (CONFIG_MEMALIGN_SUPPORT)
        if (align_is_init()) {
            align_lock();
            if (g_align_cb.used_cnt) {
                for (int i = 0; i < g_align_cb.total_cnt; i++) {
                    if (ptr == g_align_cb.ptrs[i].ptr) {
                        aos_free_align(ptr);
                        g_align_cb.used_cnt--;
                        g_align_cb.ptrs[i].used = 0;
                        g_align_cb.ptrs[i].ptr  = NULL;
                        align_unlock();
                        return;
                    }
                }
            }
            align_unlock();
        }
#endif
        aos_free(ptr);
    }
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




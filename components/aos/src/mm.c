/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc_config.h>

#include <k_api.h>
#include <aos/debug.h>
#include <aos/list.h>
#include <aos/kernel.h>

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_MM)

#define MAGIC_NUM 4

#define ISFIXEDBLK(mh, ptr)                                                                        \
    (mh->fixedmblk && ((void *)ptr > (void *)(mh->fixedmblk->mbinfo.buffer)) &&                    \
     ((void *)ptr < (void *)(mh->fixedmblk->mbinfo.buffer + mh->fixedmblk->size)))                 \
        ? 1                                                                                        \
        : 0

static int malloc_size(uint8_t *ret)
{
    if (g_kmm_head->fixedmblk != NULL && ISFIXEDBLK(g_kmm_head, ret))
        return DEF_FIX_BLK_SIZE - MAGIC_NUM;
    else {
        k_mm_list_t *b = (k_mm_list_t *)((char *)ret - MMLIST_HEAD_SIZE);
        return (b->size & RHINO_MM_BLKSIZE_MASK) - MAGIC_NUM;
    }
}

static int memory_overflow(uint8_t *q, uint32_t *caller)
{
    if (g_kmm_head->fixedmblk != NULL && ISFIXEDBLK(g_kmm_head, q)) {
        uint8_t *p = q + DEF_FIX_BLK_SIZE - MAGIC_NUM;
        return p[0] == 0x50 && p[1] == 0x50 && p[2] == 0x50 && p[3] == 0x50 ? 0 : 1;
    } else {
        k_mm_list_t *b = (k_mm_list_t *)((char *)q - MMLIST_HEAD_SIZE);

#if (RHINO_CONFIG_MM_DEBUG > 0u)
        if (caller)
            *caller = b->owner & 0xFFFFFFFE;
        if (b->dye != RHINO_MM_FREE_DYE && b->dye != RHINO_MM_CORRUPT_DYE) {
            return 1;
        }

        if (b->owner & 1) {
            uint8_t * p = q + (b->size & RHINO_MM_BLKSIZE_MASK) - MAGIC_NUM;
            return p[0] == 0x50 && p[1] == 0x50 && p[2] == 0x50 && p[3] == 0x50 ? 0 : 1;
        }
#else
        uint8_t * p = q + (b->size & RHINO_MM_BLKSIZE_MASK) - MAGIC_NUM;
        return p[0] == 0x50 && p[1] == 0x50 && p[2] == 0x50 && p[3] == 0x50 ? 0 : 1;
#endif
    }

    return 0;
}

struct mm_node {
    k_mm_list_t *b;
    int overflow;

    slist_t next;
};

static struct mem_list {
    uint32_t caller;
    uint32_t count;
    uint32_t size;
    slist_t node;
} * mmlist = NULL;
static int mm_count = 0;
static struct mm_node *node_list_head = NULL;

static void malloc_inc(struct mm_node *node)
{
    uint32_t caller = node->b->owner & 0xFFFFFFFE;
    uint32_t size = node->b->size & RHINO_MM_BLKSIZE_MASK;
    if (caller == 0)
        return;

    int found = 0;
    for (int i = 0; i < mm_count; i++) {
        if (mmlist[i].caller == caller) {
            mmlist[i].count++;
            mmlist[i].size += size;
            slist_add_tail(&node->next, &mmlist[i].node);

            // if (mmlist[i].count > 20) {
            //     printf("waring:  malloc caller: %x, total size: %d, count is %d\n", caller, mmlist[i].size, mmlist[i].count);
            // }
            found = 1;
            break;
        }
    }

retry:
    if (found == 0) {
        for (int i = 0; i < mm_count; i++) {
            if (mmlist[i].caller == 0) {
                mmlist[i].caller = caller;
                mmlist[i].count  = 1;
                mmlist[i].size   = size;
                slist_add_tail(&node->next, &mmlist[i].node);

                found = 1;
                break;
            }
        }
    }

    if (found == 0) {
        mmlist = krhino_mm_realloc(mmlist, sizeof(struct mem_list) * (mm_count + 8), NULL);

        memset(mmlist + mm_count, 0, sizeof(struct mem_list) * 8);
        mm_count += 8;
        goto retry;
    }
}

static void print_blockx(struct mm_node *node)
{
    k_mm_list_t *b = node->b;

    printf("         %p ", b);
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    printf( b->dye != RHINO_MM_FREE_DYE && b->dye != RHINO_MM_CORRUPT_DYE? "!" : " ");
#endif
    printf(b->size & RHINO_MM_FREE ? "free ": "used ");

    if ((b->size & RHINO_MM_BLKSIZE_MASK)) {
        printf(" %6lu ", (unsigned long) (b->size & RHINO_MM_BLKSIZE_MASK));
    } else {
        printf(" sentinel ");
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    printf(" %8x ", b->dye);
    printf(" 0x%-8x ", b->owner);
#endif

    printf("%s\r\n", node->overflow == 0 ? "" : "overflow");
}

static void dump_kmm_map(k_mm_head *mmhead)
{
    k_mm_region_info_t *reginfo, *nextreg;
    k_mm_list_t *next, *cur;

    if (!mmhead) {
        return;
    }

    int count = 0;

    reginfo = mmhead->regioninfo;
    while (reginfo) {
        cur = (k_mm_list_t *) ((char *) reginfo - MMLIST_HEAD_SIZE);
        while (cur) {
            count++;
            if ((cur->size & RHINO_MM_BLKSIZE_MASK)) {
                next = NEXT_MM_BLK(cur->mbinfo.buffer, cur->size & RHINO_MM_BLKSIZE_MASK);
            } else {
                next = NULL;
            }
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }

    node_list_head = krhino_mm_alloc(sizeof(struct mm_node) * count, (void*)0x12345678);
    memset(node_list_head, 0, sizeof(struct mm_node) * count);

    int i = 0;
    reginfo = mmhead->regioninfo;
    while (reginfo) {
        cur = (k_mm_list_t *) ((char *) reginfo - MMLIST_HEAD_SIZE);
        while (cur) {
            if (cur->owner != 0x12345678) {
                node_list_head[i].b = cur;
                node_list_head[i].overflow = 0;
                if (memory_overflow(cur->mbinfo.buffer, NULL) && i > 0)
                    node_list_head[i].overflow = 1;
                i++;
            }

            if ((cur->size & RHINO_MM_BLKSIZE_MASK)) {
                next = NEXT_MM_BLK(cur->mbinfo.buffer, cur->size & RHINO_MM_BLKSIZE_MASK);
            } else {
                next = NULL;
            }
            cur = next;
        }
        nextreg = reginfo->next;
        reginfo = nextreg;
    }

    for (i = 0; i < count; i++) {
        malloc_inc(&node_list_head[i]);
    }
}

void aos_malloc_show(int mm)
{
    aos_kernel_sched_suspend();
    dump_kmm_map(g_kmm_head);
    for (int i = 0; i < mm_count; i++) {
        if (mmlist[i].caller != 0) {
            struct mm_node *node;
            printf("%3d: caller=0x%x, count=%2d, total size=%d\n", i, mmlist[i].caller, mmlist[i].count, mmlist[i].size);

            if (mm) {
                slist_for_each_entry(&mmlist[i].node, node, struct mm_node, next) {
                    print_blockx(node);
                }
                printf("\r\n");
            }
        }
    }

    krhino_mm_free(mmlist);
    krhino_mm_free(node_list_head);

    node_list_head = NULL;
    mmlist = NULL;
    mm_count = 0;

    aos_kernel_sched_resume();
}

void *yoc_malloc(int32_t size, void *caller)
{
    aos_kernel_sched_suspend();
    uint8_t *ret = krhino_mm_alloc(size + MAGIC_NUM, (void*)((uint32_t)caller + 1));
    uint8_t *p   = ret + malloc_size(ret);

    *(p++) = 0x50;
    *(p++) = 0x50;
    *(p++) = 0x50;
    *(p++) = 0x50;

    aos_kernel_sched_resume();

    return ret;
}

void *yoc_realloc(void *ptr, size_t size, void *caller)
{
    aos_kernel_sched_suspend();
    ptr = krhino_mm_realloc(ptr, size + MAGIC_NUM, (void*)((uint32_t)caller + 1));

    uint8_t *p = (uint8_t*)ptr + malloc_size(ptr);

    *(p++) = 0x50;
    *(p++) = 0x50;
    *(p++) = 0x50;
    *(p++) = 0x50;

    aos_kernel_sched_resume();

    return ptr;
}

void yoc_free(void *ptr, void *unsed)
{
    aos_kernel_sched_suspend();

    uint32_t caller = 0;

    if (!memory_overflow(ptr, &caller)) {
        krhino_mm_free(ptr);
        aos_kernel_sched_resume();
        return;
    }

    printf("b->size = %d, ptr=%p\n", malloc_size(ptr), ptr);
    printf("WARNING, memory maybe corrupt!!, malloc function: 0x%x\n", caller);
    while (1);
}

#else

void *yoc_malloc(int32_t size, void *caller)
{
    aos_kernel_sched_suspend();
    void *ret = krhino_mm_alloc(size, caller);
    aos_kernel_sched_resume();

    return ret;
}

void *yoc_realloc(void *ptr, size_t size, void *caller)
{
    aos_kernel_sched_suspend();
    ptr = krhino_mm_realloc(ptr, size, caller);
    aos_kernel_sched_resume();

    return ptr;
}

void yoc_free(void *ptr, void *caller)
{
    aos_kernel_sched_suspend();
    krhino_mm_free(ptr);
    aos_kernel_sched_resume();
}

void aos_malloc_show(int mm)
{

}

#endif // defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_MM)

void *aos_zalloc(unsigned int size)
{
    void *ptr = yoc_malloc(size, __builtin_return_address(0));

    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}

void *aos_malloc(unsigned int size)
{
    return yoc_malloc(size, __builtin_return_address(0));
}

void *aos_calloc(unsigned int size, int num)
{
    return aos_zalloc(size * num);
}

void *aos_realloc(void *ptr, unsigned int size)
{
    return yoc_realloc(ptr, size, __builtin_return_address(0));
}


void *aos_zalloc_check(unsigned int size)
{
    void *ptr = yoc_malloc(size, __builtin_return_address(0));

    aos_check_mem(ptr);
    if (ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}

void *aos_malloc_check(unsigned int size)
{
    void *p = yoc_malloc(size, __builtin_return_address(0));
    aos_check_mem(p);

    return p;
}

void *aos_calloc_check(unsigned int size, int num)
{
    return aos_zalloc_check(size * num);
}

void *aos_realloc_check(void *ptr, unsigned int size)
{
    void *new_ptr = yoc_realloc(ptr, size, __builtin_return_address(0));
    aos_check_mem(new_ptr);

    return new_ptr;
}

void aos_alloc_trace(void *addr, size_t allocator)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    krhino_owner_attach(g_kmm_head, addr, allocator);
#endif
}

void aos_free(void *ptr)
{
    if (ptr)
        yoc_free(ptr, __builtin_return_address(0));
}

void aos_freep(char **ptr)
{
    if (ptr && (*ptr)) {
        aos_free(*ptr);
        *ptr = NULL;
    }
}



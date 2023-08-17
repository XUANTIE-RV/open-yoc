/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "excore/k_phymm.h"
//#include "debug_api.h"

typedef struct {
    size_t phymm_image_size;
    size_t phymm_heap_size;
    size_t phymm_dma_size;
} k_phymm_info_t;

typedef struct {
    size_t total;
    /* used_all = reserved + shared + used_detail[] */
    size_t used_all;
    size_t used_max;
    size_t reserved;
    size_t shared;
    /* used[0] is used by kernel */
    k_phymm_info_t used_detail[CONFIG_EXCORE_PROCESS_MAX];
} k_phymm_stat_t;

static k_phymm_stat_t phymm_info;

/* init pyhmm */
int k_phymm_init(unsigned long addr, size_t len)
{
    int32_t ret;

    memset(&phymm_info, 0, sizeof(phymm_info));

    ret = k_space_init(&g_space_info[PAS_DYN], addr, len);
    if (ret != 0) {
        printk("physical memory init error\r\n");
    }

    phymm_info.total = len;

    return ret;
}

/* The reserved memory will not be freed */
int k_phymm_reserve(unsigned long addr, size_t len)
{
    int32_t ret;

    CPSR_ALLOC();

    ret = k_space_occupy(&g_space_info[PAS_DYN], addr, len);
    if (ret != 0) {
        printk("physical memory reserve error\r\n");
    } else {
        RHINO_CPU_INTRPT_DISABLE();
        phymm_info.reserved += len;
        phymm_info.used_all += len;
        if (phymm_info.used_all > phymm_info.used_max) {
            phymm_info.used_max = phymm_info.used_all;
        }
        RHINO_CPU_INTRPT_ENABLE();
    }

    return ret;
}

unsigned long k_phymm_alloc(int pid, size_t len, size_t align, int reverse, kvmm_type_t type)
{
    unsigned long ret;
    CPSR_ALLOC();

    ret = k_space_acquire(&g_space_info[PAS_DYN], len, align, reverse);
    if (ret == K_ARCH_INVALID_ADDR) {
        printk("physical memory alloc error\r\n");
    } else {
        RHINO_CPU_INTRPT_DISABLE();
        phymm_info.used_all += len;

        if (phymm_info.used_all > phymm_info.used_max) {
            phymm_info.used_max = phymm_info.used_all;
        }

        if (pid < CONFIG_EXCORE_PROCESS_MAX) {
            switch (type) {
            case VMM_TYPE_APP_LOAD:
                phymm_info.used_detail[pid].phymm_image_size += len;
                break;
            case VMM_TYPE_MAP_SHARED:
                phymm_info.shared += len;
                break;
            case VMM_TYPE_MAP_HEAP:
                phymm_info.used_detail[pid].phymm_heap_size += len;
                break;
            case VMM_TYPE_MAP_DMA:
                phymm_info.used_detail[pid].phymm_dma_size += len;
                break;
            default:
                printk("%s mem type is not defined\r\n", __func__);
                break;
            }
        } else {
            printk("%s pid error\r\n", __func__);
        }
        RHINO_CPU_INTRPT_ENABLE();
    }

    return ret;
}

int k_phymm_free(int pid, unsigned long addr, size_t len, kvmm_type_t type)
{
    int32_t ret;
    CPSR_ALLOC();

    ret = k_space_release(&g_space_info[PAS_DYN], addr, len);
    if (ret != 0) {
        printk("physical memory release error %d\r\n", ret);
    } else {
        RHINO_CPU_INTRPT_DISABLE();

        phymm_info.used_all -= len;
        if (pid < CONFIG_EXCORE_PROCESS_MAX) {
            switch (type) {
            case VMM_TYPE_APP_LOAD:
                phymm_info.used_detail[pid].phymm_image_size -= len;
                break;
            case VMM_TYPE_MAP_SHARED:
                phymm_info.shared -= len;
                break;
            case VMM_TYPE_MAP_HEAP:
                phymm_info.used_detail[pid].phymm_heap_size -= len;
                break;
            case VMM_TYPE_MAP_DMA:
                phymm_info.used_detail[pid].phymm_dma_size -= len;
                break;
            default:
                printk("%s mem type is not defined\r\n", __func__);
                break;
            }
        } else {
            printk("%s pid error\r\n", __func__);
        }
        RHINO_CPU_INTRPT_ENABLE();
    }

    return ret;
}

size_t k_phymm_freesize(void)
{
    size_t free_size = phymm_info.total - phymm_info.used_all;
    return free_size;
}

void k_phymm_statis_show(int (*print_func)(const char *fmt, ...))
{
    uint32_t i;
    size_t   sum;

    if (print_func == NULL) {
        print_func = printk;
    }

    print_func("-----------------Physical Memory Statistics---------------\r\n");
    print_func("[PHYMM]  | TotalSize | UsedSize  | FreeSize  | MaxUsedSz |\r\n");
    print_func("%21d|%11d|%11d|%11d|\r\n", phymm_info.total, phymm_info.used_all,
               phymm_info.total - phymm_info.used_all, phymm_info.used_max);
    print_func("----------------------------------------------------------\r\n");

    print_func("[DETAIL] | SharedSize| ReservedSz|\r\n");
    print_func("%21d|%11d|\r\n", phymm_info.shared, phymm_info.reserved);

    print_func("----------------------------------------------------------\r\n");
    print_func("[DETAIL] | SumSize   | StaticSize| ExHeapSize| DMASize   |\r\n");

    for (i = 0; i < CONFIG_EXCORE_PROCESS_MAX; i++) {
        sum = phymm_info.used_detail[i].phymm_image_size + phymm_info.used_detail[i].phymm_heap_size
              + phymm_info.used_detail[i].phymm_dma_size;

        if (i == 0) {
            print_func("KERNEL%15d|%11c|%11d|%11d|\r\n", sum, '/',
                       phymm_info.used_detail[i].phymm_heap_size,
                       phymm_info.used_detail[i].phymm_dma_size);
        } else {
            if (sum != 0) {
                print_func("APP%d%17d|%11d|%11d|%11d|\r\n", i, sum,
                           phymm_info.used_detail[i].phymm_image_size,
                           phymm_info.used_detail[i].phymm_heap_size,
                           phymm_info.used_detail[i].phymm_dma_size);
            }
        }
    }
    print_func("----------------------------------------------------------\r\n");
}

void k_phymm_statis_get(k_phymm_statis_t *info)
{
    info->total   = phymm_info.total;
    info->used    = phymm_info.used_all;
    info->maxused = phymm_info.used_max;
}


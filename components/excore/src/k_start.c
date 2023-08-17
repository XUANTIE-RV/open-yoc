/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "excore/k_space.h"
#include "excore/k_cache.h"
#include "excore/k_mmu_sd.h"
//#include "bestat.h"

#if 0
#define VECTORS_BASE 0xFFFF0000

extern uint32_t _vector_table[];

/**
 * dynamic physical memory region,
 * defined in link script
 */
extern char *_phy_addr_dyn_start;
extern char *_phy_addr_dyn_len;

/**
 * kernel dynamic virtual address space region,
 * defined in link script
 */
extern char *_kernel_virt_addr_dyn_start;
extern char *_kernel_virt_addr_dyn_len;

/**
 * shared dynamic virtual address space region,
 * defined in link script
 */
extern char *_shared_virt_addr_dyn_start;
extern char *_shared_virt_addr_dyn_len;

/**
 * shared dynamic virtual address space region,
 * defined in link script
 */
extern char *_io_virt_addr_dyn_start;
extern char *_io_virt_addr_dyn_len;
static void k_vectable_set();

void k_addr_space_init()
{
    unsigned long addr;
    size_t    len;

    /* 1. init virtual address pool */
    addr = (unsigned long)&_kernel_virt_addr_dyn_start;
    len  = (size_t)&_kernel_virt_addr_dyn_len;

    k_space_init(&g_space_info[VAS_KERNEL], addr, len);

    /* 2. init shared virtual adress pool */
    addr = (unsigned long)&_shared_virt_addr_dyn_start;
    len  = (size_t)&_shared_virt_addr_dyn_len;

    k_space_init(&g_space_info[VAS_SHARED], addr, len);

    /* 3. init io virtual adress pool */
    addr = (unsigned long)&_io_virt_addr_dyn_start;
    len  = (size_t)&_io_virt_addr_dyn_len;

    k_space_init(&g_space_info[VAS_IO], addr, len);

    return;
}

static void k_vectable_set()
{
#if 0
    uint32_t reg;

    reg  = os_get_SCTLR();
    /*
    SCTLR.V, bit[13]   Vectors bit.
    0  Low exception vectors,
    1  High exception vectors (Hivecs), base address 0xFFFF0000.
    */
    reg |= 0x2000;
    os_set_SCTLR(reg);
#endif
}
#endif


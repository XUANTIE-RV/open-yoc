/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef K_MMU_H
#define K_MMU_H

#include "excore/k_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************* MMU configurations *************************/
/* Where to put mmu page tables
   It is recommended to place "mmu_tbl" in the place as the vaddr same with paddr */
#define K_CONFIG_MMU_TABLE_REGION           __attribute__((section(".bss.mmu_tbl")))
#define K_MMU_PAGE_ALIGN                    0x1000

/******************** mmu_mem_attr_t for app: ***********************/
typedef enum {
    /********** mem_attr for MMIO (e.g. regs, dma ...) **********/
    /* Strongly-ordered MMIO
       Non-Executable + Full access + Non-cacheable */
    MMU_ATTR_SO,
    /********** mem_attr for Normal memory (e.g. ddr, sram ...) **********/
    /* Normal memory for data / bss
       Non-Executable + Read-Write + Cache write back */
    MMU_ATTR_DATA,
    /* Normal memory for read-only data
       Non-Executable + Read-only + Cache write back */
    MMU_ATTR_DATA_RO,

    /*  memory for More combinations, add below */
    MMU_ATTR_BUTT
} mmu_mem_attr_t;

typedef enum {
    /* Executable, Readable, Writable */
    PGT_PROT_ALL,
    /* Executable, Readable */
    PGT_PROT_EXE,
    /* Readable, Writable */
    PGT_PROT_WR,
    /* Readable */
    PGT_PROT_RO,
    /* no cache */
    PGT_PROT_NOCACHE

    /* More type */
} kpgt_prot_t;

typedef struct {
    unsigned long vaddr;
    unsigned long paddr;
    unsigned long len;
    mmu_mem_attr_t mem_attr;
} k_mmu_regions_t;

/**
 * @brief  create the page table by memmaps & enable mmu
 * @return 0/-1
 */
int k_mmu_init(k_mmu_regions_t memmaps[], int memmaps_size);

/**
 * @brief  configure the page table with the address and size to establish a mapping
 * @param  [in] vaddr
 * @param  [in] paddr
 * @param  [in] len
 * @param  [in] mem_attr
 * @return 0/-1
 */
int k_mmap(unsigned long vaddr,
           unsigned long paddr,
           size_t    len,
           mmu_mem_attr_t  mem_attr);

/**
 * @brief  Configure page table unmapping with the address and size
 * @param  [in] vaddr
 * @param  [in] len
 * @return 0/-1
 */
int k_munmap(unsigned long vaddr, size_t len);

#if 0
unsigned long k_mmu_va2pa(unsigned long vaddr);

void k_mmu_table_init(void);
void k_mmu_disable(void);
void k_mmu_enable(void);


void k_mmu_show(void);
void k_addr_space_init(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* K_MMU_H */

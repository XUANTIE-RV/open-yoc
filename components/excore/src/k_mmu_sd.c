/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include <csi_core.h>
#include "excore/k_asid.h"
#include "excore/k_cache.h"
#include "excore/k_mmu_sd.h"

#define GET_SHIFT_BY_LEVEL(level) (12 + 9 * (3 - level))

static unsigned long g_tlb_addr;
static unsigned long g_tlb_size;
static unsigned long g_tlb_fillptr;
static K_CONFIG_MMU_TABLE_REGION unsigned long g_mmu_page_table[MAX_PTE_ENTRIES * MMU_PAGE_TABLE_NUM] __attribute__((aligned(4096)));

const unsigned long g_mmu_page_attr_map[] = {
    /* MMU_ATTR_SO */
    (UPPER_ATTRS(ATTR_SO | ATTR_SH) | LOWER_ATTRS(DIRTY_FLAG | ACCESS_FLAG | AP_X | AP_W | AP_R | GLOBAL_FLAG)) | 0x1,
    /* MMU_ATTR_DATA */
    (UPPER_ATTRS(ATTR_CA | ATTR_BU | ATTR_SH) | LOWER_ATTRS(DIRTY_FLAG | ACCESS_FLAG | AP_X | AP_W | AP_R | GLOBAL_FLAG)) | 0x1,
    /* MMU_ATTR_DATA_RO */
    (UPPER_ATTRS(ATTR_CA | ATTR_BU | ATTR_SH) | LOWER_ATTRS(DIRTY_FLAG | ACCESS_FLAG | AP_X | AP_W | AP_R | GLOBAL_FLAG)) | 0x1,
};

static unsigned long *_find_pte(unsigned long addr, int level)
{
    int i;
    int start_level = 1;
    unsigned long *pte;
    unsigned long idx;

    if (level < start_level)
        return NULL;

    pte = (unsigned long *)g_tlb_addr;
    for (i = start_level; i < 4; i++) {
        idx = (addr >> GET_SHIFT_BY_LEVEL(i)) & 0x1FF;
        pte += idx;

        if (i == level)
            return pte;
        if (*pte == 0)
            return NULL;
        /* goto the next level */
        pte = (unsigned long *)(PFN_PHYS(PTE_PFN(*pte)) & 0x1ffffffffffffULL);
    }

    return NULL;
}

static unsigned long *_create_page_table(void)
{
    unsigned long *new_table = (unsigned long *)g_tlb_fillptr;
    unsigned long pt_len = MAX_PTE_ENTRIES * sizeof(unsigned long);

    g_tlb_fillptr += pt_len;
    if (g_tlb_fillptr - g_tlb_addr > g_tlb_size) {
        printk("may be need increase g_tlb_size = 0x%lx!!!\n", g_tlb_size);
        while(1);
    }

    return new_table;
}

static void _set_pte_table(unsigned long *pte, unsigned long *table)
{
    *pte = PFN_PTE(PHYS_PFN(table), 1);
}

int k_mmu_mmap(unsigned long vaddr, unsigned long paddr, size_t len,
               mmu_mem_attr_t mem_attr, int asid, int32_t privileged)
{
    int level;
    CPSR_ALLOC();
    unsigned long attr;
    unsigned long *pte;
    unsigned long blocksize;
    unsigned long *new_table;

    CHECK_PARAM(len && mem_attr < MMU_ATTR_BUTT && asid < CONFIG_EXCORE_PROCESS_MAX, -1);
    CHECK_PARAM(vaddr % PAGE_SIZE == 0, -1);
#if (CONFIG_EXCORE_PROCESS_MAX > 1)
    CHECK_PARAM((vaddr % PAGE_SIZE) == (paddr % PAGE_SIZE), -1);
#else
    CHECK_PARAM(vaddr == paddr, -1);
#endif
    len = OS_ALIGN_UP(len, PAGE_SIZE),
    attr = g_mmu_page_attr_map[mem_attr];

    RHINO_CPU_INTRPT_DISABLE();
    while (len) {
        for (level = 1; level < 4; level++) {
            pte = _find_pte(vaddr, level);
            if (!pte) {
                printk("pte not found\n");
                return -1;
            }

            blocksize = 1ULL << GET_SHIFT_BY_LEVEL(level);
            if (len >= blocksize && !(vaddr & (blocksize - 1))) {
                /* create block PTE */
                *pte = PFN_PTE(PHYS_PFN(paddr), attr);
                csi_dcache_clean_range((uint64_t*)OS_ALIGN_DOWN((unsigned long)pte, 64UL), 64UL);
                vaddr += blocksize;
                paddr += blocksize;
                len   -= blocksize;
                break;
            } else if (*pte == 0) {
                /* create subpages */
                new_table = _create_page_table();
                _set_pte_table(pte, new_table);
                csi_dcache_clean_range((uint64_t*)OS_ALIGN_DOWN((unsigned long)pte, 64UL), 64UL);
            } else {
                /* match next level by the vaddr */
                continue;
            }
        }
    }
    RHINO_CPU_INTRPT_ENABLE();
    csi_mmu_invalid_tlb_all();

    return 0;
}

void local_flush_tlb_page(unsigned long addr)
{
    __DSB();
    __ISB();
    asm volatile("sfence.vma %0" : : "r" (addr) : "memory");
    __DSB();
    __ISB();
}

int k_mmu_munmap(unsigned long vaddr, size_t len, int asid)
{
    int level;
    size_t size;
    CPSR_ALLOC();
    unsigned long *pte;
    unsigned long blocksize;

    CHECK_PARAM(len && asid < CONFIG_EXCORE_PROCESS_MAX, -1);
    CHECK_PARAM(vaddr % PAGE_SIZE == 0, -1);
    size = OS_ALIGN_UP(len, PAGE_SIZE);
    RHINO_CPU_INTRPT_DISABLE();
    while (size) {
        for (level = 1; level < 4; level++) {
            pte = _find_pte(vaddr, level);
            if (!pte) {
                printk("pte not found\n");
                return -1;
            }

            blocksize = 1ULL << GET_SHIFT_BY_LEVEL(level);
            if (size >= blocksize && !(vaddr & (blocksize - 1))) {
                *pte   = 0;
                vaddr += blocksize;
                size  -= blocksize;
                csi_dcache_clean_range((uint64_t*)OS_ALIGN_DOWN((unsigned long)pte, 64UL), 64UL);
                break;
            } else {
                /* match next level by the vaddr */
                continue;
            }
        }
    }
    RHINO_CPU_INTRPT_ENABLE();
    csi_mmu_invalid_tlb_all();

    return 0;
}

/**
 * @brief  configure the page table with the address and size to establish a mapping
 * @param  [in] vaddr
 * @param  [in] paddr
 * @param  [in] len
 * @param  [in] mem_attr
 * @return 0/-1
 */
int k_mmap(unsigned long vaddr, unsigned long paddr, size_t len, mmu_mem_attr_t mem_attr)
{
    return k_mmu_mmap(vaddr, paddr, len, mem_attr, k_asid_get(), (k_asid_get() == OS_ASID_KERNEL) ? 1 : 0);
}

/**
 * @brief  Configure page table unmapping with the address and size
 * @param  [in] vaddr
 * @param  [in] len
 * @return 0/-1
 */
int k_munmap(unsigned long vaddr, size_t len)
{
    return k_mmu_munmap(vaddr, len, OS_ASID_KERNEL);
}

/**
 * @brief  create the page table by memmaps & enable mmu
 * @return 0/-1
 */
int k_mmu_init(k_mmu_regions_t memmaps[], int memmaps_size)
{
    int idx;

    CHECK_PARAM(memmaps && memmaps_size > 0, -1);
    g_tlb_addr    = (unsigned long)g_mmu_page_table;
    g_tlb_size    = sizeof(g_mmu_page_table);
    g_tlb_fillptr = g_tlb_addr;

    csi_mmu_disable();
    _create_page_table();

    for(idx = 0; idx < memmaps_size; idx ++) {
        k_mmap(memmaps[idx].vaddr, memmaps[idx].paddr, memmaps[idx].len, memmaps[idx].mem_attr);
    }
    //csi_dcache_clean_range((uint64_t*)&g_mmu_page_table, sizeof(g_mmu_page_table));
    csi_mmu_invalid_tlb_all();
    __set_SATP(((unsigned long)&g_mmu_page_table >> 12));
    csi_mmu_set_mode(MMU_MODE_39);
    csi_mmu_enable();
    //csi_dcache_invalid();
    //csi_icache_invalid();
    return 0;
}


#if 0
void k_mmap_show(void)
{
    int32_t ret;
    int asid;

    asid = k_asid_get();

    printk("[mmu show] SCTLR = 0x%x, ASID = %d, PA of TableBase = %p, VA of TableBase = %p\n",
           os_get_SCTLR(), asid, (void*)os_mmu_pgtbl_get(),
           (void*)g_pgtbl_lvl1[asid]);

    ret = os_mmu_traversal(asid, 0, os_mmu_print);

    if (ret != 0) {
        printk("[mmu show] return %d", ret);
    }
}

unsigned long k_mmap_va2pa(int asid, unsigned long vaddr)
{
    pgtbl_entry_t  lvl1_entry;
    pgtbl_entry_t *lvl2_pgtbl;
    pgtbl_entry_t  lvl2_entry;
    unsigned long  paddr;
    int32_t    lvl2_idx;

    lvl1_entry = g_pgtbl_lvl1[asid][vaddr >> MMU_SECTION_BITS];

    if ((lvl1_entry & 0x3) == DES_SEC_BASE) {
        paddr = (lvl1_entry >> MMU_SECTION_BITS) << MMU_SECTION_BITS;
        paddr = paddr | (vaddr & ((1 << MMU_SECTION_BITS) - 1));
        return paddr;
    } else if ((lvl1_entry & 0x3) == DES_PGTBL_BASE) {
        lvl2_pgtbl = (pgtbl_entry_t *)((lvl1_entry >> MMU_PAGETABLE_BITS) << MMU_PAGETABLE_BITS);
        lvl2_idx   = (vaddr >> MMU_SMALLPAGE_BITS) & (MMU_PAGE_ENTRY_CNT - 1);
        lvl2_entry = lvl2_pgtbl[lvl2_idx];

        if ((lvl2_entry & 0x3) == DES_LPG_BASE) {
            paddr = (lvl2_entry >> MMU_LARGEPAGE_BITS) << MMU_LARGEPAGE_BITS;
            paddr = paddr | (vaddr & ((1 << MMU_LARGEPAGE_BITS) - 1));
            return paddr;
        } else if ((lvl2_entry & 0x3) == DES_SPG_BASE) {
            paddr = (lvl2_entry >> MMU_SMALLPAGE_BITS) << MMU_SMALLPAGE_BITS;
            paddr = paddr | (vaddr & ((1 << MMU_SMALLPAGE_BITS) - 1));
            return paddr;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

unsigned long k_mmu_va2pa(unsigned long vaddr)
{
    /* use kernel mmu page to translate */
    return k_mmap_va2pa(OS_ASID_KERNEL, vaddr);
}

unsigned long k_va2pa(unsigned long vaddr)
{
    /* use kernel mmu page to translate */
    return k_mmap_va2pa(OS_ASID_KERNEL, vaddr);
}
#endif



/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "excore/k_vmm.h"
#include "excore/k_phymm.h"
#if (CONFIG_EXCORE_PROCESS_MAX > 1)
#include "task_group.h"
#endif
//#include "debug_api.h"

void *k_vmm_alloc(int pid, char *name, void *addr, size_t len, kvmm_prot_t prot, kvmm_type_t type)
{
    kspace_info_t *info;
    int reverse;
    unsigned long   vaddr;
#if CONFIG_EXCORE_RISCV_SUPPORT
    int ret;
    unsigned long   paddr;
    kpgt_prot_t pgt_type;
#endif
    unsigned long   paddr_once;
    unsigned long   vaddr_once;
    size_t      len_once;
    /* 'kvmm_type_t' map to 'kpgt_prot_t' */
    kpgt_prot_t type_mapping[] = {PGT_PROT_ALL,     /* VMM_PROT_ALL */
                                  PGT_PROT_EXE,     /* VMM_PROT_EXE */
                                  PGT_PROT_WR,      /* VMM_PROT_WR */
                                  PGT_PROT_RO,      /* VMM_PROT_RO */
                                  PGT_PROT_NOCACHE  /* VMM_PROT_NOCACHE */
                                 };

    if (prot >= sizeof(type_mapping)/sizeof(type_mapping[0])) {
        printk("%s prot is not valid\r\n", __func__);
        return NULL;
    }

#if CONFIG_EXCORE_RISCV_SUPPORT
    /* 1. Get 'pgt_type' for k_pgt_add */
    pgt_type = type_mapping[prot];
#endif

    /* 2. Get virtual address space as 'vaddr' */
    if ((type & VMM_TYPE_MAP_SHARED) == VMM_TYPE_MAP_SHARED) {
        info = &g_space_info[VAS_SHARED];
    } else {
        if ((pid == OS_ASID_KERNEL) || (type == VMM_TYPE_APP_LOAD)) {
            info = &g_space_info[VAS_KERNEL];
        } else {
#if (CONFIG_EXCORE_PROCESS_MAX > 1)
            info = &(task_group_get_by_pid(pid)->virt_addr_dyn_space);
#endif
        }
    }

    if (type & VMM_FLAG_VA_FIXED) {
        vaddr = (unsigned long)addr;
        /* The operation should be done outside the function */
        //k_space_occupy(info, vaddr, len);
    } else {
        reverse = (type & VMM_FLAG_VA_REV) ? 1 : 0;
        vaddr = k_space_acquire(info, len, K_MMU_PAGE_ALIGN, reverse);
        if (vaddr == K_ARCH_INVALID_ADDR) {
            printk("%s acquire virtual mem err\r\n", __func__);
            return NULL;
        }
    }

#if CONFIG_EXCORE_RISCV_SUPPORT
    if (type & VMM_FLAG_PA_ONE_PIECE) {
        /* 3.1 Get continuous physical address space as 'paddr' */
        reverse = ( type & VMM_FLAG_PA_REV ) ? 1 : 0;
        paddr = k_phymm_alloc(pid, len, K_MMU_PAGE_ALIGN, reverse, type);
        if ( paddr == K_ARCH_INVALID_ADDR ) {
            k_space_release(info, vaddr, len);
            printk("%s acquire continuous physical mem err\r\n", __func__);
            return NULL;
        }

        /* 4.  do k_pgt_add */
        if ((type & VMM_TYPE_MAP_SHARED) == VMM_TYPE_MAP_SHARED) {
            ret = k_pgt_add_shared(pid, (unsigned long)vaddr, (unsigned long)paddr, len, pgt_type);
        } else {
            ret = k_pgt_add(pid, (unsigned long)vaddr, (unsigned long)paddr, len, pgt_type);
        }

        if (ret != 0) {
            k_space_release(info, vaddr, len);
            k_phymm_free(pid,  paddr, len, type);
            printk("%s mapping err\r\n", __func__);
            return NULL;
        }

        return (void *)vaddr;
    }
#endif

    /* 3.2 Get Discontinuous physical address space as 'paddr' */
    if (len > k_phymm_freesize()) {
        k_space_release(info, vaddr, len);
        printk("%s physical memory is not enough, len 0x%x, free 0x%x\r\n",
               __func__, len, k_phymm_freesize());
        return NULL;
    }

    /* Must succeed now */
    for (vaddr_once = vaddr; vaddr_once < vaddr + len; vaddr_once += len_once) {
        len_once   = 0;
        paddr_once = 0;

        /* try small page */
        if ( len_once == 0
             && vaddr_once % MMU_SMALLPAGE_SIZE == 0
             && vaddr_once + MMU_SMALLPAGE_SIZE <= vaddr + len) {
            paddr_once = k_phymm_alloc(pid, MMU_SMALLPAGE_SIZE, K_MMU_PAGE_ALIGN, 0, type);
            if ( paddr_once != K_ARCH_INVALID_ADDR ) {
                len_once = MMU_SMALLPAGE_SIZE;
            }
        }

        if (len_once == 0) {
            printk("%s acquire discontinuous physical memory err\r\n", __func__);
            /* Never run to here */
            return NULL;
        }

        OS_PRINT_DBG("%s: pid %d map v 0x%x-0x%x -> p 0x%x-0x%x, size 0x%x\r\n",
                     __func__, pid, vaddr_once, vaddr_once + len_once,
                     paddr_once, paddr_once + len_once, len_once);

#if CONFIG_EXCORE_RISCV_SUPPORT
        /* 4.  do k_pgt_add */
        if ((type & VMM_TYPE_MAP_SHARED) == VMM_TYPE_MAP_SHARED) {
            ret = k_pgt_add_shared(pid, vaddr_once, paddr_once, len_once, pgt_type);
        } else {
            ret = k_pgt_add(pid, vaddr_once, paddr_once, len_once, pgt_type);
        }
        if (ret != 0) {
            OS_PRINT_ERR("%s: k_pgt_add failed, ret %d\r\n", __func__, ret);
            printk("%s mapping err\r\n", __func__);
            return NULL;
        }
#endif
    }

    return (void *)vaddr;
}

int k_vmm_free(int pid, char *name, void *addr, size_t len, kvmm_type_t type)
{
    int32_t        ret;
    kspace_info_t *info;
    unsigned long      paddr = (unsigned long)NULL;
    unsigned long      vaddr_page_cur;
    unsigned long      paddr_blk_start;
    unsigned long      vaddr_blk_start;
    unsigned long      paddr_page_cur;
    unsigned long      paddr_page_pre;

    if (((unsigned long)addr % MMU_SMALLPAGE_SIZE != 0) || (len % MMU_SMALLPAGE_SIZE != 0)) {
        printk("%s: addr and len not aligned to PAGE\r\n", __func__);
        return -1;
    }

    if ((type & VMM_TYPE_MAP_SHARED) == VMM_TYPE_MAP_SHARED) {
        info = &g_space_info[VAS_SHARED];
    } else if (pid == OS_ASID_KERNEL) {
        info = &g_space_info[VAS_KERNEL];
    } else {
#if (CONFIG_EXCORE_PROCESS_MAX > 1)
        info = &(task_group_get_by_pid(pid)->virt_addr_dyn_space);
#endif
    }

    k_dcache_clean_invalidate((unsigned long)addr, len);

    paddr = k_mmu_va2pa((unsigned long)addr);

    /* free physical mem */
    paddr_blk_start = paddr;
    paddr_page_pre  = paddr;
    vaddr_blk_start = (unsigned long)addr;
    vaddr_page_cur  = (unsigned long)addr + MMU_SMALLPAGE_SIZE;
    for (; vaddr_page_cur < (unsigned long)addr + len; vaddr_page_cur += MMU_SMALLPAGE_SIZE) {
        paddr_page_cur = k_mmu_va2pa((unsigned long)vaddr_page_cur);
        if (paddr_page_cur != paddr_page_pre + MMU_SMALLPAGE_SIZE) {
            ret = k_phymm_free(pid, (unsigned long)paddr_blk_start, vaddr_page_cur - vaddr_blk_start, type);
            if (ret != 0) {
                printk("%s: phy addr release failed, ret %d\r\n", __func__, ret);
                return -1;
            }

#if CONFIG_EXCORE_RISCV_SUPPORT
            /* Remove the mapping */
            if ((type & VMM_TYPE_MAP_SHARED) == VMM_TYPE_MAP_SHARED) {
                ret = k_pgt_remove_shared(pid, (unsigned long)vaddr_blk_start,
                                          vaddr_page_cur - vaddr_blk_start);
            } else {
                ret = k_pgt_remove(pid, (unsigned long)vaddr_blk_start,
                                   vaddr_page_cur - vaddr_blk_start);
            }

            if (ret != 0) {
                printk("%s: k_pgt_remove failed, ret %d\r\n", __func__, ret);
                return -1;
            }
#endif
            vaddr_blk_start = vaddr_page_cur;
            paddr_blk_start = paddr_page_cur;
        }

        paddr_page_pre = paddr_page_cur;
    }

    ret = k_phymm_free(pid, (unsigned long)paddr_blk_start, vaddr_page_cur - vaddr_blk_start, type);
    if (ret != 0) {
        printk("%s: phy addr release failed, ret %d\r\n", __func__, ret);
        return -1;
    }

#if CONFIG_EXCORE_RISCV_SUPPORT
    /* Remove the mapping */
    if ((type & VMM_TYPE_MAP_SHARED) == VMM_TYPE_MAP_SHARED) {
        ret = k_pgt_remove_shared(pid, (unsigned long)vaddr_blk_start, vaddr_page_cur - vaddr_blk_start);
    } else {
        ret = k_pgt_remove(pid, (unsigned long)vaddr_blk_start, vaddr_page_cur - vaddr_blk_start);
    }
    if (ret != 0) {
        printk("%s: k_pgt_remove failed, ret %d\r\n", __func__, ret);
        return -1;
    }
#endif

    /* free virtual address space */
    ret = k_space_release(info, (unsigned long)addr, len);
    if (ret != 0) {
        printk("%s: virt addr release failed, ret %d\r\n", __func__, ret);
        return -1;
    }

    return 0;
}

void *k_vmm_iomap(void *vaddr, void *paddr, size_t len, kvmm_prot_t prot, kvmm_type_t type)
{
    int       ret;
    uint32_t  offset = 0;

    kpgt_prot_t pgt_type;

    /* 'kvmm_type_t' map to 'kpgt_prot_t' */
    kpgt_prot_t type_mapping[] = {PGT_PROT_ALL,     /* VMM_PROT_ALL */
                                  PGT_PROT_EXE,     /* VMM_PROT_EXE */
                                  PGT_PROT_WR,      /* VMM_PROT_WR */
                                  PGT_PROT_RO,      /* VMM_PROT_RO */
                                  PGT_PROT_NOCACHE  /* VMM_PROT_NOCACHE */
                                 };

    if (len == 0) {
        return NULL;
    }

    /* 1. Get 'pgt_type' for k_pgt_add */
    pgt_type = type_mapping[prot];

    if (type & VMM_FLAG_VA_FIXED) {
        if (((unsigned long)vaddr % K_MMU_PAGE_ALIGN) != ((unsigned long)paddr % K_MMU_PAGE_ALIGN)) {
            return NULL;
        }
        ret = k_space_occupy(&g_space_info[VAS_IO], (unsigned long)vaddr, len);
        if (ret != 0) {
            return NULL;
        }
    } else {
        offset = (unsigned long)paddr & (K_MMU_PAGE_ALIGN -1);
        paddr  = (void *)((unsigned long)paddr & ~(K_MMU_PAGE_ALIGN-1));
        len    = OS_ALIGN_UP(len + offset, MMU_SMALLPAGE_SIZE);

        vaddr = (void *)k_space_acquire(&g_space_info[VAS_IO], len, K_MMU_PAGE_ALIGN, 0);
        if ((unsigned long)vaddr == K_ARCH_INVALID_ADDR) {
            return NULL;
        }
    }

    /* 2.  do k_pgt_add */
    ret = k_pgt_add(OS_ASID_KERNEL, (unsigned long)vaddr, (unsigned long)paddr, len, pgt_type);
    if (ret != 0) {
        k_space_release(&g_space_info[VAS_KERNEL], (unsigned long)vaddr, len);
        return NULL;
    }

    return (void *)(vaddr + offset);
}



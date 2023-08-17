/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "excore/k_shmm.h"

#if 0
//TODO:
static klist_t g_map_head = {&g_map_head, &g_map_head};

static k_shmm_map_t* map_info_find_by_pa(unsigned long pa)
{
    klist_t      *q;
    klist_t      *list_start;
    klist_t      *list_end;
    k_shmm_map_t *map_ptr;

    if ((void *)pa == NULL) {
        return NULL;
    }

    list_start = &g_map_head;
    list_end   = &g_map_head;

    for (q = list_start->next; q != list_end; q = q->next) {
        map_ptr = krhino_list_entry(q, k_shmm_map_t, map_list);
        if ((map_ptr->paddr) == pa) {
            return map_ptr;
        } else if ((map_ptr->paddr) > pa) {
            break;
        }
    }
    return NULL;
}

static k_shmm_map_t* map_info_find_by_va(unsigned long va)
{
    klist_t      *q;
    klist_t      *list_start;
    klist_t      *list_end;
    k_shmm_map_t *map_ptr;

    if ((void *)va == NULL) {
        return NULL;
    }

    list_start = &g_map_head;
    list_end   = &g_map_head;

    for (q = list_start->next; q != list_end; q = q->next) {
        map_ptr = krhino_list_entry(q, k_shmm_map_t, map_list);
        if ((map_ptr->vaddr) == va) {
            return map_ptr;
        } else if ((map_ptr->vaddr) > va) {
            break;
        }
    }
    return NULL;
}

static void map_info_delete(k_shmm_map_t *node)
{
    if (node == NULL) {
        return;
    }

    klist_rm(&node->map_list);
    return;
}

static void map_info_add(k_shmm_map_t *node)
{
    unsigned long    vaddr;
    klist_t      *q;
    klist_t      *list_start;
    klist_t      *list_end;
    k_shmm_map_t *map_ptr;

    if (node == NULL) {
        return;
    }

    list_start = &g_map_head;
    list_end   = &g_map_head;

    vaddr = node->vaddr;

    for (q = list_start->next; q != list_end; q = q->next) {
        map_ptr = krhino_list_entry(q, k_shmm_map_t, map_list);
        if ((map_ptr->vaddr) > vaddr) {
            break;
        }
    }

    klist_insert(q, &node->map_list);

    return;
}

void map_info_show()
{
    klist_t      *q;
    klist_t      *list_start;
    klist_t      *list_end;
    k_shmm_map_t *map_ptr;

    list_start = &g_map_head;
    list_end   = &g_map_head;

    printk("shared map info:\r\n");
    for (q = list_start->next; q != list_end; q = q->next) {
        map_ptr = krhino_list_entry(q, k_shmm_map_t, map_list);
        printk("vaddr %08p, paddr %08p, len 0x%x, type 0x%x, appmapbit 0x%x\r\n",
               map_ptr->vaddr, map_ptr->paddr, map_ptr->len, map_ptr->type, map_ptr->app_mapped);
    }

    return;
}

int k_shmm_map_init(int pid)
{
    int          ret;
    klist_t      *q;
    klist_t      *list_start;
    klist_t      *list_end;
    k_shmm_map_t *map_ptr;
    unsigned long    vaddr;
    unsigned long    paddr;
    size_t       len;
    size_t       len_continue;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    list_start = &g_map_head;
    list_end   = &g_map_head;

    for (q = list_start->next; q != list_end; q = q->next) {
        map_ptr = krhino_list_entry(q, k_shmm_map_t, map_list);

        vaddr = (unsigned long)map_ptr->vaddr;
        len   = map_ptr->len;

        if (map_ptr->type == VMM_TYPE_MAP_IO) {
            paddr = k_mmu_va2pa((unsigned long)vaddr);
            ret   = k_pgt_mmap(pid, (unsigned long)vaddr, (unsigned long)paddr, len, VMM_PROT_ALL, 1);
            if (ret != 0) {
                printk("%s: k_pgt_mmap failed, ret %d\r\n", __func__, ret);
                RHINO_CPU_INTRPT_ENABLE();
                return -1;
            }
        } else {
            while (len > 0) {
                len_continue = k_continue_phymem_len(vaddr, len);
                paddr        = k_mmu_va2pa((unsigned long)vaddr);

                ret = k_pgt_mmap(pid, (unsigned long)vaddr, (unsigned long)paddr, len_continue, VMM_PROT_ALL, 1);
                if (ret != 0) {
                    printk("%s: k_pgt_mmap failed, ret %d\r\n", __func__, ret);
                    RHINO_CPU_INTRPT_ENABLE();
                    return -1;
                }
                vaddr     += len_continue;
                len       -= len_continue;
            }
        }
    }

    RHINO_CPU_INTRPT_ENABLE();
    return 0;
}

/* FIXME: manage the shared region in process manager to unmap
 * the region during process exiting
 */

void *k_shmm_map_area(void *vaddr, void *paddr, size_t len, kvmm_prot_t prot, kvmm_type_t type)
{
#if 1
    return NULL;
#else
    int           ret;
    uint32_t      cur_pid;
    kpgt_prot_t   pgt_type;
    uint32_t      offset = 0;
    k_shmm_map_t *map_ptr;
    uint32_t      is_ram       = 0;
    uint32_t      is_first_map = 0;
    unsigned long     vaddr_cur;
    size_t        len_continue;

    /* 'kvmm_type_t' map to 'kpgt_prot_t' */
    kpgt_prot_t type_mapping[] = {PGT_PROT_ALL,     /* VMM_PROT_ALL */
                                  PGT_PROT_EXE,     /* VMM_PROT_EXE */
                                  PGT_PROT_WR,      /* VMM_PROT_WR */
                                  PGT_PROT_RO,      /* VMM_PROT_RO */
                                  PGT_PROT_NOCACHE  /* VMM_PROT_NOCACHE */
                                 };

    if (prot >= sizeof(type_mapping)/sizeof(type_mapping[0])) {
        return NULL;
    }

    /* 1. Get 'pgt_type' for k_pgt_add */
    pgt_type = type_mapping[prot];
    cur_pid  = krhino_cur_task_get()->pid;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    if (type == VMM_TYPE_MAP_IO) {
        if (paddr == NULL) {
            RHINO_CPU_INTRPT_ENABLE();
            return NULL;
        }

        offset = (unsigned long)paddr & (K_MMU_PAGE_ALIGN -1);
        paddr  = (void *)((unsigned long)paddr & ~(K_MMU_PAGE_ALIGN-1));
        len    = OS_ALIGN_UP(len + offset, MMU_SMALLPAGE_SIZE);

        map_ptr = map_info_find_by_pa((unsigned long)paddr);
    } else {
        is_ram = 1;
        map_ptr = map_info_find_by_va((unsigned long)vaddr);
    }

    if (map_ptr == NULL) {
        is_first_map = 1;
    } else {
        vaddr = (void *)map_ptr->vaddr;
    }

    if (is_first_map) {
        vaddr = (void *)k_space_acquire(&g_space_info[VAS_SHARED], len, MMU_SMALLPAGE_SIZE, 0);
        if ((unsigned long)vaddr == K_ARCH_INVALID_ADDR) {
            printk("%s %d: acurire shared virtual address failed\r\n", __func__, __LINE__);
            RHINO_CPU_INTRPT_ENABLE();
            return NULL;
        }

        map_ptr = krhino_mm_alloc(sizeof(k_shmm_map_t));
        if (map_ptr == NULL) {
            RHINO_CPU_INTRPT_ENABLE();
            return NULL;
        }

        memset(map_ptr, 0, sizeof(k_shmm_map_t));
        map_ptr->vaddr = (unsigned long)vaddr;
        if (type == VMM_TYPE_MAP_IO) {
            map_ptr->paddr = (unsigned long)paddr;
        }
        map_ptr->len   = len;
        map_ptr->type  = type;
        map_info_add(map_ptr);
    }

    map_ptr->app_mapped |= (0x1 << cur_pid);

    RHINO_CPU_INTRPT_ENABLE();

    if (is_first_map && is_ram) {
        vaddr = k_vmm_alloc(cur_pid, NULL, vaddr, len, prot, type | VMM_FLAG_VA_FIXED);
        printk(" VMM_FLAG_PA_FIXED, cur_pid = %d ,prot = %d,type=%d \n ", cur_pid, (int)prot, (int)type);
        return vaddr;
    } else {
        if (is_first_map) {
            ret = k_pgt_add_shared(cur_pid, (unsigned long)vaddr, (unsigned long)paddr, len, pgt_type);
            if (ret != 0) {
                printk("%s: k_pgt_add_shared failed, ret %d\r\n", __func__, ret);
                return NULL;
            }
        } else {
            if (is_ram) {
                vaddr_cur = (unsigned long)vaddr;
                while (len > 0) {
                    len_continue = k_continue_phymem_len(vaddr_cur, len);
                    paddr        = (void *)k_mmu_va2pa((unsigned long)vaddr_cur);

                    ret = k_pgt_mmap(cur_pid, (unsigned long)vaddr_cur, (unsigned long)paddr, len_continue, pgt_type, 0);
                    if (ret != 0) {
                        printk("%s: k_pgt_mmap failed, ret %d\r\n", __func__, ret);
                        return NULL;
                    }

                    vaddr_cur += len_continue;
                    len       -= len_continue;
                }
            } else {
                ret = k_pgt_mmap(cur_pid, (unsigned long)vaddr, (unsigned long)paddr, len, pgt_type, 0);
                if (ret != 0) {
                    printk("%s: k_pgt_mmap failed, ret %d\r\n", __func__, ret);
                    return NULL;
                }
            }
        }
        return (vaddr + offset);
    }
#endif
}

int k_shmm_unmap_area(void *start, size_t len)
{
#if 1
    return 0;
#else
    int32_t       ret;
    uint32_t      cur_pid;
    k_shmm_map_t *map_ptr;
    uint32_t      is_ram      = 0;
    uint32_t      is_last_map = 0;

    cur_pid = krhino_cur_task_get()->pid;
    len = OS_ALIGN_UP(len, K_MMU_PAGE_ALIGN);

    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();
    map_ptr = map_info_find_by_va((unsigned long)start);
    if ((map_ptr == NULL) || ((map_ptr->app_mapped & (0x1 << cur_pid)) == 0)) {
        RHINO_CPU_INTRPT_ENABLE();
        return -1;
    }

    if (map_ptr->type != VMM_TYPE_MAP_IO) {
        is_ram = 1;
    }

    map_ptr->app_mapped &= ~(0x1 << cur_pid);

    if (map_ptr->app_mapped == 0) {
        is_last_map = 1;
        map_info_delete(map_ptr);
    }
    RHINO_CPU_INTRPT_ENABLE();

    if (is_last_map && is_ram) {
        /* free Physical memory and virtual address space */
        k_vmm_free(cur_pid, NULL, start, len, VMM_TYPE_MAP_SHARED);
    } else {
        if (is_last_map) {
            ret = k_pgt_remove_shared(cur_pid, (unsigned long)start, len);
            if (ret != 0) {
                printk("%s: k_pgt_munmap failed, ret %d\r\n", __func__, ret);
                return -1;
            }

            ret = k_space_release(&g_space_info[VAS_SHARED], (unsigned long)start, len);
            if (ret != 0) {
                printk("%s: phy addr release failed, ret %d\r\n", __func__, ret);
                return -1;
            }
        } else {
            ret = k_pgt_munmap(cur_pid, (unsigned long)start, len);
            if (ret != 0) {
                printk("%s: k_pgt_munmap failed, ret %d\r\n", __func__, ret);
                return -1;
            }
        }
    }
    return 0;
#endif
}
#endif


/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "excore/k_space.h"

kspace_info_t g_space_info[SPACE_ID_LIMIT];

/* add memory space */
int k_space_init(kspace_info_t *info, unsigned long addr, size_t len)
{
    kspace_blk_t *blk_free;

    if ( info->flag == K_SPACE_INIT_FLAG ) {
        return -__LINE__;
    }

    info->flag          = K_SPACE_INIT_FLAG;
    info->address       = addr;
    info->total_size    = len;
    info->free_size     = len;
    info->free_size_min = len;
    info->freelist.next = NULL;
    info->freelist.prev = NULL;
    info->freelist.addr_start = 0;
    info->freelist.addr_end   = 0;

    /* Only one block when init. */
    blk_free = (kspace_blk_t *)krhino_mm_alloc(sizeof(kspace_blk_t));
    if (blk_free != NULL) {
        blk_free->next       = &(info->freelist);
        blk_free->prev       = &(info->freelist);
        blk_free->addr_start = addr;
        blk_free->addr_end   = addr + len;
        info->freelist.next  = blk_free;
        info->freelist.prev  = blk_free;
        return 0;
    }

    return -__LINE__;
}

/* remove the block from freelist */
int k_space_occupy(kspace_info_t *info, unsigned long addr, size_t len)
{
    kspace_blk_t *blk_find;    /* the block contain the addr  */
    kspace_blk_t *blk_left;

    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();

    if ( info->flag != K_SPACE_INIT_FLAG ) {
        RHINO_CPU_INTRPT_ENABLE();
        return -__LINE__;
    }

    if ( addr - info->address > info->total_size
         || addr + len - info->address > info->total_size ) {
        RHINO_CPU_INTRPT_ENABLE();
        return -__LINE__;
    }

    /* find a free block contain the addr */
    blk_find = info->freelist.next;
    while (blk_find != &(info->freelist)) {
        if ( addr >= blk_find->addr_start
             && addr + len <= blk_find->addr_end ) {
            /* find success */
            break;
        }
        blk_find = blk_find->next;
    }

    if (blk_find == &(info->freelist)) {
        /* not enough space */
        RHINO_CPU_INTRPT_ENABLE();
        return -__LINE__;
    }

    /* Results of memory block split:
       A. one piece, 'start' and 'end' match
       B. two pieces, only 'start' match
       C. two pieces, only 'end' match
       D. three pieces, neither 'start' or 'end' not match*/

    if (addr == blk_find->addr_start) {
        if ( addr + len == blk_find->addr_end ) {
            /* Result A, delete blk_find from freelist */
            blk_find->prev->next = blk_find->next;
            blk_find->next->prev = blk_find->prev;

            blk_find->prev = NULL;
            blk_find->next = NULL;
            krhino_mm_free(blk_find);
        } else {
            /* Result B, update blk_find */
            blk_find->addr_start = addr + len;
        }
    } else if ( addr + len == blk_find->addr_end ) {
        /* Result C, split the begin part of blk_find */
        blk_find->addr_end = addr;
    } else {
        /* Result D, new a free block */
        blk_left = (kspace_blk_t *)krhino_mm_alloc(sizeof(kspace_blk_t));
        if ( blk_left == NULL ) {
            RHINO_CPU_INTRPT_ENABLE();
            return -__LINE__;
        }
        blk_left->addr_start = addr + len;
        blk_left->addr_end   = blk_find->addr_end;

        /* insert blk_left after blk_find */
        blk_left->prev = blk_find;
        blk_left->next = blk_find->next;
        blk_find->next->prev = blk_left;
        blk_find->next = blk_left;

        /* split the begin part of blk_find */
        blk_find->addr_end = addr;
    }

    /* update statistic */
    info->free_size -= len;
    if (info->free_size < info->free_size_min) {
        info->free_size_min = info->free_size;
    }

    RHINO_CPU_INTRPT_ENABLE();
    return 0;
}

/* alloc block from freelist, physical address is aligned
   return the physical address */
unsigned long k_space_acquire(kspace_info_t *info, size_t len, size_t align, int reverse)
{
    unsigned long addr_alloc = 0;
    kspace_blk_t *blk_find;    /* the block get from freelist,  */
    kspace_blk_t *blk_left = NULL;

    OS_PRINT_DBG("%p, %zd\n", (void *)len, align);

    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();

    if ( info->flag != K_SPACE_INIT_FLAG ) {
        RHINO_CPU_INTRPT_ENABLE();
        return K_ARCH_INVALID_ADDR;
    }

    if ( len < sizeof(kspace_blk_t)
         || align == 0
         || len % align != 0
         || (align & (align - 1)) != 0 ) {
        RHINO_CPU_INTRPT_ENABLE();
        return K_ARCH_INVALID_ADDR;
    }

    if ( reverse == 0 ) {
        /* find a free block bigger than len */
        blk_find = info->freelist.next;
        while (blk_find != &(info->freelist)) {
            addr_alloc = OS_ALIGN_UP(blk_find->addr_start, align);
            if ( addr_alloc + len <= blk_find->addr_end ) {
                /* alloc success */
                break;
            }
            blk_find   = blk_find->next;
            addr_alloc = 0;
        }
    } else {
        /* find a free block bigger than len */
        blk_find = info->freelist.prev;
        while (blk_find != &(info->freelist)) {
            addr_alloc = OS_ALIGN_DOWN(blk_find->addr_end, align) - len;
            if ( addr_alloc >= blk_find->addr_start ) {
                /* alloc success */
                break;
            }
            blk_find = blk_find->prev;
            addr_alloc = 0;
        }
    }

    if (addr_alloc == 0) {
        RHINO_CPU_INTRPT_ENABLE();
        return K_ARCH_INVALID_ADDR;
    }

    /* Results of memory block split:
       A. one piece, 'start' and 'end' match
       B. two pieces, only 'start' match
       C. two pieces, only 'end' match
       D. three pieces, neither 'start' or 'end' not match*/

    if (addr_alloc == blk_find->addr_start) {
        if ( addr_alloc + len == blk_find->addr_end ) {
            /* Result A, delete blk_find from freelist */
            blk_find->prev->next = blk_find->next;
            blk_find->next->prev = blk_find->prev;

            blk_find->prev = NULL;
            blk_find->next = NULL;
            krhino_mm_free(blk_find);
        } else {
            /* Result B, update blk_find */
            blk_find->addr_start = addr_alloc + len;
        }
    } else if ( addr_alloc + len == blk_find->addr_end ) {
        /* Result C, split the begin part of blk_find */
        blk_find->addr_end = addr_alloc;
    } else {
        /* Result D, new a free block */
        blk_left = (kspace_blk_t *)krhino_mm_alloc(sizeof(kspace_blk_t));
        if ( blk_left == NULL ) {
            RHINO_CPU_INTRPT_ENABLE();
            return K_ARCH_INVALID_ADDR;
        }
        blk_left->addr_start = addr_alloc + len;
        blk_left->addr_end   = blk_find->addr_end;

        /* insert blk_left after blk_find */
        blk_left->prev = blk_find;
        blk_left->next = blk_find->next;
        blk_find->next->prev = blk_left;
        blk_find->next = blk_left;

        blk_find->addr_end = addr_alloc;
    }

    /* update statistic */
    info->free_size -= len;
    if (info->free_size < info->free_size_min) {
        info->free_size_min = info->free_size;
    }

    OS_PRINT_DBG("k_space_acquire: addr: %p, len 0x%x\n", (void *)addr_alloc, len);

    RHINO_CPU_INTRPT_ENABLE();
    return addr_alloc;
}

/* free block */
int k_space_release(kspace_info_t *info, unsigned long addr, size_t len)
{
    kspace_blk_t *blk_free;
    kspace_blk_t *blk_before; /* before the blk_free */
    kspace_blk_t *blk_after;  /* after  the blk_free */

    OS_PRINT_DBG("%p, %zd\n", (void *)addr, len);

    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();

    if ( info->flag != K_SPACE_INIT_FLAG ) {
        RHINO_CPU_INTRPT_ENABLE();
        return -__LINE__;
    }

    if ( info->freelist.next == &info->freelist ) {
        /* freelist is empty */
        blk_free = (kspace_blk_t *)krhino_mm_alloc(sizeof(kspace_blk_t));
        if ( blk_free == NULL ) {
            RHINO_CPU_INTRPT_ENABLE();
            return -__LINE__;
        }
        blk_free->next       = &(info->freelist);
        blk_free->prev       = &(info->freelist);
        blk_free->addr_start = addr;
        blk_free->addr_end   = addr + len;
        info->freelist.next  = blk_free;
        info->freelist.prev  = blk_free;

        info->free_size += len;
        RHINO_CPU_INTRPT_ENABLE();
        return 0;
    }

    if ( addr - info->address > info->total_size
         || addr + len - info->address > info->total_size ) {
        RHINO_CPU_INTRPT_ENABLE();
        return -__LINE__;
    }

    /* freelist is ordered by address, find blk_before */
    blk_before = &(info->freelist);
    while ( blk_before->next != &(info->freelist) ) {
        if ( blk_before->next->addr_start > addr ) {
            /* find position */
            break;
        }
        blk_before = blk_before->next;
    }
    blk_after = blk_before->next;

    /* now: blk_before->paddr_start < addr < blk_after->paddr_start */

    /* Results of memory block free:
       A. one piece, blk_before | addr | blk_after, stick to each other
       B. two pieces, blk_before | addr, stick to each other
       C. two pieces, addr | blk_after, stick to each other
       D. three pieces, no stick to */

    if (blk_before->addr_end == addr) {
        /* Result A or B, merge blk_before and addr */
        blk_before->addr_end = addr + len;

        if (blk_after != &(info->freelist) &&
            blk_before->addr_end == blk_after->addr_start) {
            /* Result A, merge blk_before and blk_after */
            blk_before->addr_end = blk_after->addr_end;

            blk_before->next = blk_after->next;
            blk_after->next->prev = blk_before;

            krhino_mm_free(blk_after);
        }
    } else {
        if (blk_after != &(info->freelist) &&
            addr + len == blk_after->addr_start) {
            /* Result C, merge addr and blk_after */
            blk_after->addr_start = addr;
        } else {
            /* Result D, no merge needed */
            blk_free = (kspace_blk_t *)krhino_mm_alloc(sizeof(kspace_blk_t));
            if ( blk_free == NULL ) {
                RHINO_CPU_INTRPT_ENABLE();
                return -__LINE__;
            }
            /* add a new block to freelist */
            blk_free->addr_start = addr;
            blk_free->addr_end   = addr + len;

            blk_before->next     = blk_free;
            blk_free->prev       = blk_before;
            blk_free->next       = blk_after;
            blk_after->prev      = blk_free;

            /* now: blk_before -> blk_free -> blk_after */
        }
    }

    info->free_size += len;

    RHINO_CPU_INTRPT_ENABLE();
    return 0;
}

int k_space_deinit(kspace_info_t *info)
{
    kspace_blk_t *blk_free, *blk_next;

    if (info->flag != K_SPACE_INIT_FLAG) {
        return -__LINE__;
    }

    info->flag = 0;

    blk_free = info->freelist.next;

    while (blk_free != &(info->freelist)) {
        blk_next = blk_free->next;
        krhino_mm_free(blk_free);
        blk_free = blk_next;
    }

    memset(info, 0, sizeof(kspace_info_t));
    return 0;
}

/* check if the addr belongs to space */
int k_space_check(kspace_info_t *info, unsigned long addr)
{

    if ( info->flag != K_SPACE_INIT_FLAG ) {
        return 0;
    }

    return ((unsigned long)addr - info->address < info->total_size);
}

size_t k_space_freesize(kspace_info_t *info)
{
    if ( info->flag != K_SPACE_INIT_FLAG ) {
        return 0;
    }

    return info->free_size;
}


void k_space_show(kspace_info_t *info)
{
    kspace_blk_t *blk;

    CPSR_ALLOC();

    if ( info->flag != K_SPACE_INIT_FLAG ) {
        OS_PRINT_SHOW("[k_space_show] Space is not initialized!\n");
        return;
    }

    OS_PRINT_SHOW("  addr          = 0x%"PRIXPTR"\n", info->address);
    OS_PRINT_SHOW("  total_size    = %"PRIdPTR"KB\n", info->total_size/1024);
    OS_PRINT_SHOW("  free_size     = %"PRIdPTR"KB\n", info->free_size/1024);
    OS_PRINT_SHOW("  free_size_min = %"PRIdPTR"KB\n", info->free_size_min/1024);

    RHINO_CPU_INTRPT_DISABLE();

    blk = info->freelist.next;

    while (blk != NULL) {
        OS_PRINT_SHOW("  free block- address 0x%"PRIXPTR"~0x%"PRIXPTR" / size 0x%"PRIXPTR"(%"PRIdPTR"KB)\n",
                       blk->addr_start, blk->addr_end, blk->addr_end - blk->addr_start, (blk->addr_end - blk->addr_start)/1024);
        blk = blk->next;
    }
    RHINO_CPU_INTRPT_ENABLE();
}

/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef K_SPACE_H
#define K_SPACE_H

#include "excore/k_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define K_SPACE_INIT_FLAG       0xEFEFEFEF

typedef enum {
    /* Physical Address Space for dynamic alloc */
    PAS_DYN,
    /* Virtual Address Space for kernel */
    VAS_KERNEL,
    /* Virtual Address Space for shared memory */
    VAS_SHARED,
    /* Virtual Address Space for IO memory */
    VAS_IO,

    /* More Physical Address Space, Add here */

    /* Always on the bottom */
    SPACE_ID_LIMIT
} kspace_id;

/* free block head */
typedef struct k_space_blk_tag {
    /* free memory block list, order by address*/
    struct k_space_blk_tag *next;
    struct k_space_blk_tag *prev;
    unsigned long addr_start;
    unsigned long addr_end;
} kspace_blk_t;

/* region info */
typedef struct {
    uint32_t  flag;
    unsigned long address;
    size_t    total_size;
    /* Size for statistic. */
    size_t    free_size;
    size_t    free_size_min;
    /* free block list: order by address, from low to high */
    kspace_blk_t freelist;
} kspace_info_t;

extern kspace_info_t g_space_info[SPACE_ID_LIMIT];

/**
 * Initialize a address space.
 * @param[in]  info    the space handler
 * @param[in]  addr       the address of space
 * @param[in]  len        the length of space
 * @return  0 success, < 0 error
 */
int k_space_init(kspace_info_t *info, unsigned long addr, size_t len);

/**
 * Occupy a address block from space.
 * @param[in]  info    the space handler
 * @param[in]  addr       the address of block
 * @param[in]  len        the length of block
 * @return  0 success, < 0 error
 */
int k_space_occupy(kspace_info_t *info, unsigned long addr, size_t len);

/**
 * Acquire a address block from space.
 * @param[in]  info    the space handler
 * @param[in]  len        the length of block
 * @param[in]  align      the align size of block
 * @param[in]  reverse    0 not reverse, 1 reverse
 * @return  the address, K_ARCH_INVALID_ADDR is fail
 */
unsigned long k_space_acquire(kspace_info_t *info, size_t len, size_t align, int reverse);

/**
 * Release a address block to space.
 * @param[in]  info    the space handler
 * @param[in]  addr       the address of block
 * @param[in]  len        the length of block
 * @return  0 success, < 0 error
 */
int k_space_release(kspace_info_t *info, unsigned long addr, size_t len);

/**
 * Check if the address belongs to the space.
 * @param[in]  info    the space handler
 * @param[in]  addr       the address of block
 * @return  0 no, 1 yes
 */
int k_space_check(kspace_info_t *info, unsigned long addr);

/**
 * Get the free space size.
 * @param[in]  info    the space handler
 * @return  freesize
 */
size_t k_space_freesize(kspace_info_t *info);

/**
 * Show information of the space.
 * @param[in]  info    the space handler
 * @return  none
 */
void k_space_show(kspace_info_t *info);

int k_space_deinit(kspace_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* K_SPACE_H */

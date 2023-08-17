/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#ifndef K_VMM_H
#define K_VMM_H

#include "excore/k_space.h"
#include "excore/k_asid.h"
#include "excore/k_mmu_sd.h"
#include "excore/k_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VMM_FLAG_PRIVATE            (1<<0)  /* Private memory */
#define VMM_FLAG_SHARED             (1<<1)  /* Shared memory between processes  */
#define VMM_FLAG_VA_REV             (1<<2)  /* Virtual address get from high memory */
#define VMM_FLAG_PA_REV             (1<<3)  /* Physical address get from high memory */
#define VMM_FLAG_VA_FIXED           (1<<4)  /* Virtual address can be specified */
#define VMM_FLAG_PA_FIXED           (1<<5)  /* Physical address can be specified */
#define VMM_FLAG_PA_ONE_PIECE       (1<<6)  /* Keep physical addresses continuous, */

typedef enum {
    /* Executable, Readable, Writable */
    VMM_PROT_ALL,
    /* Executable, Readable */
    VMM_PROT_EXE,
    /* Readable, Executable */
    VMM_PROT_WR,
    /* Readable */
    VMM_PROT_RO,
    /* no cache */
    VMM_PROT_NOCACHE

    /* More type */
} kvmm_prot_t;

typedef enum {
    VMM_TYPE_APP_LOAD   = VMM_FLAG_VA_FIXED | VMM_FLAG_PA_ONE_PIECE,
    VMM_TYPE_MAP_HEAP   = VMM_FLAG_PRIVATE,
    VMM_TYPE_MAP_SHARED = VMM_FLAG_SHARED,
    VMM_TYPE_MAP_DMA    = VMM_FLAG_PA_ONE_PIECE,
    VMM_TYPE_MAP_IO     = VMM_FLAG_PA_FIXED,

    /* More type */
} kvmm_type_t;

/* vmm infomation */
typedef struct {
    char *name;
} kvmm_info_t;

/* aligned to K_MMU_PAGE_ALIGN */
void *k_vmm_alloc(int pid, char *name, void *addr, size_t len, kvmm_prot_t prot, kvmm_type_t type);
int   k_vmm_free(int pid, char *name, void *addr, size_t len, kvmm_type_t type);
/* map virtual address to io address */
void *k_vmm_iomap(void *vaddr, void *paddr, size_t len, kvmm_prot_t prot, kvmm_type_t type);
#ifdef __cplusplus
}
#endif

#endif /* K_VMM_H */

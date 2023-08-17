/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_PHYMM_H
#define K_PHYMM_H

#include "excore/k_vmm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t total;
    size_t used;
    size_t maxused;
} k_phymm_statis_t;

int32_t   k_phymm_init(uintptr_t addr, size_t len);
int32_t   k_phymm_reserve(uintptr_t addr, size_t len);
uintptr_t k_phymm_alloc(int pid, size_t len, size_t align, int reverse, kvmm_type_t type);
int32_t   k_phymm_free(int pid, uintptr_t addr, size_t len, kvmm_type_t type);
size_t    k_phymm_freesize(void);
void      k_phymm_statis_show(int (*print_func)(const char *fmt, ...));
void      k_phymm_statis_get(k_phymm_statis_t *info);

#ifdef __cplusplus
}
#endif

#endif /* K_PHYMM_H */


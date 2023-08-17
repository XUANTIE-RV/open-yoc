/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_ASID_H
#define K_ASKD_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Address Space Identifier (ASID) */
/* 0 for kernel, others for APPs */
#define OS_ASID_KERNEL      0

#define OS_ASID_INV         ((uint32_t)-1)

/**
 * @brief allocate free asid from system
 *
 * @return On success, return the allocated asid,
 *         else return -1 to indicate no free asid is availible
 */
int k_asid_alloc(void);

/**
 * @brief free asid to system
 *
 * @param[in] asid  The asid to be freed
 *
 * @return On success, return 0, else return -1 to indicate
 *         that the asid is invalid
 */
int k_asid_dealloc(int asid);

/**
 * @brief Rserve system asid so that it will not be allocated in future
 *
 * @param[in] asid  The asid to be reserved.
 *
 * @return On success, return 0, else return -1 to indicate
 *         that the asid is invalid.
 */
int k_asid_reserve(int asid);

/**
 * @Check whether the asid is active
 *
 * @param [in] asid   The asid to be tested
 *
 * If the @asid is active, return 1, else return 0
 *
 * @note: If an asid is allocted, it's active
 */
int k_asid_is_active(int asid);

void k_asid_set(int asid);

int k_asid_get();
#ifdef __cplusplus
}
#endif

#endif /* K_ASID_H */


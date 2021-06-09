/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_efusec.h
 * @brief    header file for efusec driver
 * @version  V1.0
 * @date     22. Mar 2019
 * @model    efusec
 ******************************************************************************/
#ifndef _CSI_EFUSEC_H_
#define _CSI_EFUSEC_H_


#include <stdint.h>
//#include <drv_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/// definition for efusec handle.
typedef void *efusec_handle_t;

/**
\brief Flash information
*/
typedef struct {
    uint32_t          start;              ///< Chip Start address
    uint32_t          end;                ///< Chip End address (start+size-1)
    uint32_t          page_size;          ///< Optimal programming page size in bytes
    uint32_t          program_unit;       ///< Smallest programmable unit in bytes
} efusec_info_t;

/**
\brief Flash Status
*/
typedef struct {
    uint32_t mode  : 1;             ///< 0: read mode 1: program mode
    uint32_t lock_shadow : 1;       ///< lock shadow regs
    uint32_t lock_efuse : 1;        ///< lock efusec
} efusec_status_t;

typedef enum {
    EFUSEC_LOCK_SHADOW             = 0,    ///< all shadow regs can’t be programmed
    EFUSEC_LOCK_EFUSE                      ///< all Efuse can’t be programmed
} efusec_lock_e;

/**
\brief Flash Driver Capabilities.
*/
typedef struct {
    uint32_t lock_shadow   : 1;            ///< Supports lock shadow operation
} efusec_capabilities_t;

#ifdef __cplusplus
}
#endif

#endif /* _CSI_EFUSEC_H_ */

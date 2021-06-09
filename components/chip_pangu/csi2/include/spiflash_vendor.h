/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     spiflash_vendor.h
 * @brief
 * @version
 * @date     2020-03-26
 ******************************************************************************/

#ifndef _SPIFLASH_VENDOR_H_
#define _SPIFLASH_VENDOR_H_

#include <drv/spiflash.h>

#ifdef __cplusplus
extern "C" {
#endif

#define P25Q40UDWF
#define W25Q64FV
#define MX25L12833F
#define GD25Q127C
#define GD25WQ16E
#define XT25F128B

typedef enum {
    FLASH_ADDR_24,                        ///< 24bit address
    FLASH_ADDR_32,                        ///< 32bit address
} csi_spiflash_addr_mode_t;

typedef struct csi_spiflash_param {

    char      *flash_name;       ///< name string of spiflash
    uint32_t   flash_id;         ///< JEDEC ID  = manufature ID <<16 | device ID (ID15~ID0)
    csi_spiflash_addr_mode_t addr_mode; ///< flash addr mode(24bit or 32 bit)
    uint32_t   flash_size;              ///< flash chip size
    uint32_t   sector_size;             ///< sector size
    uint32_t   page_size;               ///< page size for read or program
    uint32_t   has_lock: 1;             ///< whether flash support lock function
    uint32_t   region:  2;              ///< spiflash lock region type, pls /ref csi_spiflash_lock_region_t
    uint32_t   bp_bits: 4;              ///< how many block protect bits to protect flash
    uint32_t   base_protect_size;       ///< smallest protect size
    uint32_t   qe_pos;                  ///< qe bit position
} csi_spiflash_param_t;

csi_error_t get_spiflash_vendor_param(uint32_t flash_id, void **param);

#ifdef __cplusplus
}
#endif

#endif  /* _SPIFLASH_VENDOR_H_*/

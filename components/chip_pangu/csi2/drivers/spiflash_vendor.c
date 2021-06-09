/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     spiflash_vendor.c
 * @brief
 * @version
 * @date     2020-03-26
 ******************************************************************************/

#include "spiflash_vendor.h"

const struct csi_spiflash_param g_spiflash_vendor_list[] = {
#ifdef P25Q40UDWF
    {
        .flash_name  = ( char * )"p25q40udwf",
        .flash_id    = 0x00856013U,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 512 * 1024 ),
        .sector_size = (uint32_t)( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 3U,
        .base_protect_size = ( uint32_t )( 64 * 1024 ),
        .qe_pos      = 9U
    },
#endif
#ifdef W25Q64FV
    {
        .flash_name  = ( char * )"w25q64fv",
        .flash_id    = 0x00c86018U,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 8 * 1024 * 1024 ),
        .sector_size = ( uint32_t )( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 3U,
        .base_protect_size = ( uint32_t )( 128 * 1024 ),
        .qe_pos      = 9U
    },
    {
        .flash_name  = ( char * )"w25q64fv",
        .flash_id    = 0x00ef4017U,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 8 * 1024 * 1024 ),
        .sector_size = ( uint32_t )( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 3U,
        .base_protect_size = ( uint32_t )( 128 * 1024 ),
        .qe_pos      = 9U
    },
#endif
#ifdef MX25L12833F
    {
        .flash_name  = ( char * )"mx25l12833f",
        .flash_id    = 0x00c22018U,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 16 * 1024 * 1024 ),
        .sector_size = ( uint32_t )( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 4U,
        .base_protect_size = ( uint32_t )( 64 * 1024 ),
        .qe_pos      = 6U
    },
#endif
#ifdef GD25Q127C
    {
        .flash_name  = ( char * )"gd25q127c",
        .flash_id    = 0x00c84018U,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 16 * 1024 * 1024 ),
        .sector_size = ( uint32_t )( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 3U,
        .base_protect_size = ( uint32_t )( 256 * 1024 ),
        .qe_pos      = 1U
    },
#endif
#ifdef GD25WQ16E
    {
        .flash_name  = ( char * )"gd25wq16e",
        .flash_id    = 0x00c86515U,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 2 * 1024 * 1024 ),
        .sector_size = ( uint32_t )( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 3U,
        .base_protect_size = ( uint32_t )( 256 * 1024 ),
        .qe_pos      = 9U
    },
#endif
#ifdef XT25F128B
    {
        .flash_name  = ( char * )"xt25f128b",
        .flash_id    = 0x000b4018,
        .addr_mode   = FLASH_ADDR_24,
        .flash_size  = ( uint32_t )( 16 * 1024 * 1024 ),
        .sector_size = ( uint32_t )( 4 * 1024 ),
        .page_size   = 256U,
        .has_lock    = 1U,
        .region      = (uint32_t)LOCK_TP_DUAL,
        .bp_bits     = 3U,
        .base_protect_size = ( uint32_t )( 256 * 1024 ),
        .qe_pos      = 9U
    }
#endif
};

csi_error_t get_spiflash_vendor_param(uint32_t flash_id, void **param)
{
    uint32_t i;
    csi_error_t ret = CSI_ERROR;
#ifdef CONFIG_CHIP_PANGU
        flash_id = 0x00c22018U;
#endif
    for (i = 0U; i < (sizeof(g_spiflash_vendor_list) / sizeof(g_spiflash_vendor_list[0])); i++) {
        if (g_spiflash_vendor_list[i].flash_id == flash_id) {
            *param = (void *)&g_spiflash_vendor_list[i];
            ret = CSI_OK;
            break;
        }
    }

    return ret;
}

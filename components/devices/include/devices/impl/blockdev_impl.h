/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_BLOCKDEV_IMPL_H
#define HAL_BLOCKDEV_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/blockdev.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct blockdev_driver {
    driver_t drv;
    int (*read_blks)(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt);
    int (*write_blks)(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt);
    int (*erase_blks)(rvm_dev_t *dev, uint32_t start_block, uint32_t block_cnt);
    int (*get_info)(rvm_dev_t *dev, rvm_hal_blockdev_info_t *info);
#if defined(CONFIG_COMP_SDMMC)
    int (*current_area)(rvm_dev_t *dev, rvm_hal_mmc_access_area_t *area);
    int (*select_area)(rvm_dev_t *dev, rvm_hal_mmc_access_area_t area);
#endif    
} blockdev_driver_t;

#ifdef __cplusplus
}
#endif

#endif
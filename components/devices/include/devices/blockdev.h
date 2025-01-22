 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _DEVICE_BLOCKDEV_H_
#define _DEVICE_BLOCKDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>


#if defined(CONFIG_COMP_SDMMC)
#include <mmc.h>

typedef struct {
    uint16_t sdif;
    uint16_t use_default_busWidth;
    mmc_voltage_window_t hostVoltageWindowVCCQ; /*!< Host IO voltage window */
    mmc_voltage_window_t hostVoltageWindowVCC;  /*!< application must set this value according to board specific */
    mmc_data_bus_width_t default_busWidth;      /*!< indicate the current work bus width */
} rvm_hal_mmc_config_t;

typedef enum {
    MMC_AccessPartitionUserAera = 0U,        /*!< No access to boot partition (default), normal partition */
    MMC_AccessPartitionBoot1 = 1U,           /*!< Read/Write boot partition 1 */
    MMC_AccessPartitionBoot2 = 2U,           /*!< Read/Write boot partition 2*/
    MMC_AccessRPMB = 3U,
} rvm_hal_mmc_access_area_t;

int rvm_hal_blockdev_mmc_select_area(rvm_dev_t *dev, rvm_hal_mmc_access_area_t area);

typedef struct {
    uint16_t sdif;
} rvm_hal_sd_config_t;

#endif /*CONFIG_COMP_SDMMC*/

typedef struct {
    uint32_t block_size;        /*!< block size */
    uint32_t erase_blks;        /*!< minmum erase blocks */
    uint32_t boot_area_blks;    /*!< emmc boot area blocks */
    uint32_t user_area_blks;    /*!< device blocks, user area blocks if emmc */
} rvm_hal_blockdev_info_t;

#define rvm_hal_blockdev_open(name) rvm_hal_device_open(name)
#define rvm_hal_blockdev_close(dev) rvm_hal_device_close(dev)

int rvm_hal_blockdev_read_blks(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt);
int rvm_hal_blockdev_write_blks(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt);
int rvm_hal_blockdev_erase_blks(rvm_dev_t *dev, uint32_t start_block, uint32_t block_cnt);
int rvm_hal_blockdev_get_info(rvm_dev_t *dev, rvm_hal_blockdev_info_t *info);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_blockdev.h>
#endif

#ifdef __cplusplus
}
#endif

#endif

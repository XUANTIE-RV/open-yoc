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
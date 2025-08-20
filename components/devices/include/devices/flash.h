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

#ifndef DEVICE_FLASH_PAI_H
#define DEVICE_FLASH_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

typedef struct {
    uint32_t    start_addr;
    uint32_t    sector_size;
    uint32_t    sector_count;
    uint16_t    page_size;                       ///< page-size of nand-device
    uint16_t    oob_size;                        ///< oob-size(spare size) of nand-device
    uint16_t    block_size;                      ///< block size
    uint16_t    max_bad_blocks;                  ///< max possible bad blocks of nand-device
    uint32_t    total_blocks;                    ///< total blocks of nand-device
} rvm_hal_flash_dev_info_t;

#define rvm_hal_flash_open(name) rvm_hal_device_open(name)
#define rvm_hal_flash_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Notify a flash to read data.
  \param[in]   dev      Pointer to device object.
  \param[in]   addroff  address to read flash.
  \param[out]  buff     buffer address to store data read.
  \param[in]   bytesize data length expected to read.
  \return      0 on success, -1 on fail.
*/
int rvm_hal_flash_read(rvm_dev_t *dev, int32_t addroff, void *buff, int32_t bytesize);

/**
  \brief       Program flash at specified address.
  \param[in]   dev      Pointer to device object.
  \param[in]   dstaddr  address to program flash.
  \param[in]   srcbuf   buffer storing data to be programmed.
  \param[in]   bytesize data length to be programmed.
  \return      0 on success, -1 on fail.
*/
int rvm_hal_flash_program(rvm_dev_t *dev, int32_t dstaddr, const void *srcbuf, int32_t bytesize);

/**
  \brief       Notify a flash to fetch data.
  \param[in]   dev      Pointer to device object.
  \param[in]   addroff  a flash section including the address will be erased.
  \param[in]   blkcnt   erased block count
  \return      0 on success, -1 on fail.
*/
int rvm_hal_flash_erase(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt);

/**
  \brief       Get info from a flash device.
  \param[in]   dev  Pointer to device object.
  \param[out]  info return flash info
  \return      0 on success, -1 on fail.
*/
int rvm_hal_flash_get_info(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_flash.h>
#endif

#ifdef __cplusplus
}
#endif

#endif

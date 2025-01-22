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
#ifndef __PARTITION_DEVICE_H__
#define __PARTITION_DEVICE_H__

#include <stdint.h>
#include <stddef.h>
#include <yoc/partition.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t sector_size;       // for eflash/spinorflash, 0 when spinandflash/emmc/sd/usb
    uint32_t block_size;        // for spinandflash/emmc/sd/usb
    uint32_t erase_size;        // the minimum erase size
#if CONFIG_PARTITION_SUPPORT_EMMC
    uint32_t boot_area_size;    // for emmc boot area
#endif
    uint64_t device_size;       // if emmc, user area size
    uint64_t base_addr;         // the base absolute address of the device
} partition_device_info_t;

typedef struct {
    void *dev_hdl;
    storage_info_t storage_info;

    void *(*find)(int device_id);
    int (*close)(void *dev_hdl);
    int (*info_get)(void *dev_hdl, partition_device_info_t *info);
    int (*read)(void *dev_hdl, off_t offset, void *data, size_t data_len);
    int (*write)(void *dev_hdl, off_t offset, void *data, size_t data_len);
    int (*erase)(void *dev_hdl, off_t offset, size_t len);
} partition_device_ops_t;

/**
 * Find storage device
 *
 * @param[in]  storage_info  Storage information, \ref storage_info_t
 * @return  NULL: If an error occurred with any step, otherwise is the storage device handle
 */
partition_device_ops_t *partition_device_find(storage_info_t *storage_info);

/**
 * Close storage device
 *
 * @param[in]   dev_ops Device opration class
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_device_close(partition_device_ops_t *dev_ops);

/**
 * Get storage device information with handle
 *
 * @param[in]   dev_ops Device opration class
 * @param[out]  info    Point to storage device info buffer
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_device_info_get(partition_device_ops_t *dev_ops, partition_device_info_t *info);

/**
 * Read data from storage device
 *
 * @param[in]  dev_ops      Device opration class
 * @param[in]  offset       Offset address relative to the device base address
 * @param[in]  data         Point to the data buffer that stores the data read from storage device
 * @param[in]  data_len     The length of the buffer
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_device_read(partition_device_ops_t *dev_ops, off_t offset, void *data, size_t data_len);

/**
 * Write data to storage device
 *
 * @param[in]  dev_ops      Device opration class
 * @param[in]  offset       Offset address relative to the device base address
 * @param[in]  data         Point to the data buffer that stores the data write to storage device
 * @param[in]  data_len     The length of the buffer
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_device_write(partition_device_ops_t *dev_ops, off_t offset, void *data, size_t data_len);

/**
 * Erase storage device with size
 *
 * @param[in]  dev_ops      Device opration class
 * @param[in]  offset       Offset address relative to the device base address
 * @param[in]  len          The length that need to erase
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_device_erase(partition_device_ops_t *dev_ops, off_t offset, size_t len);

/**
 * Register device operation
 *
 * @param[in]  dev_ops      Device opration class
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_device_register(partition_device_ops_t *dev_ops);

/**
 * Register eflash operation
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_eflash_register(void);

/**
 * Register spinorflash operation
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_flash_register(void);

/**
 * Register spinand flash operation
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_spinandflash_register(void);

/**
 * Register emmc operation
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_emmc_register(void);

/**
 * Register sd operation
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_sd_register(void);

/**
 * Register usb operation
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_usb_register(void);

#ifdef __cplusplus
}
#endif
#endif /* __PARTITION_DEVICE_H__ */
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

#ifndef YOC_PARTITION_H
#define YOC_PARTITION_H

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <mtb.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_MAX_PARTITION_NUM
#define CONFIG_MAX_PARTITION_NUM 12
#endif

#ifndef CONFIG_PARITION_NO_VERIFY
#define CONFIG_PARITION_NO_VERIFY 1
#endif

#ifndef CONFIG_NOT_SUPORRT_SASC
#define CONFIG_NOT_SUPORRT_SASC 1
#endif

#ifndef CONFIG_IMG_AUTHENTICITY_NOT_CHECK
#define CONFIG_IMG_AUTHENTICITY_NOT_CHECK 0
#endif

#ifndef CONFIG_PARTITION_SUPPORT_EFLASH
#define CONFIG_PARTITION_SUPPORT_EFLASH 0
#endif

#ifndef CONFIG_PARTITION_SUPPORT_SPINORFLASH
#define CONFIG_PARTITION_SUPPORT_SPINORFLASH 1
#endif

#ifndef CONFIG_PARTITION_SUPPORT_SPINANDFLASH
#define CONFIG_PARTITION_SUPPORT_SPINANDFLASH 0
#endif

#ifndef CONFIG_PARTITION_SUPPORT_EMMC
#define CONFIG_PARTITION_SUPPORT_EMMC 0
#endif

#ifndef CONFIG_PARTITION_SUPPORT_SD
#define CONFIG_PARTITION_SUPPORT_SD 0
#endif

#ifndef CONFIG_PARTITION_SUPPORT_USB
#define CONFIG_PARTITION_SUPPORT_USB 0
#endif

// Please define as 1 when use multi devices in config.yaml
#ifndef CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
#define CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV 0
#endif

#if CONFIG_PARTITION_SUPPORT_EMMC || CONFIG_PARTITION_SUPPORT_SD || CONFIG_PARTITION_SUPPORT_USB || CONFIG_PARTITION_SUPPORT_SPINANDFLASH
#undef CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
#define CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV 1
#endif

typedef struct {
    char image_name[MTB_IMAGE_NAME_SIZE];
    uint64_t part_addr;
    uint64_t part_size;
    uint64_t load_addr;
    uint32_t image_size;
    uint32_t preload_size;
#if CONFIG_PARTITION_SUPPORT_BLOCK_OR_MULTI_DEV
    storage_info_t storage_info;
#endif
} sys_partition_info_t;
/////////////////////////////////////////////////////

typedef int partition_t;

typedef struct {
    char     description[MTB_IMAGE_NAME_SIZE];  // 分区名字
    uint64_t base_addr;                         // 分区所在Flash的基地址
    uint64_t start_addr;                        // 分区的偏移地址（不是绝对地址）
    uint64_t length;                            // 分区大小
    uint32_t sector_size;                       // Flash sector大小
    uint32_t block_size;                        // bock大小
    uint32_t erase_size;                        // 最小擦除单元大小
    void    *flash_dev;                         // partition device handle
    uint32_t load_addr;                         // 加载地址，一般指在RAM中的运行地址
    uint32_t image_size;                        // 镜像实际大小
    storage_info_t storage_info;
    uint32_t preload_size;
#if CONFIG_PARTITION_SUPPORT_EMMC
    uint32_t boot_area_size;                    // for emmc boot area
#endif
} partition_info_t;

/**
 * Init the partition table
 *
 * @return  partition num, >0: On success， otherwise is error
 */
int partition_init(void);

/**
 * Open partition with partition name
 *
 * @note  You can find partition name in 'config.yaml'
 *
 * @param[in]  name     The partition name, e.g., boot,kv,prim
 *
 * @return  <0: If an error occurred with any step, otherwise is the partition handle
 */
partition_t partition_open(const char *name);

/**
 * Close partition with partition handle
 *
 * @param[in]  partition    The partition handle
 *
 */
void partition_close(partition_t partition);

/**
 * Get the information of the specified partition
 *
 * @param[in]  partition    The partition handle
 *
 * @return  0: On success， otherwise is error
 */
partition_info_t *partition_info_get(partition_t partition);

// Will be deprecated
#define hal_flash_get_info(partition) partition_info_get(partition)

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  partition       The target flash logical partition which should be read
 * @param[in]  off_set         Offset address relative to the partition start address
 * @param[in]  data            Point to the data buffer that stores the data read from flash
 * @param[in]  size            The length of the buffer
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_read(partition_t partition, off_t off_set, void *data, size_t size);

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  partition       The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Offset address relative to the partition start address
 * @param[in]  data            point to the data buffer that will be written to flash
 * @param[in]  size            The length of the buffer
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_write(partition_t partition, off_t off_set, void *data, size_t size);

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a erase_size that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected erase_size, the data
 *        will be lost.
 *
 * @param[in]  partition            The target flash logical partition which should be erased
 * @param[in]  off_set              Offset address relative to the partition start address
 * @param[in]  erase_unit_count     The count of erase_size with the device
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_erase(partition_t partition, off_t off_set, uint32_t erase_unit_count);

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a erase_size that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected erase_size, the data
 *        will be lost.
 *
 * @param[in]  partition     The target flash logical partition which should be erased
 * @param[in]  off_set       Offset address relative to the partition start address
 * @param[in]  size          The erase size, must be erase_size align
 *                           It will erase more erase_size when the size is not erase_size align.
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_erase_size(partition_t partition, off_t off_set, size_t size);

/**
 * Verify partition data
 *
 * @param[in]  partition     The partition handle
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_verify(partition_t partition);

/**
 * Verify all partition data
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_all_verify(void);

/**
 * Get SHA digest and SHA type for required partition
 *
 * @param[in]  partition     The partition handle
 * @param[in]  out_hash      Returned SHA digest for a given partition
 * @param[in]  out_len       Returned SHA digetst data length
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_get_digest(partition_t partition, uint8_t *out_hash, uint32_t *out_len);

/**
 * Set the specified partition for safe
 *
 * @param[in]  partition     The partition handle
 *
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_set_region_safe(partition_t partition);

/**
 * Check the partition firmware is packed
 *
 * @param[in]  partition     The partition handle
 *
 * @return  true : is packed, false: not packed
 */
bool partition_check_firmware_is_packed(partition_t partition);

/**
 * Get the information from a combine image partition
 *
 * @param[in]  partition     The partition handle
 * @param[in]  index         The index of the combined images
 * @param[out]  offset       The image offset in the partition
 * @param[out]  olen         The image length
 * @param[out]  run_address  The image run address
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_split_and_get(partition_t partition, int index, unsigned long *offset, size_t *olen, unsigned long *run_address);

/**
 * Split the combined partition data, then decompress and copy to the run address
 *
 * @param[in]  partition     The partition handle
 * @param[in]  index         The index of the combined images
 * @return  0 : On success, <0 If an error occurred with any step
 */
int partition_split_and_copy(partition_t partition, int index);

#ifdef __cplusplus
}
#endif

#endif
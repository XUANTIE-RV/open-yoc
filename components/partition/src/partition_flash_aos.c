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
#include "yoc/partition.h"

#if CONFIG_PARTITION_SUPPORT_SPINORFLASH && !defined(CONFIG_KERNEL_NONE) && defined(CONFIG_AOS_FLASH)
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "yoc/partition_device.h"
#include "mtb_log.h"
#include "aos/hal/flash.h"

void *part_flash_open(int id)
{
    // rvm_hal_flash_open if use rvm_hal
    
    switch (id)
    {
    default:
    case 0:
        return (void*)HAL_ALL_FLASH_0;
    case 1:
        return (void*)HAL_ALL_FLASH_1;
    case 2:
        return (void*)HAL_ALL_FLASH_2;
    }
    return NULL;
}

int part_flash_info_get(void *handle, partition_device_info_t *info)
{
    int rc;
    hal_logic_partition_t partition;

    if (handle && info) {
        rc = hal_flash_info_get((hal_partition_t)handle, &partition);
        if (rc == 0) {
            info->base_addr = partition.partition_start_addr;
            info->sector_size = partition.partition_length / partition.partition_options;
            info->device_size = partition.partition_length;
            info->erase_size = info->sector_size;
            info->block_size = 0;
#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 2
            static int iprintflag = 0;
            if (!iprintflag) {
                MTB_LOGD("info->base_addr:0x%" PRIX64, info->base_addr);
                MTB_LOGD("info->sector_size:0x%x", info->sector_size);
                MTB_LOGD("info->block_size:0x%x", info->block_size);
                MTB_LOGD("info->erase_size:0x%x", info->erase_size);
                MTB_LOGD("info->device_size:0x%" PRIX64, info->device_size);
                iprintflag = 1;
            }
#endif
            return 0;
        }
    }
    return -EINVAL;
}

int part_flash_read(void *handle, off_t offset, void *data, size_t data_len)
{
    int rc;
    uint32_t off_set = offset;

    if (data_len == 0) {
        return 0;
    }

    if (handle && data && data_len > 0) {
        hal_logic_partition_t partition;

        rc = hal_flash_info_get((hal_partition_t)handle, &partition);
        if (rc == 0) {
            if (data_len > partition.partition_length ||
                offset + data_len > partition.partition_length) {
                return -EINVAL;
            }
            return hal_flash_read((hal_partition_t)handle, &off_set, data, data_len);
        }
    }
    return -EINVAL;
}

int part_flash_write(void *handle, off_t offset, void *data, size_t data_len)
{
    int rc;
    uint32_t off_set = offset;

    if (data_len == 0) {
        return 0;
    }

    if (handle && data && data_len > 0) {
        hal_logic_partition_t partition;

        rc = hal_flash_info_get((hal_partition_t)handle, &partition);
        if (rc == 0) {
            if (data_len > partition.partition_length ||
                offset + data_len > partition.partition_length) {
                return -EINVAL;
            }
            return hal_flash_write((hal_partition_t)handle, &off_set, data, data_len);
        }
    }
    return -EINVAL;
}

int part_flash_erase(void *handle, off_t offset, size_t len)
{
    int rc;

    if (len == 0) {
        return 0;
    }

    if (handle && len > 0) {
        hal_logic_partition_t partition;

        rc = hal_flash_info_get((hal_partition_t)handle, &partition);
        if (rc == 0) {
            if (len > partition.partition_length ||
                offset + len > partition.partition_length) {
                return -EINVAL;
            }
            return hal_flash_erase((hal_partition_t)handle, offset, len);
        }
    }
    return -EINVAL;
}

static partition_device_ops_t default_flash_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_SPI_NOR_FLASH,
    .find     = part_flash_open,
    .info_get = part_flash_info_get,
    .read     = part_flash_read,
    .write    = part_flash_write,
    .erase    = part_flash_erase
};

int partition_flash_register(void)
{
    return partition_device_register(&default_flash_ops);
}
#endif
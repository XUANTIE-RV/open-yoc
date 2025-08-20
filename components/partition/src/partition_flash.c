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

#if (CONFIG_PARTITION_SUPPORT_SPINORFLASH || CONFIG_PARTITION_SUPPORT_EFLASH || CONFIG_PARTITION_SUPPORT_SPINANDFLASH) \
    && !defined(CONFIG_KERNEL_NONE) && !defined(CONFIG_AOS_FLASH)
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <devices/flash.h>
#include "yoc/partition_device.h"
#include "mtb_log.h"

static int part_flash_info_get(void *handle, partition_device_info_t *info)
{
    int rc;
    rvm_hal_flash_dev_info_t flash_info;

    if (handle && info) {
        rc = rvm_hal_flash_get_info(handle, &flash_info);
        if (rc == 0) {
            info->base_addr = flash_info.start_addr;
            info->sector_size = flash_info.sector_size;
            info->device_size = (uint64_t)flash_info.sector_size * flash_info.sector_count;
            info->erase_size = info->sector_size;
            info->block_size = flash_info.block_size;
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

static int part_flash_read(void *handle, off_t offset, void *data, size_t data_len)
{
    int rc;

    if (data_len == 0) {
        return 0;
    }

    if (handle && data && data_len > 0) {
        uint64_t device_size;
        rvm_hal_flash_dev_info_t flash_info;

        rc = rvm_hal_flash_get_info(handle, &flash_info);
        if (rc == 0) {
            device_size = (uint64_t)flash_info.sector_size * flash_info.sector_count;
            if (data_len > device_size || offset + data_len > device_size) {
                MTB_LOGE("read size overflow.");
                return -EINVAL;
            }
            return rvm_hal_flash_read(handle, offset, data, data_len);
        }
    }
    MTB_LOGE("read size arg e.");
    return -EINVAL;
}

static int part_flash_write(void *handle, off_t offset, void *data, size_t data_len)
{
    int rc;

    if (data_len == 0) {
        return 0;
    }

    if (handle && data && data_len > 0) {
        uint64_t device_size;
        rvm_hal_flash_dev_info_t flash_info;

        rc = rvm_hal_flash_get_info(handle, &flash_info);
        if (rc == 0) {
            device_size = (uint64_t)flash_info.sector_size * flash_info.sector_count;
            if (data_len > device_size || offset + data_len > device_size) {
                MTB_LOGE("write size overflow.");
                return -EINVAL;
            }
            return rvm_hal_flash_program(handle, offset, data, data_len);
        }
    }
    MTB_LOGE("write size arg e.");
    return -EINVAL;
}

static int part_flash_erase(void *handle, off_t offset, size_t len)
{
    int rc;

    if (len == 0) {
        return 0;
    }

    if (handle && len > 0) {
        uint64_t device_size;
        rvm_hal_flash_dev_info_t flash_info;

        rc = rvm_hal_flash_get_info(handle, &flash_info);
        if (rc == 0) {
            device_size = (uint64_t)flash_info.sector_size * flash_info.sector_count;
            if (len > device_size || offset + len > device_size) {
                MTB_LOGE("erase size overflow.");
                return -EINVAL;
            }
            return rvm_hal_flash_erase(handle, offset, (len + flash_info.sector_size -1) / flash_info.sector_size);
        }
    }
    MTB_LOGE("erase size arg e.");
    return -EINVAL;
}

static int part_flash_close(void *handle)
{
    if (handle)
        return rvm_hal_flash_close(handle);
    return -EINVAL;
}

#if CONFIG_PARTITION_SUPPORT_SPINORFLASH
static void *part_flash_find(int id)
{
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "flash%d", id);
    rvm_dev_t *handle = rvm_hal_flash_open(buffer);
    return handle;
}

static partition_device_ops_t default_flash_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_SPI_NOR_FLASH,
    .find     = part_flash_find,
    .close    = part_flash_close,
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

#if CONFIG_PARTITION_SUPPORT_EFLASH
static void *part_eflash_find(int id)
{
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "eflash%d", id);
    rvm_dev_t *handle = rvm_hal_flash_open(buffer);
    return handle;
}

static partition_device_ops_t default_eflash_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_EFLASH,
    .find     = part_eflash_find,
    .close    = part_flash_close,
    .info_get = part_flash_info_get,
    .read     = part_flash_read,
    .write    = part_flash_write,
    .erase    = part_flash_erase
};

int partition_eflash_register(void)
{
    return partition_device_register(&default_eflash_ops);
}
#endif

#if CONFIG_PARTITION_SUPPORT_SPINANDFLASH
static void *part_spinandflash_find(int id)
{
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "spinand%d", id);
    rvm_dev_t *handle = rvm_hal_flash_open(buffer);
    return handle;
}

static partition_device_ops_t default_spinand_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_SPI_NAND_FLASH,
    .find     = part_spinandflash_find,
    .close    = part_flash_close,
    .info_get = part_flash_info_get,
    .read     = part_flash_read,
    .write    = part_flash_write,
    .erase    = part_flash_erase
};

int partition_spinandflash_register(void)
{
    return partition_device_register(&default_spinand_ops);
}
#endif

#endif
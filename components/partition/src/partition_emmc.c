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

#if (CONFIG_PARTITION_SUPPORT_EMMC || CONFIG_PARTITION_SUPPORT_SD || CONFIG_PARTITION_SUPPORT_USB) && !defined(CONFIG_KERNEL_NONE)
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <devices/blockdev.h>
#include "yoc/partition_device.h"
#include "mtb_log.h"

#define RW_BLOCK_SIZE 512

static int part_blockdev_info_get(void *handle, partition_device_info_t *info)
{
    if (handle && info) {
        rvm_hal_blockdev_info_t mmc_info;

        if (rvm_hal_blockdev_get_info(handle, &mmc_info)) {
            return -1;
        }
        info->base_addr = 0;
        info->sector_size = 0;
        info->block_size = mmc_info.block_size;
        info->erase_size = mmc_info.erase_blks * mmc_info.block_size;
        info->device_size = (uint64_t)mmc_info.user_area_blks * mmc_info.block_size;
#if CONFIG_PARTITION_SUPPORT_EMMC
        info->boot_area_size = mmc_info.boot_area_blks * mmc_info.block_size;
#endif
#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 2
        static int iprintflag = 0;
        if (!iprintflag) {
            MTB_LOGD("info->base_addr:0x%" PRIX64, info->base_addr);
            MTB_LOGD("info->sector_size:0x%x", info->sector_size);
            MTB_LOGD("info->block_size:0x%x", info->block_size);
            MTB_LOGD("info->erase_size:0x%x", info->erase_size);
            MTB_LOGD("info->device_size:0x%" PRIX64, info->device_size);
#if CONFIG_PARTITION_SUPPORT_EMMC
            MTB_LOGD("info->boot_area_size:0x%x", info->boot_area_size);
#endif
            iprintflag = 1;
        }
#endif
        return 0;
    }
    return -EINVAL;
}

static int part_blockdev_read(void *handle, off_t offset, void *data, size_t data_len)
{
    uint32_t start_block;
    uint32_t block_cnt;
    size_t left_size;
    uint32_t offt_data_left, offset_mod;
    uint8_t tmpbuf[RW_BLOCK_SIZE];
    uint8_t offt_tmpbuf[RW_BLOCK_SIZE];

    // MTB_LOGD("offset:0x%x, data_len:%d", offset, (uint32_t)data_len);
    if (data_len == 0) {
        return 0;
    }

    if (handle && data && data_len > 0) {
        rvm_hal_blockdev_info_t mmc_info;

        if (rvm_hal_blockdev_get_info(handle, &mmc_info)) {
            MTB_LOGE("info get e");
            return -1;
        }
        offt_data_left = 0;
        offset_mod = offset % mmc_info.block_size;
        start_block = offset / mmc_info.block_size;
        if (offset_mod) {
            // MTB_LOGW("offset not align block size, offset:0x%x", offset);
            // MTB_LOGD("read startblk:%d, blkcount:%d to offt_tmpbuf", start_block, 1);
            if (rvm_hal_blockdev_read_blks(handle, offt_tmpbuf, start_block, 1)) {
                MTB_LOGE("read blks e");
                return -1;
            }
            offt_data_left = mmc_info.block_size - offset_mod;
            if (offt_data_left >= data_len) {
                memcpy(data, (void *)((unsigned long)offt_tmpbuf + offset_mod), data_len);
                return 0;
            }
            memcpy(data, (void *)((unsigned long)offt_tmpbuf + offset_mod), offt_data_left);
            start_block += 1;
            data_len -= offt_data_left;
        }
        left_size = data_len % mmc_info.block_size;
        block_cnt = data_len / mmc_info.block_size;
        // MTB_LOGD("read startblk:%d, blkcount:%d, left_size:0x%x", start_block, block_cnt, (uint32_t)left_size);
        if (block_cnt > 0) {
            if (rvm_hal_blockdev_read_blks(handle, (void *)((unsigned long)data + offt_data_left), start_block, block_cnt)) {
                MTB_LOGE("read blks e");
                return -1;
            }
            if (left_size) {
                // MTB_LOGD("read left size from blk:%d", start_block + block_cnt);
                if (rvm_hal_blockdev_read_blks(handle, tmpbuf, start_block + block_cnt, 1)) {
                    MTB_LOGE("read 1 blk e");
                    return -1;
                }
                memcpy((void *)((unsigned long)data + offt_data_left + block_cnt * mmc_info.block_size), tmpbuf, left_size);
            }
        } else {
            if (rvm_hal_blockdev_read_blks(handle, tmpbuf, start_block, 1)) {
                MTB_LOGE("read 1 blk e");
                return -1;
            }
            memcpy((void *)((unsigned long)data + offt_data_left), tmpbuf, left_size);
        }
        return 0;
    }
    MTB_LOGE("read arg e.");
    return -EINVAL;
}

static int part_blockdev_write(void *handle, off_t offset, void *data, size_t data_len)
{
    uint32_t start_block;
    uint32_t block_cnt;
    size_t left_size;
    uint32_t offt_data_left, offset_mod;
    uint8_t tmpbuf[RW_BLOCK_SIZE];
    uint8_t offt_tmpbuf[RW_BLOCK_SIZE];

    // MTB_LOGD("offset:0x%x, data_len:%d", offset, (uint32_t)data_len);
    if (data_len == 0) {
        return 0;
    }

    if (handle && data && data_len > 0) {
        rvm_hal_blockdev_info_t mmc_info;

        if (rvm_hal_blockdev_get_info(handle, &mmc_info)) {
            MTB_LOGE("info get e");
            return -1;
        }
        offt_data_left = 0;
        offset_mod = offset % mmc_info.block_size;
        start_block = offset / mmc_info.block_size;
        if (offset_mod) {
            // MTB_LOGW("offset not align block size, offset:0x%x", offset);
            // MTB_LOGD("read startblk:%d, blkcount:%d to offt_tmpbuf", start_block, 1);
            if (rvm_hal_blockdev_read_blks(handle, offt_tmpbuf, start_block, 1)) {
                MTB_LOGE("read blks e");
                return -1;
            }
            offt_data_left = mmc_info.block_size - offset_mod;
            if (offt_data_left >= data_len) {
                memcpy((void *)((unsigned long)offt_tmpbuf + offset_mod), data, data_len);
            } else {
                memcpy((void *)((unsigned long)offt_tmpbuf + offset_mod), data, offt_data_left);
            }
            if (rvm_hal_blockdev_write_blks(handle, offt_tmpbuf, start_block, 1)) {
                MTB_LOGE("write blks e");
                return -1;
            }
            if (offt_data_left >= data_len) {
                return 0;
            }
            start_block += 1;
            data_len -= offt_data_left;
        }
        left_size = data_len % mmc_info.block_size;
        block_cnt = data_len / mmc_info.block_size;
        // MTB_LOGD("write startblk:%d, blkcount:%d, left_size:0x%x", start_block, block_cnt, (uint32_t)left_size);
        if (block_cnt > 0) {
            if (rvm_hal_blockdev_write_blks(handle, (uint8_t *)data + offt_data_left, start_block, block_cnt)) {
                MTB_LOGE("write blks e");
                return -1;
            }
        }
        if (left_size) {
            // MTB_LOGD("read 1 block from blk:%d", start_block + block_cnt);
            if (rvm_hal_blockdev_read_blks(handle, tmpbuf, start_block + block_cnt, 1)) {
                MTB_LOGE("read 1 blk e");
                return -1;
            }
            memcpy(tmpbuf, (void *)((unsigned long)data + offt_data_left + block_cnt * mmc_info.block_size), left_size);
            // MTB_LOGD("write left_size to blk:%d", start_block + block_cnt);
            if (rvm_hal_blockdev_write_blks(handle, tmpbuf, start_block + block_cnt, 1)) {
                MTB_LOGE("write blks e");
                return -1;
            }
        }
        return 0;
    }
    MTB_LOGE("write arg e.");
    return -EINVAL;
}

static int part_blockdev_erase(void *handle, off_t offset, size_t len)
{
    uint32_t start_block;
    uint32_t block_cnt;
    uint32_t erase_size;

    // MTB_LOGD("offset:0x%x, len:%d", offset, (uint32_t)len);
    if (len == 0) {
        return 0;
    }

    if (handle && len > 0) {
        rvm_hal_blockdev_info_t mmc_info;

        if (rvm_hal_blockdev_get_info(handle, &mmc_info)) {
            MTB_LOGE("info get e");
            return -1;
        }
        erase_size = mmc_info.erase_blks * mmc_info.block_size;
        if (offset % erase_size) {
            MTB_LOGE("offset not erase_size align: 0x%lx, erase_size:0x%x", offset, erase_size);
            return -EINVAL;
        }
        if (len % erase_size) {
            len = (len + erase_size - 1) / erase_size * erase_size;
        }
        start_block = offset / mmc_info.block_size;
        block_cnt = len / mmc_info.block_size;
        // MTB_LOGD("erase startblk:%d, blkcount:%d", start_block, block_cnt);
        if (rvm_hal_blockdev_erase_blks(handle, start_block, block_cnt)) {
            MTB_LOGE("erase blks e");
            return -1;
        }
        return 0;
    }
    MTB_LOGE("erase arg e.");
    return -EINVAL;
}

static int part_blockdev_close(void *handle)
{
    if (handle)
        return rvm_hal_blockdev_close(handle);
    return -EINVAL;
}

#if CONFIG_PARTITION_SUPPORT_EMMC
static void *part_emmc_find(int id)
{
    char buffer[8];

    snprintf(buffer, sizeof(buffer), "mmc%d", id);
    rvm_dev_t *handle = rvm_hal_blockdev_open(buffer);
    return handle;
}

static partition_device_ops_t default_emmc_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_EMMC,
    .find     = part_emmc_find,
    .close    = part_blockdev_close,
    .info_get = part_blockdev_info_get,
    .read     = part_blockdev_read,
    .write    = part_blockdev_write,
    .erase    = part_blockdev_erase,
};

int partition_emmc_register(void)
{
    return partition_device_register(&default_emmc_ops);
}
#endif

#if CONFIG_PARTITION_SUPPORT_SD
static void *part_sd_find(int id)
{
    char buffer[8];

    snprintf(buffer, sizeof(buffer), "sd%d", id);
    rvm_dev_t *handle = rvm_hal_blockdev_open(buffer);
    return handle;
}

static partition_device_ops_t default_sd_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_SD,
    .find     = part_sd_find,
    .close    = part_blockdev_close,
    .info_get = part_blockdev_info_get,
    .read     = part_blockdev_read,
    .write    = part_blockdev_write,
    .erase    = part_blockdev_erase,
};

int partition_sd_register(void)
{
    return partition_device_register(&default_sd_ops);
}
#endif

#if CONFIG_PARTITION_SUPPORT_USB
static void *part_usb_find(int id)
{
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "usb_mass%d", id);
    rvm_dev_t *handle = rvm_hal_blockdev_open(buffer);
    return handle;
}

static partition_device_ops_t default_usb_ops = {
    .storage_info.id    = 0,
    .storage_info.type  = MEM_DEVICE_TYPE_USB,
    .find     = part_usb_find,
    .close    = part_blockdev_close,
    .info_get = part_blockdev_info_get,
    .read     = part_blockdev_read,
    .write    = part_blockdev_write,
    .erase    = part_blockdev_erase,
};

int partition_usb_register(void)
{
    return partition_device_register(&default_usb_ops);
}
#endif

#endif
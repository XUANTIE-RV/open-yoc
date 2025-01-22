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
#if defined(CONFIG_COMP_SDMMC)
#include <stdio.h>
#include <errno.h>
#include <sd.h>
#include <drv/sdif.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <devices/impl/blockdev_impl.h>

#define TAG "sd_drv"

typedef struct {
    rvm_dev_t   device;
    sd_card_t   SDIO_SDCard;
    rvm_hal_sd_config_t config;
    rvm_hal_blockdev_info_t info;
} sd_dev_t;

#define _sd_uninit rvm_hal_device_free

static rvm_dev_t *_sd_init(driver_t *drv, void *config, int id)
{
    sd_dev_t *dev = (sd_dev_t*)rvm_hal_device_new(drv, sizeof(sd_dev_t), id);
    if (dev && config) {
        memcpy(&dev->config, config, sizeof(rvm_hal_mmc_config_t));
    }

    return (rvm_dev_t*)dev;
}

static int _sd_open(rvm_dev_t *dev)
{
    sd_dev_t *sd = (sd_dev_t*)dev;
    if (!sd)
        return -EINVAL;
    memset(&sd->SDIO_SDCard, 0, sizeof(sd->SDIO_SDCard));
    status_t ret = SD_Init(&sd->SDIO_SDCard, NULL, sd->config.sdif);
    if (ret != kStatus_Success) {
        LOGE(TAG, "=====>>>>>>>>>>>>%s, %d=====SD_Init failed", __FUNCTION__, __LINE__);
        SD_Deinit(&sd->SDIO_SDCard);
        memset(&sd->SDIO_SDCard, 0, sizeof(sd->SDIO_SDCard));
        return -EIO;
    }
    LOGD(TAG, "sd init ok.");
    sd->info.block_size = sd->SDIO_SDCard.block_size;
    if ((sd->SDIO_SDCard.csd.flags & kSD_CsdEraseBlockEnabledFlag) == kSD_CsdEraseBlockEnabledFlag)
        sd->info.erase_blks = 1;
    else
        sd->info.erase_blks = sd->SDIO_SDCard.csd.eraseSectorSize + 1;
    sd->info.user_area_blks = sd->SDIO_SDCard.block_count;
    sd->info.boot_area_blks = 0;
    if (sd->info.block_size != 512) {
        LOGE(TAG, "sd block_size is %d", sd->info.block_size);
        return -1;
    }
#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 2
    static int iprintflag = 0;
    if (!iprintflag) {
        LOGD(TAG, "info->block_size:%d", sd->info.block_size);
        LOGD(TAG, "info->erase_blks:%d", sd->info.erase_blks);
        LOGD(TAG, "info->boot_area_blks:%d", sd->info.boot_area_blks);
        LOGD(TAG, "info->user_area_blks:%d", sd->info.user_area_blks);
        iprintflag = 1;
    }
#endif
    return 0;
}

static int _sd_close(rvm_dev_t *dev)
{
    sd_dev_t *sd = (sd_dev_t*)dev;

    if (!sd)
        return -EINVAL;
    SD_Deinit(&sd->SDIO_SDCard);

    return 0;
}

static int _sd_read(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    sd_dev_t *sd = (sd_dev_t*)dev;

    if (!sd || !buffer)
        return -EINVAL;
    if (block_cnt == 0)
        return 0;
    // LOGD(TAG, "start_block:%d, block_cnt:%d", start_block, block_cnt);
    int32_t ret = SD_ReadBlocks(&sd->SDIO_SDCard, buffer, start_block, block_cnt);
    if (kStatus_Success != ret) {
        LOGE(TAG, "read start_block:%d, block_cnt:%d, ret:%d", start_block, block_cnt, ret);
        return -EIO;
    }

    return 0;
}

static int _sd_write(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    sd_dev_t *sd = (sd_dev_t*)dev;

    if (!sd || !buffer)
        return -EINVAL;
    if (block_cnt == 0)
        return 0;
    // LOGD(TAG, "start_block:%d, block_cnt:%d", start_block, block_cnt);
    int32_t ret = SD_WriteBlocks(&sd->SDIO_SDCard, buffer, start_block, block_cnt);
    if (kStatus_Success != ret) {
        LOGE(TAG, "write start_block:%d, block_cnt:%d, ret:%d", start_block, block_cnt, ret);
        return -EIO;
    }

    return 0;
}

static int _sd_erase(rvm_dev_t *dev, uint32_t start_block, uint32_t block_cnt)
{
    sd_dev_t *sd = (sd_dev_t*)dev;

    if (!sd)
        return -EINVAL;
    if (block_cnt == 0)
        return 0;
    // LOGD(TAG, "start_block:%d, block_cnt:%d", start_block, block_cnt);
    int32_t ret = SD_EraseBlocks(&sd->SDIO_SDCard, start_block, block_cnt);
    if (kStatus_Success != ret) {
        LOGE(TAG, "erase start_block:%d, block_cnt:%d, ret:%d", start_block, block_cnt, ret);
        return -EIO;
    }
    return 0;
}

static int _sd_get_info(rvm_dev_t *dev, rvm_hal_blockdev_info_t *info)
{
    sd_dev_t *sd = (sd_dev_t*)dev;

    if (!sd || !info)
        return -EINVAL;

    info->block_size = sd->info.block_size;
    info->erase_blks = sd->info.erase_blks;
    info->user_area_blks = sd->info.user_area_blks;
    info->boot_area_blks = sd->info.boot_area_blks;

    return 0;
}

static blockdev_driver_t sd_driver = {
    .drv = {
        .name   = "sd",
        .init   = _sd_init,
        .uninit = _sd_uninit,
        .open   = _sd_open,
        .close  = _sd_close,
    },
    .read_blks      = _sd_read,
    .write_blks     = _sd_write,
    .erase_blks     = _sd_erase,
    .get_info       = _sd_get_info,
    .current_area   = NULL,
    .select_area    = NULL,
};

void rvm_sd_drv_register(int idx, rvm_hal_sd_config_t *config)
{
    rvm_driver_register(&sd_driver.drv, config, idx);
}
#endif /*CONFIG_COMP_SDMMC*/
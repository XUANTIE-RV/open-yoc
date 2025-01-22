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
#include <mmc.h>
#include <drv/sdif.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <devices/impl/blockdev_impl.h>

#define TAG "mmc_drv"

typedef struct {
    rvm_dev_t   device;
    mmc_card_t  SDIO_MMCCard;
    rvm_hal_mmc_config_t config;
    rvm_hal_blockdev_info_t info;
} mmc_dev_t;

#define _mmc_uninit rvm_hal_device_free

static rvm_dev_t *_mmc_init(driver_t *drv, void *config, int id)
{
    mmc_dev_t *dev = (mmc_dev_t*)rvm_hal_device_new(drv, sizeof(mmc_dev_t), id);
    if (dev && config) {
        memcpy(&dev->config, config, sizeof(rvm_hal_mmc_config_t));
    }

    return (rvm_dev_t*)dev;
}

static int _mmc_open(rvm_dev_t *dev)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;
    if (!mmc)
        return -EINVAL;
    memset(&mmc->SDIO_MMCCard, 0U, sizeof(mmc_card_t));
    mmc->SDIO_MMCCard.hostVoltageWindowVCC = mmc->config.hostVoltageWindowVCC;
    mmc->SDIO_MMCCard.hostVoltageWindowVCCQ = mmc->config.hostVoltageWindowVCCQ;
    if (mmc->config.use_default_busWidth)
        mmc->SDIO_MMCCard.busWidth = mmc->config.default_busWidth;
    status_t ret = MMC_Init(&mmc->SDIO_MMCCard, NULL, mmc->config.sdif);
    if (ret != kStatus_Success) {
        LOGE(TAG, "=====>>>>>>>>>>>>%s, %d=====MMC_Init failed", __FUNCTION__, __LINE__);
        MMC_Deinit(&mmc->SDIO_MMCCard);
        memset(&mmc->SDIO_MMCCard, 0U, sizeof(mmc_card_t));
        return -EIO;
    }
    LOGD(TAG, "mmc init ok.");
    mmc->info.block_size = mmc->SDIO_MMCCard.block_size;
    mmc->info.erase_blks = mmc->SDIO_MMCCard.eraseGroupBlocks;
    mmc->info.boot_area_blks = mmc->SDIO_MMCCard.bootPartitionBlocks;
    mmc->info.user_area_blks = mmc->SDIO_MMCCard.userPartitionBlocks;
    if (mmc->info.block_size != 512) {
        LOGE(TAG, "mmc block_size is %d", mmc->info.block_size);
        return -1;
    }
    if (mmc->info.erase_blks != 1024) {
        LOGW(TAG, "mmc erase_blks is %d", mmc->info.erase_blks);
    }
#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 2
    static int iprintflag = 0;
    if (!iprintflag) {
        LOGD(TAG, "info->block_size:%d", mmc->info.block_size);
        LOGD(TAG, "info->erase_blks:%d", mmc->info.erase_blks);
        LOGD(TAG, "info->boot_area_blks:%d", mmc->info.boot_area_blks);
        LOGD(TAG, "info->user_area_blks:%d", mmc->info.user_area_blks);
        iprintflag = 1;
    }
#endif
    return 0;
}

static int _mmc_close(rvm_dev_t *dev)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc)
        return -EINVAL;
    MMC_Deinit(&mmc->SDIO_MMCCard);

    return 0;
}

static int _mmc_read(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc || !buffer)
        return -EINVAL;
    if (block_cnt == 0)
        return 0;
    // LOGD(TAG, "start_block:%d, block_cnt:%d", start_block, block_cnt);
    int32_t ret = MMC_ReadBlocks(&mmc->SDIO_MMCCard, buffer, start_block, block_cnt);
    if (kStatus_Success != ret) {
        LOGE(TAG, "read start_block:%d, block_cnt:%d, ret:%d", start_block, block_cnt, ret);
        return -EIO;
    }

    return 0;
}

static int _mmc_write(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc || !buffer)
        return -EINVAL;
    if (block_cnt == 0)
        return 0;
    // LOGD(TAG, "start_block:%d, block_cnt:%d", start_block, block_cnt);
    int32_t ret = MMC_WriteBlocks(&mmc->SDIO_MMCCard, buffer, start_block, block_cnt);
    if (kStatus_Success != ret) {
        LOGE(TAG, "write start_block:%d, block_cnt:%d, ret:%d", start_block, block_cnt, ret);
        return -EIO;
    }

    return 0;
}

static int _mmc_erase(rvm_dev_t *dev, uint32_t start_block, uint32_t block_cnt)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc)
        return -EINVAL;
    if (block_cnt == 0)
        return 0;

    uint32_t startGroup, endGroup;
    if (start_block % mmc->SDIO_MMCCard.eraseGroupBlocks) {
        LOGE(TAG, "start_block is not align.");
        return -EINVAL;
    }
    if (block_cnt % mmc->SDIO_MMCCard.eraseGroupBlocks) {
        LOGE(TAG, "block_cnt is not align.");
        return -EINVAL;
    }
    startGroup = start_block / mmc->SDIO_MMCCard.eraseGroupBlocks;
    endGroup = startGroup + block_cnt / mmc->SDIO_MMCCard.eraseGroupBlocks - 1;
    // LOGD(TAG, "startGroup:%d, endGroup:%d", startGroup, endGroup);
    int32_t ret = MMC_EraseGroups(&mmc->SDIO_MMCCard, startGroup, endGroup);
    if (kStatus_Success != ret) {
        LOGE(TAG, "erase startGroup:%d, endGroup:%d, ret:%d", startGroup, endGroup, ret);
        return -EIO;
    }
    return 0;
}

static int _mmc_current_area(rvm_dev_t *dev, rvm_hal_mmc_access_area_t *area)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc || !area)
        return -EINVAL;
    *area = mmc->SDIO_MMCCard.currentPartition;
    return 0;
}

static int _mmc_select_area(rvm_dev_t *dev, rvm_hal_mmc_access_area_t area)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc)
        return -EINVAL;
    LOGD(TAG, "select area to %d", area);
    if (kStatus_Success != MMC_SelectPartition(&mmc->SDIO_MMCCard, area)) {
        LOGE(TAG, "MMC_SelectPartition %d e.", area);
        return -EIO;
    }
    return 0;
}

static int _mmc_get_info(rvm_dev_t *dev, rvm_hal_blockdev_info_t *info)
{
    mmc_dev_t *mmc = (mmc_dev_t*)dev;

    if (!mmc || !info)
        return -EINVAL;

    info->block_size = mmc->info.block_size;
    info->erase_blks = mmc->info.erase_blks;
    info->boot_area_blks = mmc->info.boot_area_blks;
    info->user_area_blks = mmc->info.user_area_blks;

    return 0;
}

static blockdev_driver_t mmc_driver = {
    .drv = {
        .name   = "mmc",
        .init   = _mmc_init,
        .uninit = _mmc_uninit,
        .open   = _mmc_open,
        .close  = _mmc_close,
    },
    .read_blks      = _mmc_read,
    .write_blks     = _mmc_write,
    .erase_blks     = _mmc_erase,
    .current_area   = _mmc_current_area,
    .select_area    = _mmc_select_area,
    .get_info       = _mmc_get_info,
};

void rvm_mmc_drv_register(int idx, rvm_hal_mmc_config_t *config)
{
    rvm_driver_register(&mmc_driver.drv, config, idx);
}
#endif /*CONFIG_COMP_SDMMC*/
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
#if defined(CONFIG_COMP_TINY_BBM) && CONFIG_COMP_TINY_BBM
#include <soc.h>
#include <board.h>
#include <ulog/ulog.h>
#include <drv/spinand.h>
#include <devices/impl/flash_impl.h>
#include <tiny_bbm.h>

#define TAG "spinand_drv"

typedef struct {
    rvm_dev_t                   device;
    csi_spinand_t               handle;
    csi_spinand_dev_params_t    info;
    nand_bbm_t                  *nand_bbm;
} flash_dev_t;

static rvm_dev_t *yoc_spinand_init(driver_t *drv,void *config, int id)
{
    flash_dev_t *dev = (flash_dev_t*)rvm_hal_device_new(drv, sizeof(flash_dev_t), id);

    return (rvm_dev_t*)dev;
}

#define yoc_spinand_uninit rvm_hal_device_free

static int yoc_spinand_open(rvm_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    csi_error_t ret = csi_spinand_qspi_init(&flash->handle, dev->id, NULL);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_spinand_qspi_init failed, start to try spiflash!");
        return -1;
    }
    if (csi_spinand_get_flash_info(&flash->handle, &flash->info)) {
        LOGE(TAG, "csi_spinand_get_flash_info error.");
        return -1;
    }
    flash->nand_bbm = nand_bbm_init(&flash->handle, &flash->info);
    if (!flash->nand_bbm) {
        LOGE(TAG, "nand bbm init failed.");
        return -1;
    }
    return 0;
}

static int yoc_spinand_close(rvm_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    csi_spinand_qspi_uninit(&flash->handle);
    nand_bbm_deinit(flash->nand_bbm);

    return 0;
}

static int yoc_spinand_read(rvm_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    // LOGD(TAG, "addroff:0x%x, bytesize:%d", addroff, bytesize);
    int ret = nandflash_read(flash->nand_bbm, addroff, buff, (size_t)bytesize);
    if (ret != 0) {
        LOGE(TAG, "read addroff:0x%x, bytesize:%d", addroff, bytesize);
        return -EIO;
    }

    return 0;
}

static int yoc_spinand_program(rvm_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    // LOGD(TAG, "dstaddr:0x%x, bytesize:%d", dstaddr, bytesize);
    int ret = nandflash_write(flash->nand_bbm, dstaddr, srcbuf, (size_t)bytesize);
    if (ret != 0) {
        LOGE(TAG, "write dstaddr:0x%x, bytesize:%d", dstaddr, bytesize);
        return -EIO;
    }

    return 0;
}

static int yoc_spinand_erase(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    // LOGD(TAG, "addroff:0x%x, blkcnt:%d", addroff, blkcnt);
    csi_spinand_dev_params_t *dev_info = &flash->info;
    uint32_t erase_size = dev_info->page_size * dev_info->pages_per_block;
    int ret = nandflash_erase(flash->nand_bbm, addroff, blkcnt * erase_size);
    if (ret != 0) {
        LOGE(TAG, "erase addroff:0x%x, blkcnt:%d", addroff, blkcnt);
        return -EIO;
    }
    return 0;
}

static int yoc_spinand_get_info(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    csi_spinand_dev_params_t flash_info;
    csi_error_t ret = csi_spinand_get_flash_info(&flash->handle, &flash_info);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_spinand_get_flash_info failed.");
    	return -1;
    }

    info->page_size = flash_info.page_size;
    info->oob_size = flash_info.oob_size;
    info->block_size = flash_info.page_size * flash_info.pages_per_block;
    info->max_bad_blocks = flash_info.max_bad_blocks;
    info->total_blocks = flash_info.total_blocks;
    info->start_addr = 0;
    info->sector_size = info->block_size;
    info->sector_count = info->total_blocks;
    return 0;
}

static flash_driver_t spinand_driver = {
    .drv = {
        .name   = "spinand",
        .type   = "flash",
        .init   = yoc_spinand_init,
        .uninit = yoc_spinand_uninit,
        .open   = yoc_spinand_open,
        .close  = yoc_spinand_close,
    },
    .read       = yoc_spinand_read,
    .program    = yoc_spinand_program,
    .erase      = yoc_spinand_erase,
    .get_info   = yoc_spinand_get_info,
};

void rvm_spinandflash_drv_register(int idx)
{
    rvm_driver_register(&spinand_driver.drv, NULL, idx);
}
#endif
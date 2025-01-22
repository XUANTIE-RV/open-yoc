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
#include <soc.h>
#include <board.h>
#include <ulog/ulog.h>
#include <drv/spiflash.h>
#include <devices/impl/flash_impl.h>

#define TAG "spiflash_drv"

typedef struct {
    rvm_dev_t           device;
    csi_spiflash_t      handle;
    csi_spiflash_info_t info;
} flash_dev_t;

static rvm_dev_t *yoc_spiflash_init(driver_t *drv,void *config, int id)
{
    flash_dev_t *dev = (flash_dev_t*)rvm_hal_device_new(drv, sizeof(flash_dev_t), id);

    return (rvm_dev_t*)dev;
}

#define yoc_spiflash_uninit rvm_hal_device_free

static int yoc_spiflash_lpm(rvm_dev_t *dev, int state)
{
    //flash_dev_t *flash = (flash_dev_t*)dev;

    if (state) {
     ; //  csi_spiflash_power_down(flash);
    } else {
     ; //  csi_spiflash_release_power_down(flash);
    }
    return 0;
}

static int yoc_spiflash_open(rvm_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    csi_error_t ret = csi_spiflash_qspi_init(&flash->handle, dev->id, NULL);
    if (ret != CSI_OK) {
        LOGW(TAG, "qspi_flash init failed, start to try spiflash!");
        ret = csi_spiflash_spi_init(&flash->handle, dev->id, NULL);
        if (ret != CSI_OK) {
            LOGW(TAG, "spi init failed, maybe no flash in board!");
            return -1;
        }
    }
    if (csi_spiflash_get_flash_info(&flash->handle, &flash->info)) {
        LOGE(TAG, "csi get flash info error.");
        return -1;
    }
    return 0;
}

static int yoc_spiflash_close(rvm_dev_t *dev)
{
    //flash_dev_t *flash = (flash_dev_t*)dev;

    //csi_spiflash_qspi_uninit(&flash->handle);

    return 0;
}

static int yoc_spiflash_clock(rvm_dev_t *dev, bool enable)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    if (enable) {
        csi_clk_enable(&flash->handle.spi_qspi.spi.dev);
    } else {
        csi_clk_disable(&flash->handle.spi_qspi.spi.dev);
    }
    return 0;
}

static int yoc_spiflash_read(rvm_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret;
    // LOGD(TAG, "addroff:0x%x, bytesize:%d", addroff, bytesize);
    ret = csi_spiflash_read(&flash->handle, addroff, buff, bytesize);
    if (ret != bytesize) {
        LOGE(TAG, "addroff:0x%x, bytesize:%d", addroff, bytesize);
        return -EIO;
    }

    return 0;
}

static int yoc_spiflash_program(rvm_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    // LOGD(TAG, "dstaddr:0x%x, bytesize:%d", dstaddr, bytesize);
    int ret = csi_spiflash_program(&flash->handle, dstaddr, srcbuf, bytesize);
    if (ret != bytesize) {
        LOGE(TAG, "dstaddr:0x%x, bytesize:%d", dstaddr, bytesize);
        return -EIO;
    }

    return 0;
}

static int yoc_spiflash_erase(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret = -EIO;
    int i;
    // LOGD(TAG, "addroff:0x%x, blkcnt:%d", addroff, blkcnt);
    for (i = 0; i < blkcnt; i++) {
        ret = csi_spiflash_erase(&flash->handle, addroff + i * flash->info.sector_size, flash->info.sector_size);
        if (ret != 0) {
            break;
        }
    }

    return ret < 0 ? -EIO : 0;
}

static int yoc_spiflash_get_info(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    csi_spiflash_info_t flash_info;
    csi_error_t ret = csi_spiflash_get_flash_info(&flash->handle, &flash_info);
    if (ret != CSI_OK) {
    	return -1;
    }

    info->start_addr = flash_info.xip_addr;
    info->sector_size = flash_info.sector_size;
    info->sector_count = flash_info.flash_size / flash_info.sector_size;
    info->block_size = 0; // FIXME:
    info->page_size = 0;
    info->oob_size = 0;
    info->max_bad_blocks = 0;
    info->total_blocks = 0;

    return 0;
}

static flash_driver_t flash_driver = {
    .drv = {
        .name   = "flash",
        .type   = "flash",
        .init   = yoc_spiflash_init,
        .uninit = yoc_spiflash_uninit,
        .lpm    = yoc_spiflash_lpm,
        .open   = yoc_spiflash_open,
        .close  = yoc_spiflash_close,
        .clk_en = yoc_spiflash_clock
    },
    .read       = yoc_spiflash_read,
    .program    = yoc_spiflash_program,
    .erase      = yoc_spiflash_erase,
    .get_info   = yoc_spiflash_get_info,
};

void rvm_spiflash_drv_register(int idx)
{
    rvm_driver_register(&flash_driver.drv, NULL, idx);
}

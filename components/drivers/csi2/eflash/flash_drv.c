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

#include "hal/flash_impl.h"
#include "drv/eflash.h"

typedef struct {
    rvm_dev_t           device;
    eflash_handle_t handle;
    eflash_info_t  *info;
} flash_dev_t;

static rvm_dev_t *yoc_eflash_init(driver_t *drv,void *config, int id)
{
    flash_dev_t *dev = (flash_dev_t*)rvm_hal_device_new(drv, sizeof(flash_dev_t), id);

    return (rvm_dev_t*)dev;
}

#define yoc_eflash_uninit rvm_hal_device_free

static int yoc_eflash_lpm(rvm_dev_t *dev, int state)
{

    return 0;
}

static int yoc_eflash_open(rvm_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    flash->handle = csi_eflash_initialize(dev->id, NULL);

    if (flash->handle == NULL) {
        return -1;
    }

    flash->info = csi_eflash_get_info(flash->handle);

    return 0;
}

static int yoc_eflash_close(rvm_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    csi_eflash_uninitialize(flash->handle);

    return 0;
}

static int yoc_eflash_clock(rvm_dev_t *dev, bool enable)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    if (enable) {
        csi_clk_enable(&flash->handle.dev);
    } else {
        csi_clk_disable(&flash->handle.dev);
    }
    return 0;
}

static int yoc_eflash_read(rvm_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret;

    ret = csi_eflash_read(flash->handle, flash->info->start + addroff, buff, bytesize);

    return ret < 0 ? -EIO : 0;
}

static int yoc_eflash_program(rvm_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    int ret = csi_eflash_program(flash->handle, flash->info->start + dstaddr, srcbuf, bytesize);
#if 0
    uint8_t *writeaddr = (uint8_t *)dstaddr;
    uint8_t ro_num = 0;
    int ret = 0;
    uint8_t *p;
    uint32_t *mbuf;
    uint32_t block_num, blk_size;
    uint32_t blk_inoff, blk_off;
    uint32_t i;
    uint8_t *src_buf = (uint8_t *)srcbuf;
    writeaddr = (uint8_t *)(dstaddr & 0xfffffffc);

    ro_num = dstaddr & 0x3;
    blk_size = flash->info->sector_size;

    mbuf = aos_malloc(blk_size);

    if (!mbuf) {
        return -ENOMEM;
    }

    p = (uint8_t *)mbuf;
    int start_addr = flash->info->start;
    blk_off = ((uint32_t)(dstaddr + start_addr) & (0 - blk_size));
    blk_inoff = ((uint32_t)(dstaddr + start_addr) & (blk_size - 1));

    // write first block
    if (blk_inoff) {
        ret = csi_eflash_read(flash->handle, (uint32_t)blk_off, p, blk_size);

        if (ret != blk_size) {
            goto fail;
        }

        for (i = blk_inoff; i < blk_size; i++) {
            *(p + i) = *(src_buf);
            bytesize--;
            src_buf++;

            if (bytesize == 0) {
                break;
            }
        }

        if (ro_num) {
            ret = csi_eflash_erase_sector(flash->handle, blk_off);

            if (ret < 0) {
                goto fail;
            }

            ret = csi_eflash_program(flash->handle, (uint32_t)blk_off, p, blk_size);

            if (ret != blk_size) {
                goto fail;
            }
        } else {
            ret = csi_eflash_program(flash->handle, (uint32_t)(blk_off + blk_inoff), p + blk_inoff, blk_size - blk_inoff);

            if (ret != (blk_size - blk_inoff)) {
                goto fail;
            }
        }

        writeaddr += (blk_size - blk_inoff);
    }

    // write mult-block
    block_num = bytesize / blk_size;

    for (i = 0; i < block_num; i++) {

        memcpy(p, src_buf, blk_size);
        ret = csi_eflash_program(flash->handle, (uint32_t)writeaddr + start_addr, (void *)p, blk_size);

        if (ret != blk_size) {
            goto fail;
        }

        src_buf += blk_size;
        writeaddr += blk_size;

        bytesize -= blk_size;
    }

    // write last block in case of bytesize must be less than blocksize
    if (bytesize != 0) {
        ro_num = bytesize & 0x3;
        if (ro_num) {
            memcpy(p, src_buf, bytesize);
            memset(p + bytesize, 0xff, 0x4 - ro_num);
            bytesize += 0x4 - ro_num;
        } else {
            memcpy(p, src_buf, bytesize);
        }

        ret = csi_eflash_program(flash->handle, (uint32_t)writeaddr + start_addr, (void *)p, bytesize);

        if (ret != bytesize) {
            goto fail;
        }
    }

fail:

    if (mbuf) {
        aos_free(mbuf);
    }
#endif
    return ret < 0 ? -EIO : 0;
}

static int yoc_eflash_erase(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret = -EIO;
    int i;

    for (i = 0; i < blkcnt; i++) {
        ret = csi_eflash_erase_sector(flash->handle, flash->info->start + addroff + i * flash->info->sector_size);
        if (ret != 0) {
            break;
        }
    }

    return ret < 0 ? -EIO : 0;
}

static int yoc_eflash_get_info(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    info->start_addr = flash->info->start;
    info->block_size = flash->info->sector_size;
    info->block_count = (flash->info->end - flash->info->start + 1) / flash->info->sector_size;

    return 0;
}

static flash_driver_t flash_driver = {
    .drv = {
        .name   = "eflash",
        .type   = "flash",
        .init   = yoc_eflash_init,
        .uninit = yoc_eflash_uninit,
        .lpm    = yoc_eflash_lpm,
        .open   = yoc_eflash_open,
        .close  = yoc_eflash_close,
        .clk_en = yoc_eflash_clock
    },
    .read       = yoc_eflash_read,
    .program    = yoc_eflash_program,
    .erase      = yoc_eflash_erase,
    .get_info   = yoc_eflash_get_info,
};

void rvm_eflash_drv_register(int idx)
{
    rvm_driver_register(&flash_driver.drv, NULL, idx);
}

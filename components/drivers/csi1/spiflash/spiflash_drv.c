/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "hal/flash_impl.h"

#include "drv/spiflash.h"

typedef struct {
    aos_dev_t           device;
    spiflash_handle_t handle;
    spiflash_info_t  *info;
} flash_dev_t;

static aos_dev_t *yoc_spiflash_init(driver_t *drv,void *config, int id)
{
    flash_dev_t *dev = (flash_dev_t*)device_new(drv, sizeof(flash_dev_t), id);

    return (aos_dev_t*)dev;
}

#define yoc_spiflash_uninit device_free

static int yoc_spiflash_lpm(aos_dev_t *dev, int state)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    if (state) {
        csi_spiflash_power_down(flash);
    } else {
        csi_spiflash_release_power_down(flash);
    }
    return 0;
}

static int yoc_spiflash_open(aos_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    flash->handle = csi_spiflash_initialize(dev->id, NULL);

    if (flash->handle == NULL) {
        // aos_free(dev);
        return -1;
    }

    flash->info = csi_spiflash_get_info(flash->handle);

    return 0;
}

static int yoc_spiflash_close(aos_dev_t *dev)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    csi_spiflash_uninitialize(flash->handle);

    return 0;
}

static int yoc_spiflash_read(aos_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret;

    ret = csi_spiflash_read(flash->handle, flash->info->start + addroff, buff, bytesize);

    return ret < 0 ? -EIO : 0;
}

static int yoc_spiflash_program(aos_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize)
{
    flash_dev_t *flash = (flash_dev_t*)dev;

    int ret = csi_spiflash_program(flash->handle, flash->info->start + dstaddr, srcbuf, bytesize);
    
    return ret < 0 ? -EIO : 0;
}

#if defined(CONFIG_SPIFLASH_BLOCK_ERASE) && CONFIG_SPIFLASH_BLOCK_ERASE
static int yoc_spiflash_block_erase(aos_dev_t *dev, int32_t addroff, int32_t sector_cnt)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret = -EIO;
    int left_sector_count = sector_cnt;

    int FLASH_BLOCK_SIZE = flash->info->block_size; /*get erase block size */
    int BLKERASE_SECTOR_CNT = FLASH_BLOCK_SIZE / flash->info->sector_size;

    while (left_sector_count > 0) {

        int flash_addr = (sector_cnt - left_sector_count) * flash->info->sector_size + addroff + flash->info->start;

        if ( (flash_addr % FLASH_BLOCK_SIZE == 0) && (left_sector_count >= BLKERASE_SECTOR_CNT) ) {
            ret = csi_spiflash_erase_block(flash->handle, flash_addr);
            left_sector_count -= BLKERASE_SECTOR_CNT;
            //printf("Erase block 0x%x\n", flash->info->start + flash_addr);
        } else {
            ret = csi_spiflash_erase_sector(flash->handle, flash_addr);
            left_sector_count --;
            //printf("Erase sector 0x%x\n", flash->info->start + flash_addr);
        }

        if (ret) {
            return -EIO;
        }
    }
    return 0;
}
#else
static int yoc_spiflash_block_erase(aos_dev_t *dev, int32_t addroff, int32_t sector_cnt)
{
    printf("Error: block erase is not supported!\n");
    return -1;
}
#endif

static int yoc_spiflash_erase(aos_dev_t *dev, int32_t addroff, int32_t sector_cnt)
{
    flash_dev_t *flash = (flash_dev_t*)dev;
    int ret = -EIO;
    int i;

    /* check block erase support */
    if (flash->info->block_size) {
        ret = yoc_spiflash_block_erase(dev, addroff, sector_cnt);
        return ret;
    }

    /* sector erase */
    for (i = 0; i < sector_cnt; i++) {
        ret = csi_spiflash_erase_sector(flash->handle, flash->info->start + addroff + i * flash->info->sector_size);
        if (ret != 0) {
            break;
        }
    }

    return ret < 0 ? -EIO : 0;
}

static int yoc_spiflash_get_info(aos_dev_t *dev, flash_dev_info_t *info)
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
        .init   = yoc_spiflash_init,
        .uninit = yoc_spiflash_uninit,
        .lpm    = yoc_spiflash_lpm,
        .open   = yoc_spiflash_open,
        .close  = yoc_spiflash_close,
    },
    .read       = yoc_spiflash_read,
    .program    = yoc_spiflash_program,
    .erase      = yoc_spiflash_erase,
    .get_info   = yoc_spiflash_get_info,
};

void spiflash_csky_register(int idx)
{
    driver_register(&flash_driver.drv, NULL, idx);
}

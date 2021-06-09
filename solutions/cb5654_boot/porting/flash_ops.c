/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <drv/spiflash.h>
#include <yoc/partition_flash.h>

static spiflash_handle_t spiflash_hd;
static spiflash_info_t *spiflash_info;

static int _boot_flash_info_get(void *handle, partition_flash_info_t *info)
{
    if (info != NULL) {
        spiflash_info = csi_spiflash_get_info(spiflash_hd);
        info->start_addr = spiflash_info->start;
        info->sector_size = spiflash_info->sector_size;
        info->sector_count = spiflash_info->sector_count;
        return 0;
    }
    return -1;
}

static int _boot_flash_read(void *handle, uint32_t addr, void *data, size_t data_len)
{
    int ret = csi_spiflash_read(spiflash_hd, addr, data, data_len);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int _boot_flash_write(void *handle, uint32_t addr, void *data, size_t data_len)
{
    if (csi_spiflash_program(spiflash_hd, addr, data, data_len) < 0) {
        return -1;
    }
    return 0; 
}

static int _boot_flash_erase(void *handle, uint32_t addr, uint32_t len)
{
    uint32_t i = 0;
    uint32_t sector;
    int blkcnt;

    sector = spiflash_info->sector_size;
    blkcnt = (len + sector - 1) / sector;
    for (; i < blkcnt; i++) {
        if (csi_spiflash_erase_sector(spiflash_hd, addr)) {
            return -1;
        }

        addr += sector;
    }
    return 0;
}

static partition_flash_ops_t g_flash_ops = {
    .open = NULL,
    .close = NULL,
    .info_get = _boot_flash_info_get,
    .read = _boot_flash_read,
    .write = _boot_flash_write,
    .erase = _boot_flash_erase
};

int boot_flash_init(void)
{
    spiflash_hd = csi_spiflash_initialize(0, NULL);

    partition_flash_register(&g_flash_ops);

    return 0;
}
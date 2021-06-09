/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <drv/spiflash.h>
#include <yoc/partition_flash.h>

static csi_spiflash_t spiflash_hd;
static csi_spiflash_info_t spiflash_info;

static int _boot_flash_info_get(void *handle, partition_flash_info_t *info)
{
    if (info != NULL) {
        csi_error_t ret = csi_spiflash_get_flash_info(&spiflash_hd, &spiflash_info);
        if (ret != CSI_OK) {
            return -1;
        }
        info->start_addr = spiflash_info.xip_addr;
        info->sector_size = spiflash_info.sector_size;
        info->sector_count = spiflash_info.flash_size / spiflash_info.sector_size;
        return 0;
    }
    return -1;
}

static int _boot_flash_read(void *handle, uint32_t addr, void *data, size_t data_len)
{
    int ret = csi_spiflash_read(&spiflash_hd, addr - spiflash_info.xip_addr, data, data_len);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int _boot_flash_write(void *handle, uint32_t addr, void *data, size_t data_len)
{
    int ret = csi_spiflash_program(&spiflash_hd, addr - spiflash_info.xip_addr, data, data_len);
    if ( ret < 0) {
        return -1;
    }
    return 0; 
}

static int _boot_flash_erase(void *handle, uint32_t addr, size_t len)
{
    csi_error_t ret;

    if (len % spiflash_info.sector_size) {
        len = (len / spiflash_info.sector_size + 1) * spiflash_info.sector_size;
    }
    ret = csi_spiflash_erase(&spiflash_hd, addr, len);
    if (ret) {
        return ret;
    }

    return 0;
}

static const partition_flash_ops_t g_flash_ops = {
    .open = NULL,
    .close = NULL,
    .info_get = _boot_flash_info_get,
    .read = _boot_flash_read,
    .write = _boot_flash_write,
    .erase = _boot_flash_erase
};

int boot_flash_init(void)
{
    csi_spiflash_qspi_init(&spiflash_hd, 0, NULL);
    partition_flash_register((void *)&g_flash_ops);
    return 0;
}

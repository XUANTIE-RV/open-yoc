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
            printf("csi_spiflash_get_flash_info failed,ret:%d\n", ret);
            return -1;
        }
        info->start_addr = spiflash_info.xip_addr;
        info->sector_size = spiflash_info.sector_size;
        info->sector_count = spiflash_info.flash_size / spiflash_info.sector_size;
        // printf("info->start_addr:0x%x\n", info->start_addr);
        // printf("info->sector_size:0x%x\n", info->sector_size);
        // printf("info->sector_count:0x%x\n", info->sector_count);
        return 0;
    }
    return -1;
}

static int _boot_flash_read(void *handle, unsigned long addr, void *data, size_t data_len)
{
    // printf("%s, %d, 0x%x\n", __func__, __LINE__, addr);
    int ret = csi_spiflash_read(&spiflash_hd, addr - spiflash_info.xip_addr, data, data_len);
    // printf("%s, %d, 0x%x, ret:%d\n", __func__, __LINE__, addr, ret);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int _boot_flash_write(void *handle, unsigned long addr, void *data, size_t data_len)
{
    int ret = csi_spiflash_program(&spiflash_hd, addr - spiflash_info.xip_addr, data, data_len);
    if ( ret < 0) {
        return -1;
    }
    return 0; 
}

static int _boot_flash_erase(void *handle, unsigned long addr, size_t len)
{
    if (csi_spiflash_erase(&spiflash_hd, addr, len)) {
        return -1;
    }

    return 0;
}

int boot_flash_init(void)
{
    partition_flash_ops_t flash_ops = {
        .open = NULL,
        .close = NULL,
        .info_get = _boot_flash_info_get,
        .read = _boot_flash_read,
        .write = _boot_flash_write,
        .erase = _boot_flash_erase
    };
    csi_error_t ret = csi_spiflash_qspi_init(&spiflash_hd, 0, NULL);
    if (ret != 0) {
        printf("csi flash init failed.\n");
        return -1;
    }
    partition_flash_register(&flash_ops);
    return 0;
}

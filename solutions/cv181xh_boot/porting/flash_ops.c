/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/partition.h>
#if CONFIG_PARTITION_SUPPORT_SPINORFLASH
#include <drv/spiflash.h>
#include <yoc/partition_device.h>

#define DGB_PRINT(...) printf(__VA_ARGS__)

static csi_spiflash_t spiflash_hd;
static csi_spiflash_info_t spiflash_info;

static void *_boot_flash_find(int device_id)
{
    csi_error_t ret = csi_spiflash_spi_init(&spiflash_hd, device_id, NULL);
    if (ret != 0) {
        printf("csi flash init failed.\n");
        return NULL;
    }
    return &spiflash_hd;
}

static int _boot_flash_info_get(void *handle, partition_device_info_t *info)
{
    if (handle && info != NULL) {
        csi_error_t ret = csi_spiflash_get_flash_info(handle, &spiflash_info);
        if (ret != CSI_OK) {
            printf("csi_spiflash_get_flash_info failed,ret:%d\n", ret);
            return -1;
        }
        info->base_addr = spiflash_info.xip_addr;
        info->sector_size = spiflash_info.sector_size;
        info->block_size = 0;
        info->device_size = spiflash_info.flash_size;
        info->erase_size = spiflash_info.sector_size;
        static int iprintflag = 0;
        if (!iprintflag) {
            DGB_PRINT("%s, %d\n", __func__, __LINE__);
            DGB_PRINT("info->base_addr:0x%lx\n", info->base_addr);
            DGB_PRINT("info->sector_size:0x%x\n", info->sector_size);
            DGB_PRINT("info->block_size:0x%x\n", info->block_size);
            DGB_PRINT("info->device_size:0x%lx\n", info->device_size);
            DGB_PRINT("info->erase_size:0x%x\n", info->erase_size);
            iprintflag = 1;
        }
        return 0;
    }
    return -1;
}

static int _boot_flash_read(void *handle, off_t offset, void *data, size_t data_len)
{
    // DGB_PRINT("%s, %d, 0x%x\n", __func__, __LINE__, offset);
    int ret = csi_spiflash_read(handle, offset, data, data_len);
    // DGB_PRINT("%s, %d, 0x%x, ret:%d\n", __func__, __LINE__, offset, ret);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int _boot_flash_write(void *handle, off_t offset, void *data, size_t data_len)
{
    // DGB_PRINT("%s, %d, 0x%x\n", __func__, __LINE__, offset);
    int ret = csi_spiflash_program(handle, offset, data, data_len);
    // DGB_PRINT("%s, %d, 0x%x, ret:%d\n", __func__, __LINE__, offset, ret);
    if ( ret < 0) {
        return -1;
    }
    return 0; 
}

static int _boot_flash_erase(void *handle, off_t offset, size_t len)
{
    // DGB_PRINT("%s, %d, 0x%x\n", __func__, __LINE__, offset);
    int ret = csi_spiflash_erase(handle, offset, len);
    // DGB_PRINT("%s, %d, 0x%x, ret:%d\n", __func__, __LINE__, offset, ret);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

static partition_device_ops_t boot_flash_ops = {
    .storage_info.id      = 0,
    .storage_info.type    = MEM_DEVICE_TYPE_SPI_NOR_FLASH,
    .find           = _boot_flash_find,
    .info_get       = _boot_flash_info_get,
    .read           = _boot_flash_read,
    .write          = _boot_flash_write,
    .erase          = _boot_flash_erase
};

int partition_flash_register(void)
{
    DGB_PRINT("%s, %d\n", __func__, __LINE__);
    return partition_device_register(&boot_flash_ops);
}
#endif
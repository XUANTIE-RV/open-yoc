/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/partition.h>
#if CONFIG_PARTITION_SUPPORT_SPINANDFLASH
#include <errno.h>
#include <drv/spinand.h>
#include <yoc/partition_device.h>
#include <tiny_bbm.h>

#define DGB_PRINT(fmt, ...) //printf(fmt, ##__VA_ARGS__)

#define LOGD(mod, fmt, ...) printf("[bbm][D][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGI(mod, fmt, ...) printf("[bbm][I][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGW(mod, fmt, ...) printf("[bbm][W][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGE(mod, fmt, ...) printf("[bbm][E][%s,%d] "fmt"\r\n", __func__, __LINE__, ##__VA_ARGS__)

#define TAG "spinand"

static int g_nand_init_ok;
static csi_spinand_t spinand_handle;
static csi_spinand_dev_params_t spinand_dev_info;
static partition_device_info_t g_nandflash_info;
static nand_bbm_t *g_nand_bbm;

static void *_boot_flash_find(int device_id)
{
    // LOGD(TAG, "nand flash find.");
    if (!g_nand_init_ok) {
        if (csi_spinand_qspi_init(&spinand_handle, device_id, NULL) != CSI_OK) {
            LOGE(TAG, "qspi nandflash init failed");
            return NULL;
        }
        LOGD(TAG, "qspi_nandflash_init ok.");
        if (csi_spinand_get_flash_info(&spinand_handle, &spinand_dev_info) != CSI_OK) {
            LOGE(TAG, "qspi nandflash get info e");
            return NULL;
        }
        LOGD(TAG, "*********************************************");
        LOGD(TAG, "flash_info.model_name = %s", spinand_dev_info.model_name);
        LOGD(TAG, "flash_info.page_size = 0x%x", spinand_dev_info.page_size);
        LOGD(TAG, "flash_info.oob_size = 0x%x", spinand_dev_info.oob_size);
        LOGD(TAG, "flash_info.pages_per_block = %d", spinand_dev_info.pages_per_block);
        LOGD(TAG, "flash_info.max_bad_blocks = %d", spinand_dev_info.max_bad_blocks);
        LOGD(TAG, "flash_info.total_blocks = %d", spinand_dev_info.total_blocks);
        LOGD(TAG, "********************************************\n");

        g_nandflash_info.base_addr = 0;
        g_nandflash_info.sector_size = 0;
        g_nandflash_info.block_size = (uint32_t)spinand_dev_info.pages_per_block * spinand_dev_info.page_size;
        g_nandflash_info.device_size = (uint64_t)spinand_dev_info.pages_per_block * spinand_dev_info.page_size * spinand_dev_info.total_blocks;
        g_nandflash_info.erase_size = (uint32_t)spinand_dev_info.pages_per_block * spinand_dev_info.page_size;
        LOGD(TAG,"g_nandflash_info.base_addr:0x%lx", g_nandflash_info.base_addr);
        LOGD(TAG,"g_nandflash_info.sector_size:%d", g_nandflash_info.sector_size);
        LOGD(TAG,"g_nandflash_info.block_size:%d", g_nandflash_info.block_size);
        LOGD(TAG,"g_nandflash_info.erase_size:%d", g_nandflash_info.erase_size);
        LOGD(TAG,"g_nandflash_info.device_size:%ld", g_nandflash_info.device_size);

        g_nand_bbm = nand_bbm_init(&spinand_handle, &spinand_dev_info);
        if (!g_nand_bbm) {
            LOGE(TAG, "nand_bbm_init e");
            return NULL;
        }
        g_nand_init_ok = 1;
    }

    return &spinand_handle;
}

static int _boot_flash_info_get(void *handle, partition_device_info_t *info)
{
    if (handle && info != NULL) {
        csi_error_t ret = csi_spinand_get_flash_info(handle, &spinand_dev_info);
        if (ret != CSI_OK) {
            printf("csi_spinand_get_flash_info failed,ret:%d\n", ret);
            return -1;
        }
        info->base_addr = 0;
        info->sector_size = 0;
        info->block_size = (uint32_t)spinand_dev_info.pages_per_block * spinand_dev_info.page_size;
        info->device_size = (uint64_t)spinand_dev_info.pages_per_block * spinand_dev_info.page_size * spinand_dev_info.total_blocks;
        info->erase_size = (uint32_t)spinand_dev_info.pages_per_block * spinand_dev_info.page_size;
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
    int ret;

    DGB_PRINT("%s, %d, 0x%lx, len:0x%lx\n", __func__, __LINE__, offset, data_len);
    if (data == NULL)
        return -EINVAL;
    if (data_len == 0)
        return 0;

    ret = nandflash_read(g_nand_bbm, offset, data, data_len);
    if (ret != 0) {
        printf("nandflash_read %ld bytes error, ret: %d\n", data_len, ret);
    }
    DGB_PRINT("%s, %d, 0x%lx, ret:%d\n", __func__, __LINE__, offset, ret);

    return ret;
}

static int _boot_flash_write(void *handle, off_t offset, void *data, size_t data_len)
{
    int ret;

    DGB_PRINT("%s, %d, 0x%lx, len:0x%lx\n", __func__, __LINE__, offset, data_len);
    if (data == NULL)
        return -EINVAL;
    if (data_len == 0)
        return 0;

    ret = nandflash_write(g_nand_bbm, offset, data, data_len);
    if (ret != 0) {
        printf("nandflash_write %ld bytes error, ret: %d\n", data_len, ret);
    }
    DGB_PRINT("%s, %d, 0x%lx, ret:%d\n", __func__, __LINE__, offset, ret);

    return ret;
}

static int _boot_flash_erase(void *handle, off_t offset, size_t len)
{
    int ret;

    DGB_PRINT("%s, %d, 0x%lx, len:0x%lx\n", __func__, __LINE__, offset, len);
    if (len == 0)
        return 0;

    ret = nandflash_erase(g_nand_bbm, offset, len);
    if (ret != 0) {
        printf("nandflash_erase %ld bytes error, ret: %d\n", len, ret);
    }
    DGB_PRINT("%s, %d, 0x%lx, ret:%d\n", __func__, __LINE__, offset, ret);

    return ret;
}

static partition_device_ops_t boot_nandflash_ops = {
    .storage_info.id      = 0,
    .storage_info.type    = MEM_DEVICE_TYPE_SPI_NAND_FLASH,
    .find           = _boot_flash_find,
    .info_get       = _boot_flash_info_get,
    .read           = _boot_flash_read,
    .write          = _boot_flash_write,
    .erase          = _boot_flash_erase
};

int partition_spinandflash_register(void)
{
    DGB_PRINT("%s, %d\n", __func__, __LINE__);
    return partition_device_register(&boot_nandflash_ops);
}
#endif
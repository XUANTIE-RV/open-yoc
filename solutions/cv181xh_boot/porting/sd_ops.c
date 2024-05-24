/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/partition.h>
#if CONFIG_PARTITION_SUPPORT_SD
#include <mmc.h>
#include <sd.h>
#include <errno.h>
#include <drv/sdif.h>
#include <yoc/partition_device.h>

#define DGB_PRINT(...) //printf(__VA_ARGS__)
#define DGB_PRINTE(...) printf(__VA_ARGS__)

#define SD_BLOCK_SIZE 512

static int g_sd_init_ok;
static uint8_t tmpbuf[SD_BLOCK_SIZE];
static uint8_t offt_tmpbuf[SD_BLOCK_SIZE];
static sd_card_t  SDIO_SDCard;
static partition_device_info_t g_sd_info;

static void *_boot_sd_find(int id)
{
    DGB_PRINT("boot sdmmc find.\n");
    if (g_sd_init_ok == 0) {
        printf("sd init start\n");
        memset(&SDIO_SDCard, 0, sizeof(SDIO_SDCard));
        status_t ret = SD_Init(&SDIO_SDCard, NULL, CONFIG_SD_SDIF);
        if (ret != kStatus_Success) {
            DGB_PRINTE("=====>>>>>>>>>>>>%s, %d=====SD_Init failed\n", __FUNCTION__, __LINE__);
            SD_Deinit(&SDIO_SDCard);
            memset(&SDIO_SDCard, 0, sizeof(SDIO_SDCard));
            return NULL;
        }
        printf("sd init ok.\n");
        g_sd_info.base_addr = 0;
        g_sd_info.sector_size = 0;
        g_sd_info.block_size = SDIO_SDCard.block_size;
        if ((SDIO_SDCard.csd.flags & kSD_CsdEraseBlockEnabledFlag) == kSD_CsdEraseBlockEnabledFlag)
            g_sd_info.erase_size = SDIO_SDCard.block_size;
        else
            g_sd_info.erase_size = (SDIO_SDCard.csd.eraseSectorSize + 1) * SDIO_SDCard.block_size;
        g_sd_info.device_size = (uint64_t)SDIO_SDCard.block_count * SDIO_SDCard.block_size;
        if (g_sd_info.block_size != SD_BLOCK_SIZE) {
            DGB_PRINTE("sd block_size is %d\n", g_sd_info.block_size);
            return NULL;
        }
        printf("g_sd_info.base_addr:0x%lx\n", g_sd_info.base_addr);
        printf("g_sd_info.sector_size:%d\n", g_sd_info.sector_size);
        printf("g_sd_info.block_size:%d\n", g_sd_info.block_size);
        printf("g_sd_info.erase_size:%d\n", g_sd_info.erase_size);
        printf("g_sd_info.user_area_size:%ld\n", g_sd_info.device_size);
        g_sd_init_ok = 1;
    }
    return &SDIO_SDCard;
}

static int _boot_sd_info_get(void *handle, partition_device_info_t *info)
{
    if (handle && info != NULL) {
        info->block_size = g_sd_info.block_size;
        info->erase_size = g_sd_info.erase_size;
        info->device_size = g_sd_info.device_size;
        info->base_addr = 0;
        info->sector_size = 0;
        DGB_PRINT("%s, %d\n", __func__, __LINE__);
        DGB_PRINT("info->base_addr:0x%lx\n", info->base_addr);
        DGB_PRINT("info->sector_size:0x%x\n", info->sector_size);
        DGB_PRINT("info->block_size:0x%x\n", info->block_size);
        DGB_PRINT("info->device_size:0x%lx\n", info->device_size);
        DGB_PRINT("info->erase_size:0x%x\n", info->erase_size);
        return 0;
    }
    DGB_PRINTE("info get arg e.\n");
    return -1;
}

static int _boot_sd_read(void *handle, off_t offset, void *data, size_t data_len)
{
    uint32_t offt_data_left;
    uint32_t left_size, offset_mod;
    uint32_t start_block, block_cnt;

    if (!(handle && data)) {
        DGB_PRINTE("read arg e.\n");
        return -EINVAL;
    }
    if (data_len == 0) {
        return 0;
    }

    DGB_PRINT("%s, %d, offset:0x%x, data_len:0x%lx\n", __func__, __LINE__, offset, data_len);
    offt_data_left = 0;
    offset_mod = offset % g_sd_info.block_size;
    start_block = offset / g_sd_info.block_size;
    if (offset_mod) {
        DGB_PRINT("offset not align block size\n");
        DGB_PRINT("read startblk:%d, blkcount:%d to offt_tmpbuf\n", start_block, 1);
        if (SD_ReadBlocks(handle, offt_tmpbuf, start_block, 1)) {
            DGB_PRINTE("read blks e\n");
            return -1;
        }
        offt_data_left = g_sd_info.block_size - offset_mod;
        if (offt_data_left >= data_len) {
            memcpy(data, (void *)((unsigned long)offt_tmpbuf + offset_mod), data_len);
            DGB_PRINT("read ok, %s, %d\n", __func__, __LINE__);
            return 0;
        }
        memcpy(data, (void *)((unsigned long)offt_tmpbuf + offset_mod), offt_data_left);
        start_block += 1;
        data_len -= offt_data_left;
    }
    left_size = data_len % g_sd_info.block_size;
    block_cnt = data_len / g_sd_info.block_size;
    DGB_PRINT("read startblk:%d, blkcount:%d, left_size:0x%x\n", start_block, block_cnt, left_size);
    if (block_cnt > 0) {
        if (SD_ReadBlocks(handle, (void *)((unsigned long)data + offt_data_left), start_block, block_cnt)) {
            DGB_PRINTE("read blks e\n");
            return -1;
        }
        if (left_size) {
            DGB_PRINT("read left size from blk:%d\n", start_block + block_cnt);
            if (SD_ReadBlocks(handle, tmpbuf, start_block + block_cnt, 1)) {
                DGB_PRINTE("read 1 blk e\n");
                return -1;
            }
            memcpy((void *)((unsigned long)data + offt_data_left + block_cnt * g_sd_info.block_size), tmpbuf, left_size);
        }
    } else {
        if (SD_ReadBlocks(handle, tmpbuf, start_block, 1)) {
            DGB_PRINTE("read 1 blk e\n");
            return -1;
        }
        memcpy((void *)((unsigned long)data + offt_data_left), tmpbuf, left_size);
    }
    DGB_PRINT("read ok, %s, %d\n", __func__, __LINE__);
    return 0;
}

static int _boot_sd_write(void *handle, off_t offset, void *data, size_t data_len)
{
    uint32_t offt_data_left;
    size_t left_size, offset_mod;
    uint32_t start_block, block_cnt;

    if (!(handle && data)) {
        DGB_PRINTE("write arg e.\n");
        return -EINVAL;
    }
    if (data_len == 0) {
        return 0;
    }
    DGB_PRINT("%s, %d, 0x%x, 0x%lx\n", __func__, __LINE__, offset, data_len);
    offt_data_left = 0;
    offset_mod = offset % g_sd_info.block_size;
    start_block = offset / g_sd_info.block_size;
    if (offset_mod) {
        DGB_PRINT("offset not align block size\n");
        DGB_PRINT("read startblk:%d, blkcount:%d to offt_tmpbuf\n", start_block, 1);
        if (SD_ReadBlocks(handle, offt_tmpbuf, start_block, 1)) {
            DGB_PRINTE("read blks e\n");
            return -1;
        }
        offt_data_left = g_sd_info.block_size - offset_mod;
        if (offt_data_left >= data_len) {
            memcpy((void *)((unsigned long)offt_tmpbuf + offset_mod), data, data_len);
        } else {
            memcpy((void *)((unsigned long)offt_tmpbuf + offset_mod), data, offt_data_left);
        }
        if (SD_WriteBlocks(handle, offt_tmpbuf, start_block, 1)) {
            DGB_PRINTE("write blks e\n");
            return -1;
        }
        DGB_PRINT("write ok, %s, %d\n", __func__, __LINE__);
        if (offt_data_left >= data_len) {
            return 0;
        }
        start_block += 1;
        data_len -= offt_data_left;
    }
    left_size = data_len % g_sd_info.block_size;
    block_cnt = data_len / g_sd_info.block_size;
    DGB_PRINT("write startblk:%d, blkcount:%d, left_size:0x%lx\n", start_block, block_cnt, left_size);
    if (block_cnt > 0) {
        if (SD_WriteBlocks(handle, (uint8_t *)data + offt_data_left, start_block, block_cnt)) {
            DGB_PRINTE("write blks e\n");
            return -1;
        }
    }
    if (left_size) {
        DGB_PRINT("read 1 block from blk:%d\n", start_block + block_cnt);
        if (SD_ReadBlocks(handle, tmpbuf, start_block + block_cnt, 1)) {
            DGB_PRINTE("read 1 blk e\n");
            return -1;
        }
        memcpy(tmpbuf, (void *)((unsigned long)data + offt_data_left + block_cnt * g_sd_info.block_size), left_size);
        DGB_PRINT("write left_size to blk:%d\n", start_block + block_cnt);
        if (SD_WriteBlocks(handle, tmpbuf, start_block + block_cnt, 1)) {
            DGB_PRINTE("write blks e\n");
            return -1;
        }
    }
    DGB_PRINT("%s, %d\n", __func__, __LINE__);
    return 0;
}

static int _boot_sd_erase(void *handle, off_t offset, size_t len)
{
    if (!(handle)) {
        DGB_PRINTE("erase arg e.\n");
        return -EINVAL;
    }
    if (len == 0) {
        return 0;
    }
    DGB_PRINT("%s, %d, 0x%x, 0x%lx\n", __func__, __LINE__, offset, len);
    if (offset % g_sd_info.erase_size) {
        DGB_PRINTE("offset must align erase_size\n");
        return -EINVAL;
    }
    if (len % g_sd_info.erase_size) {
        printf("len must align erase_size, adjust\n");
        len = (len / g_sd_info.erase_size + 1) * g_sd_info.erase_size;
    }
    if (SD_EraseBlocks(handle, offset / g_sd_info.block_size, len / g_sd_info.block_size)) {
        return -1;
    }
    return 0;
}

static partition_device_ops_t boot_sd_ops = {
    .storage_info.id      = 0,
    .storage_info.type    = MEM_DEVICE_TYPE_SD,
    .find           = _boot_sd_find,
    .info_get       = _boot_sd_info_get,
    .read           = _boot_sd_read,
    .write          = _boot_sd_write,
    .erase          = _boot_sd_erase
};

int partition_sd_register(void)
{
    return partition_device_register(&boot_sd_ops);
}

int partition_usb_register(void)
{
    return 0;
}
#endif
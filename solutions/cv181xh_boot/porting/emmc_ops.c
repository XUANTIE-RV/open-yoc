/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/partition.h>
#if CONFIG_PARTITION_SUPPORT_EMMC
#include <mmc.h>
#include <errno.h>
#include <drv/sdif.h>
#include <yoc/partition_device.h>

#define DGB_PRINT(...) //printf(__VA_ARGS__)
#define DGB_PRINTE(...) printf(__VA_ARGS__)

#define EMMC_BLOCK_SIZE 512

static int g_mmc_init_ok;
static uint8_t tmpbuf[EMMC_BLOCK_SIZE];
static uint8_t offt_tmpbuf[EMMC_BLOCK_SIZE];
static mmc_card_t SDIO_MMCCard;
static partition_device_info_t g_mmc_info;

static void *_boot_emmc_find(int device_id)
{
    DGB_PRINT("boot emmc find.\n");
    if (g_mmc_init_ok == 0) {
        printf("MMC init start\n");
        memset(&SDIO_MMCCard, 0, sizeof(SDIO_MMCCard));
        SDIO_MMCCard.hostVoltageWindowVCC = kMMC_VoltageWindows270to360;
        SDIO_MMCCard.busWidth = kMMC_DataBusWidth4bit;
        status_t ret = MMC_Init(&SDIO_MMCCard, NULL, CONFIG_EMMC_SDIF);
        if (ret != kStatus_Success) {
            DGB_PRINTE("=====>>>>>>>>>>>>%s, %d=====MMC_Init failed\n", __FUNCTION__, __LINE__);
            MMC_Deinit(&SDIO_MMCCard);
            memset(&SDIO_MMCCard, 0U, sizeof(mmc_card_t));
            return NULL;
        }
        printf("MMC init ok\n");
        g_mmc_info.block_size = SDIO_MMCCard.block_size;
        g_mmc_info.erase_size = SDIO_MMCCard.eraseGroupBlocks * SDIO_MMCCard.block_size;
        g_mmc_info.boot_area_size = SDIO_MMCCard.bootPartitionBlocks * SDIO_MMCCard.block_size;
        g_mmc_info.device_size = (uint64_t)SDIO_MMCCard.userPartitionBlocks * SDIO_MMCCard.block_size;
        if (g_mmc_info.block_size != EMMC_BLOCK_SIZE) {
            DGB_PRINTE("mmc block_size is %d\n", g_mmc_info.block_size);
            return NULL;
        }
        if (g_mmc_info.erase_size != EMMC_BLOCK_SIZE * 1024) {
            DGB_PRINTE("mmc erase_size is %d\n", g_mmc_info.erase_size);
        }
        printf("g_mmc_info.base_addr:0x%lx\n", g_mmc_info.base_addr);
        printf("g_mmc_info.sector_size:%d\n", g_mmc_info.sector_size);
        printf("g_mmc_info.block_size:%d\n", g_mmc_info.block_size);
        printf("g_mmc_info.erase_size:%d\n", g_mmc_info.erase_size);
        printf("g_mmc_info.boot_area_size:%d\n", g_mmc_info.boot_area_size);
        printf("g_mmc_info.user_area_size:%ld\n", g_mmc_info.device_size);
        g_mmc_init_ok = 1;
    }

    return &SDIO_MMCCard;
}

static int _boot_emmc_info_get(void *handle, partition_device_info_t *info)
{
    if (handle && info != NULL) {
        info->block_size = g_mmc_info.block_size;
        info->erase_size = g_mmc_info.erase_size;
        info->boot_area_size = g_mmc_info.boot_area_size;
        info->device_size = g_mmc_info.device_size;
        info->base_addr = 0;
        info->sector_size = 0;
        DGB_PRINT("%s, %d\n", __func__, __LINE__);
        DGB_PRINT("info->base_addr:0x%lx\n", info->base_addr);
        DGB_PRINT("info->sector_size:0x%x\n", info->sector_size);
        DGB_PRINT("info->block_size:0x%x\n", info->block_size);
        DGB_PRINT("info->device_size:0x%lx\n", info->device_size);
        DGB_PRINT("info->erase_size:0x%x\n", info->erase_size);
        DGB_PRINT("info->boot_area_size:0x%x\n", info->boot_area_size);
        return 0;
    }
    DGB_PRINTE("info get arg e.\n");
    return -1;
}

static int _boot_emmc_read(void *handle, off_t offset, void *data, size_t data_len)
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
    offset_mod = offset % g_mmc_info.block_size;
    start_block = offset / g_mmc_info.block_size;
    if (offset_mod) {
        DGB_PRINT("offset not align block size\n");
        DGB_PRINT("read startblk:%d, blkcount:%d to offt_tmpbuf\n", start_block, 1);
        if (MMC_ReadBlocks(handle, offt_tmpbuf, start_block, 1)) {
            DGB_PRINTE("read blks e\n");
            return -1;
        }
        offt_data_left = g_mmc_info.block_size - offset_mod;
        if (offt_data_left >= data_len) {
            memcpy(data, (void *)((unsigned long)offt_tmpbuf + offset_mod), data_len);
            DGB_PRINT("read ok, %s, %d\n", __func__, __LINE__);
            return 0;
        }
        memcpy(data, (void *)((unsigned long)offt_tmpbuf + offset_mod), offt_data_left);
        start_block += 1;
        data_len -= offt_data_left;
    }
    left_size = data_len % g_mmc_info.block_size;
    block_cnt = data_len / g_mmc_info.block_size;
    DGB_PRINT("read startblk:%d, blkcount:%d, left_size:0x%x\n", start_block, block_cnt, left_size);
    if (block_cnt > 0) {
        if (MMC_ReadBlocks(handle, (void *)((unsigned long)data + offt_data_left), start_block, block_cnt)) {
            DGB_PRINTE("read blks e\n");
            return -1;
        }
        if (left_size) {
            DGB_PRINT("read left size from blk:%d\n", start_block + block_cnt);
            if (MMC_ReadBlocks(handle, tmpbuf, start_block + block_cnt, 1)) {
                DGB_PRINTE("read 1 blk e\n");
                return -1;
            }
            memcpy((void *)((unsigned long)data + offt_data_left + block_cnt * g_mmc_info.block_size), tmpbuf, left_size);
        }
    } else {
        if (MMC_ReadBlocks(handle, tmpbuf, start_block, 1)) {
            DGB_PRINTE("read 1 blk e\n");
            return -1;
        }
        memcpy((void *)((unsigned long)data + offt_data_left), tmpbuf, left_size);
    }
    DGB_PRINT("read ok, %s, %d\n", __func__, __LINE__);
    return 0;
}

static int _boot_emmc_write(void *handle, off_t offset, void *data, size_t data_len)
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
    offset_mod = offset % g_mmc_info.block_size;
    start_block = offset / g_mmc_info.block_size;
    if (offset_mod) {
        DGB_PRINT("offset not align block size\n");
        DGB_PRINT("read startblk:%d, blkcount:%d to offt_tmpbuf\n", start_block, 1);
        if (MMC_ReadBlocks(handle, offt_tmpbuf, start_block, 1)) {
            DGB_PRINTE("read blks e\n");
            return -1;
        }
        offt_data_left = g_mmc_info.block_size - offset_mod;
        if (offt_data_left >= data_len) {
            memcpy((void *)((unsigned long)offt_tmpbuf + offset_mod), data, data_len);
        } else {
            memcpy((void *)((unsigned long)offt_tmpbuf + offset_mod), data, offt_data_left);
        }
        if (MMC_WriteBlocks(handle, offt_tmpbuf, start_block, 1)) {
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
    left_size = data_len % g_mmc_info.block_size;
    block_cnt = data_len / g_mmc_info.block_size;
    DGB_PRINT("write startblk:%d, blkcount:%d, left_size:0x%lx\n", start_block, block_cnt, left_size);
    if (block_cnt > 0) {
        if (MMC_WriteBlocks(handle, (uint8_t *)data + offt_data_left, start_block, block_cnt)) {
            DGB_PRINTE("write blks e\n");
            return -1;
        }
    }
    if (left_size) {
        DGB_PRINT("read 1 block from blk:%d\n", start_block + block_cnt);
        if (MMC_ReadBlocks(handle, tmpbuf, start_block + block_cnt, 1)) {
            DGB_PRINTE("read 1 blk e\n");
            return -1;
        }
        memcpy(tmpbuf, (void *)((unsigned long)data + offt_data_left + block_cnt * g_mmc_info.block_size), left_size);
        DGB_PRINT("write left_size to blk:%d\n", start_block + block_cnt);
        if (MMC_WriteBlocks(handle, tmpbuf, start_block + block_cnt, 1)) {
            DGB_PRINTE("write blks e\n");
            return -1;
        }
    }
    DGB_PRINT("%s, %d\n", __func__, __LINE__);
    return 0;
}

static int _boot_emmc_erase(void *handle, off_t offset, size_t len)
{
    int ret;
    uint32_t startGroup, endGroup;

    if (!(handle)) {
        DGB_PRINTE("erase arg e.\n");
        return -EINVAL;
    }
    if (len == 0) {
        return 0;
    }
    DGB_PRINT("%s, %d, 0x%x, 0x%lx\n", __func__, __LINE__, offset, len);
    if (offset % g_mmc_info.erase_size) {
        DGB_PRINTE("offset must align erase_size\n");
        return -EINVAL;
    }
    if (len % g_mmc_info.erase_size) {
        DGB_PRINTE("len must align erase_size\n");
        return -EINVAL;
    }
    startGroup = offset / g_mmc_info.erase_size;
    endGroup = startGroup + len / g_mmc_info.erase_size - 1;
    ret = MMC_EraseGroups(handle, startGroup, endGroup);
    DGB_PRINT("%s, %d, startGroup: %d, endGroup: %d, ret:%d\n", __func__, __LINE__, startGroup, endGroup, ret);
    if (ret != kStatus_Success) {
        DGB_PRINTE("erase error.");
        return -EIO;
    }
    return 0;
}

static partition_device_ops_t boot_emmc_ops = {
    .storage_info.id      = 0,
    .storage_info.type    = MEM_DEVICE_TYPE_EMMC,
    .find           = _boot_emmc_find,
    .info_get       = _boot_emmc_info_get,
    .read           = _boot_emmc_read,
    .write          = _boot_emmc_write,
    .erase          = _boot_emmc_erase
};

int partition_emmc_register(void)
{
    return partition_device_register(&boot_emmc_ops);
}
#endif
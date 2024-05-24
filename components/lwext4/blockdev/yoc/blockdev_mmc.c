/*
 * Copyright (c) 2015 Grzegorz Kostka (kostka.grzegorz@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <ext4_config.h>
#include <ext4_blockdev.h>
#include <ext4_errno.h>
#include <ext4.h>
#include <ext4_mkfs.h>
#include <ext4_vfs.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <yoc/partition.h>

/**********************BLOCKDEV INTERFACE**************************************/
static int blockdev_mmc_open(struct ext4_blockdev *bdev);
static int blockdev_mmc_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
                              uint32_t blk_cnt);
static int blockdev_mmc_bwrite(struct ext4_blockdev *bdev, const void *buf,
                               uint64_t blk_id, uint32_t blk_cnt);
static int blockdev_mmc_close(struct ext4_blockdev *bdev);
static int blockdev_mmc_lock(struct ext4_blockdev *bdev);
static int blockdev_mmc_unlock(struct ext4_blockdev *bdev);

/******************************************************************************/
EXT4_BLOCKDEV_STATIC_INSTANCE(blockdev_mmc, 512, 0, blockdev_mmc_open,
                              blockdev_mmc_bread, blockdev_mmc_bwrite, blockdev_mmc_close, blockdev_mmc_lock, blockdev_mmc_unlock);

/******************************************************************************/
static aos_mutex_t blk_mmc_mtx;
static int blockdev_mmc_lock(struct ext4_blockdev *bdev)
{
    int ret;

    if (!aos_mutex_is_valid(&blk_mmc_mtx)) {
        ret = aos_mutex_new(&blk_mmc_mtx);
        if (ret != 0) {
            return -1;
        }
    }
    ret = aos_mutex_lock(&blk_mmc_mtx, AOS_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

static int blockdev_mmc_unlock(struct ext4_blockdev *bdev)
{
    int ret;

    if (aos_mutex_is_valid(&blk_mmc_mtx)) {
        ret = aos_mutex_unlock(&blk_mmc_mtx);
        if (ret != 0) {
            return -1;
        }
        return 0;
    }
    return -1;
}

static int blockdev_mmc_open(struct ext4_blockdev *bdev)
{
    partition_t partition;
    partition_info_t *part_info;

    printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    partition = partition_open(EXT4_PARTITION_NAME);
    if (partition < 0) {
        printf("ext4 partition open e.\n");
        return -1;
    }
    part_info = partition_info_get(partition);
    if (!part_info) {
        printf("ext4 partition get info e.\n");
        return -1;
    }
    bdev->part_offset    = 0;
    bdev->part_size      = part_info->length;
    bdev->bdif->ph_bcnt  = part_info->length / part_info->block_size;
    bdev->bdif->ph_bsize = part_info->block_size;
    return 0;
}
/******************************************************************************/

static int blockdev_mmc_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
                              uint32_t blk_cnt)
{
    // printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    partition_t partition = partition_open(EXT4_PARTITION_NAME);
    if (partition < 0) {
        printf("ext4 partition open e.\n");
        return -1;
    }
    partition_info_t *part_info = partition_info_get(partition);
    if (!part_info) {
        printf("ext4 partition get info e.\n");
        return -1;
    }
    //printf("=====>>>>>>>>>>>>%s, %d, id = %lu, cnt = %u=====\n", __FUNCTION__, __LINE__, blk_id, blk_cnt);
    int ret = partition_read(partition, blk_id * part_info->block_size, buf, blk_cnt * part_info->block_size);
    if (ret) {
        printf("!!=====>>>>>>>>>>>>%s, %d, id = %lu, cnt = %u=====\n", __FUNCTION__, __LINE__, blk_id, blk_cnt);
    }
    return ret;
}

/******************************************************************************/
static int blockdev_mmc_bwrite(struct ext4_blockdev *bdev, const void *buf,
                               uint64_t blk_id, uint32_t blk_cnt)
{
    // printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    partition_t partition = partition_open(EXT4_PARTITION_NAME);
    if (partition < 0) {
        printf("ext4 partition open e.\n");
        return -1;
    }
    partition_info_t *part_info = partition_info_get(partition);
    if (!part_info) {
        printf("ext4 partition get info e.\n");
        return -1;
    }
    int ret = partition_write(partition, blk_id * part_info->block_size, (void *)buf, blk_cnt * part_info->block_size);
    if (ret) {
        printf("!!=====>>>>>>>>>>>>%s, %d, id = %lu, cnt = %u=====\n", __FUNCTION__, __LINE__, blk_id, blk_cnt);
    }
    return ret;
}
/******************************************************************************/
static int blockdev_mmc_close(struct ext4_blockdev *bdev)
{
    printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    partition_t partition = partition_open(EXT4_PARTITION_NAME);
    if (partition < 0) {
        printf("ext4 partition open e.\n");
        return -1;
    }
    partition_close(partition);
    return 0;
}

/******************************************************************************/
struct ext4_blockdev *ext4_blockdev_mmc_get(void)
{
    return &blockdev_mmc;
}
/******************************************************************************/

void dfs_ext_mkfs()
{
    int rc;
    char *img = "mmc";
    static struct ext4_fs fs;
    static struct ext4_mkfs_info info = {
        .block_size = 1024,
        .journal = true,
    };

    printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    rc = ext4_device_register(&blockdev_mmc, img);
    if (0 == rc) {
        printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
        rc = ext4_mkfs(&fs, &blockdev_mmc, &info, F_SET_EXT4);
        printf("=====>>>>>>>>>>>>%s, %d=====, rc = %d\n", __FUNCTION__, __LINE__, rc);
        ext4_device_unregister(img);
    }
    printf("=====>>>>>>>>>>>>%s, %d=====, rc:%d\n", __FUNCTION__, __LINE__, rc);
}

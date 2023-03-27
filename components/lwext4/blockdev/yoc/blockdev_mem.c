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
#include <aos/kernel.h>
#include <aos/debug.h>

static aos_mutex_t mutex;
#define BLOCKDEV_MEM_SIZE (20*1024*1024)
static char g_memdev[BLOCKDEV_MEM_SIZE];

/**********************BLOCKDEV INTERFACE**************************************/
static int blockdev_mem_open(struct ext4_blockdev *bdev);
static int blockdev_mem_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
                              uint32_t blk_cnt);
static int blockdev_mem_bwrite(struct ext4_blockdev *bdev, const void *buf,
                               uint64_t blk_id, uint32_t blk_cnt);
static int blockdev_mem_close(struct ext4_blockdev *bdev);

/******************************************************************************/
EXT4_BLOCKDEV_STATIC_INSTANCE(blockdev_mem, 512, 0, blockdev_mem_open,
                              blockdev_mem_bread, blockdev_mem_bwrite, blockdev_mem_close, 0, 0);

/******************************************************************************/
static int blockdev_mem_open(struct ext4_blockdev *bdev)
{
    //printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    aos_mutex_new(&mutex);
    bdev->part_offset   = 0;
    bdev->part_size     = BLOCKDEV_MEM_SIZE;
    bdev->bdif->ph_bcnt = bdev->part_size / bdev->bdif->ph_bsize;

    return 0;
}

/******************************************************************************/

static int blockdev_mem_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
                              uint32_t blk_cnt)
{
    aos_mutex_lock(&mutex, AOS_WAIT_FOREVER);

    memcpy(buf, &g_memdev[0] + bdev->bdif->ph_bsize * blk_id, bdev->bdif->ph_bsize * blk_cnt);
    //printf("=====>>>>>>>>>>>>%s, %d, id = %llu, cnt = %u=====\n", __FUNCTION__, __LINE__, blk_id, blk_cnt);

    aos_mutex_unlock(&mutex);
    return 0;
}

/******************************************************************************/
static int blockdev_mem_bwrite(struct ext4_blockdev *bdev, const void *buf,
                               uint64_t blk_id, uint32_t blk_cnt)
{
    aos_mutex_lock(&mutex, AOS_WAIT_FOREVER);

    memcpy(&g_memdev[0] + bdev->bdif->ph_bsize * blk_id, buf, bdev->bdif->ph_bsize * blk_cnt);
    //printf("=====>>>>>>>>>>>>%s, %d, id = %llu, cnt = %u=====\n", __FUNCTION__, __LINE__, blk_id, blk_cnt);

    aos_mutex_unlock(&mutex);
    return 0;
}
/******************************************************************************/
static int blockdev_mem_close(struct ext4_blockdev *bdev)
{
    //TODO:
    //printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    return 0;
}

/******************************************************************************/
struct ext4_blockdev *ext4_blockdev_mem_get(void)
{
    return &blockdev_mem;
}
/******************************************************************************/

void dfs_ext_mkfs()
{
    int rc;
    char *img = "mem";
    static struct ext4_fs fs;
    static struct ext4_mkfs_info info = {
        .block_size = 1024,
        .journal = true,
    };

    printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
    rc = ext4_device_register(&blockdev_mem, img);
    if (0 == rc) {
        printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
        rc = ext4_mkfs(&fs, &blockdev_mem, &info, F_SET_EXT4);
        printf("=====>>>>>>>>>>>>%s, %d=====, rc = %d\n", __FUNCTION__, __LINE__, rc);
        ext4_device_unregister(img);
    }
    printf("=====>>>>>>>>>>>>%s, %d=====\n", __FUNCTION__, __LINE__);
}






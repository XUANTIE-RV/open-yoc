/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sd_disk.h"
#include <fatfs_vfs.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <ulog/ulog.h>
#include <yoc/partition.h>

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static int g_sd_partition = -1;
static char *TAG = "sddisk";

/*******************************************************************************
 * Code
 ******************************************************************************/
DRESULT sd_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != SDDISK) {
        return RES_PARERR;
    }

    if (g_sd_partition < 0) {
        return RES_PARERR;
    }
    partition_info_t *part_info = partition_info_get(g_sd_partition);
    if (!part_info) {
        LOGE(TAG, "get partition info e.");
        return RES_PARERR;
    }
    int ret = partition_write(g_sd_partition, (off_t)sector * part_info->block_size, (void *)buffer, count * part_info->block_size);
    if (ret) {
        LOGE(TAG, "sd disk write error: %d, sector:%d, count:%d", ret, sector, count);
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT sd_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != SDDISK) {
        return RES_PARERR;
    }

    if (g_sd_partition < 0) {
        return RES_PARERR;
    }
    partition_info_t *part_info = partition_info_get(g_sd_partition);
    if (!part_info) {
        LOGE(TAG, "get partition info e.");
        return RES_PARERR;
    }

    int ret = partition_read(g_sd_partition, (off_t)sector * part_info->block_size, buffer, count * part_info->block_size);
    if (ret) {
        LOGE(TAG, "sd disk read error: %d, sector:%d, count:%d", ret, sector, count);
        return RES_ERROR;
    }

    return RES_OK;
}

DRESULT sd_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer)
{
    DRESULT result = RES_OK;

    // LOGD(TAG, "physicalDrive:%d, command:%d", physicalDrive, command);
    if (physicalDrive != SDDISK) {
        return RES_PARERR;
    }
    if (g_sd_partition < 0) {
        return RES_PARERR;
    }
    partition_info_t *part_info = partition_info_get(g_sd_partition);
    if (!part_info) {
        return RES_PARERR;
    }
    switch (command) {
    case GET_SECTOR_COUNT:
        if (buffer) {
            *(uint32_t *)buffer = part_info->length / part_info->block_size;
        } else {
            result = RES_PARERR;
        }

        break;

    case GET_SECTOR_SIZE:
        if (buffer) {
            *(uint32_t *)buffer = part_info->block_size;
        } else {
            result = RES_PARERR;
        }

        break;

    case GET_BLOCK_SIZE:
        if (buffer) {
            *(uint32_t *)buffer = part_info->erase_size / part_info->block_size;
        } else {
            result = RES_PARERR;
        }

        break;

    case CTRL_SYNC:
        result = RES_OK;
        break;

    default:
        result = RES_PARERR;
        break;
    }

    return result;
}

DSTATUS sd_disk_status(uint8_t physicalDrive)
{
    if (physicalDrive != SDDISK) {
        return STA_NOINIT;
    }

    return 0;
}

DSTATUS sd_disk_initialize(uint8_t physicalDrive)
{

    if (physicalDrive != SDDISK) {
        return STA_NOINIT;
    }

    if (g_sd_partition >= 0) {
        partition_close(g_sd_partition);
        g_sd_partition = -1;
    }
    partition_t partition = partition_open(FATFS_PARTITION_NAME);
    if (partition < 0) {
        LOGE(TAG, "open %s failed.", (char *)FATFS_PARTITION_NAME);
        return RES_PARERR;
    }
    g_sd_partition = partition;

    return RES_OK;
}

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
#include <aos/kernel.h>
#include <aos/debug.h>

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor */
extern sd_card_t SDIO_SDCard;

static aos_mutex_t mutex;

/*******************************************************************************
 * Code
 ******************************************************************************/
DRESULT sd_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != SDDISK) {
        return RES_PARERR;
    }

    aos_mutex_lock(&mutex, AOS_WAIT_FOREVER);

    if (kStatus_Success != SD_WriteBlocks(&SDIO_SDCard, buffer, sector, count)) {
        aos_mutex_unlock(&mutex);
        return RES_ERROR;
    }

    aos_mutex_unlock(&mutex);

    return RES_OK;
}

DRESULT sd_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count)
{
    if (physicalDrive != SDDISK) {
        return RES_PARERR;
    }

    aos_mutex_lock(&mutex, AOS_WAIT_FOREVER);

    if (kStatus_Success != SD_ReadBlocks(&SDIO_SDCard, buffer, sector, count)) {
        aos_mutex_unlock(&mutex);
        return RES_ERROR;
    }

    aos_mutex_unlock(&mutex);

    return RES_OK;
}

DRESULT sd_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer)
{
    DRESULT result = RES_OK;

    if (physicalDrive != SDDISK) {
        return RES_PARERR;
    }

    switch (command) {
        case GET_SECTOR_COUNT:
            if (buffer) {
                *(uint32_t *)buffer = SDIO_SDCard.block_count;
            } else {
                result = RES_PARERR;
            }

            break;

        case GET_SECTOR_SIZE:
            if (buffer) {
                *(uint32_t *)buffer = SDIO_SDCard.block_size;
            } else {
                result = RES_PARERR;
            }

            break;

        case GET_BLOCK_SIZE:
            if (buffer) {
                *(uint32_t *)buffer = SDIO_SDCard.csd.eraseSectorSize;
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

    /* Save host information. */
    memset(&SDIO_SDCard, 0, sizeof(SDIO_SDCard));

    int re = aos_mutex_new(&mutex);

    aos_check(!re, ERR_MEM);

    status_t ret = SD_Init(&SDIO_SDCard, NULL, 1);

    return (ret == kStatus_Success) ? RES_OK : RES_NOTRDY;
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sdio_lanqi_porting.c
 * @brief    porting Source File for sdio Driver
 * @version  V1.0
 * @date     10. Oct 2018
 ******************************************************************************/

#include <sdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sdmmc_host.h>
#include <sdmmc_spec.h>
#include <soc.h>
#include "sdio_lanqi_porting.h"

static sdio_card_t SDIO_Card;
static struct rt_mmcsd_card mmcsd_card;
static struct rt_sdio_function sdio_function_sq[7];
static struct rt_sdio_function_tuple  sdio_function_tuple[3];

static volatile uint8_t flag_sem = 0;
uint8_t data_data = 0;

extern void aos_msleep(int ms);

static const uint32_t g_tupleList[3] = {
    SDIO_TPL_CODE_MANIFID, SDIO_TPL_CODE_FUNCID, SDIO_TPL_CODE_FUNCE,
};

static void sdio_inserted_callback(bool isInserted, void *userData)
{
    printf("SDIO Card Inserted!\n");
}

static void sdio_removed_callback(bool isInserted, void *userData)
{
    printf("SDIO Card Removed!\n");
}

/*! @brief SDMMC host detect card configuration */
const sdmmchost_detect_card_t sdio_CardDetect = {
    .cdType = kSDMMCHOST_DetectCardByHostCD,
    .cdTimeOut_ms = (~0U),
    .card_inserted = sdio_inserted_callback,
    .card_removed = sdio_removed_callback,
};

void sdio_int_lanqi(void)
{
    memset(&SDIO_Card, 0, sizeof(SDIO_Card));
    memset(&mmcsd_card, 0, sizeof(mmcsd_card));
    SDIO_Card.usrParam.cd = &sdio_CardDetect;
    SDIO_Card.host.base  = (sdif_handle_t)SDMMC_BASE_ADDR;
    status_t ret = SDIO_Init(&SDIO_Card);

    if (ret != kStatus_Success) {
        printf("sd init error ret %d\n", ret);
        return;
    }

    mmcsd_card.sdio_function[0] = &sdio_function_sq[0];
    mmcsd_card.sdio_function[1] = &sdio_function_sq[1];
    mmcsd_card.sdio_function[0]->num = 0;
    mmcsd_card.sdio_function[1]->num = 1;
    sdio_function_tuple[0].code = 0x80;
    sdio_function_tuple[0].size = 0x29;
    sdio_function_tuple[1].code = 0x22;
    sdio_function_tuple[1].size = 0x2a;
    sdio_function_tuple[2].code = 0x3;
    sdio_function_tuple[2].size = 0x4;

    mmcsd_card.cis.manufacturer = 0x188;
    mmcsd_card.cis.product = 0x6700;
    mmcsd_card.sdio_function[1]->tuples = &sdio_function_tuple[0];
    mmcsd_card.sdio_function[1]->tuples->next = &sdio_function_tuple[1];
    mmcsd_card.sdio_function[1]->tuples->next->next = &sdio_function_tuple[2];

    extern int sdio_wifi_probe(void * sdcard);
    sdio_wifi_probe((void *) &mmcsd_card);
}
int32_t sdio_io_rw_extended_block(struct rt_sdio_function *func,
                                  int32_t               rw,
                                  uint32_t              addr,
                                  int32_t               op_code,
                                  uint8_t              *buf,
                                  uint32_t              len)
{
    uint32_t flags = SDIO_EXTEND_CMD_BLOCK_MODE_MASK;
    uint32_t block_size;

    if (func->num == 1) {
        block_size = SDIO_Card.ioFBR[0].ioBlockSize;
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        block_size = SDIO_Card.io0block_size;
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    if (op_code == 1) {
        flags |= SDIO_EXTEND_CMD_OP_CODE_MASK;
    }

    status_t ret;

    if (rw == 0) {
        ret = SDIO_IO_Read_Extended(&SDIO_Card, func->num, addr, buf, len / block_size, flags);
    } else {
        ret = SDIO_IO_Write_Extended(&SDIO_Card, func->num, addr, buf, len / block_size, flags);
    }

    if (ret != kStatus_Success) {
        printf("sdio_io_rw_extended_block error rw %d\n", rw);
        return 1;
    }

    return 0;
}

uint8_t sdio_io_readb(struct rt_sdio_function *func,
                      uint32_t               reg,
                      int32_t              *err)
{

    status_t ret;
    data_data = 0;

    if (func->num == 1) {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {

        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    ret = SDIO_IO_Read_Direct(&SDIO_Card, func->num, reg, &data_data);

    if (ret != kStatus_Success) {
        printf("sdio_io_readb error\n");
        *err = 1;
        return 0;
    }

    *err = 0;
    return data_data;
}


int32_t sdio_io_writeb(struct rt_sdio_function *func,
                       uint32_t              reg,
                       uint8_t                 data)
{
    uint8_t data_tmp = data;
    status_t ret;

    if (func->num == 1) {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    ret = SDIO_IO_Write_Direct(&SDIO_Card, func->num, reg, &data_tmp, 0);

    if (ret != kStatus_Success) {
        printf("sdio_io_writeb error reg 0x%x\n", reg);
        return 1;
    }

    return 0;
}

int32_t sdio_set_block_size(struct rt_sdio_function *func, uint32_t blksize)
{
    status_t ret;

    if (func->num == 1) {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    static int i = 0;

    if (func->num == 1 && i == 0) {
        ret = SDIO_EnableIO(&SDIO_Card, kSDIO_FunctionNum1, 1);

        if (ret != kStatus_Success) {
            printf("SDIO_EnableIO error\n");
        }

        printf("start get capability\n");
        ret = SDIO_GetCardCapability(&SDIO_Card, kSDIO_FunctionNum1);

        if (ret != kStatus_Success) {
            printf("SDIO_GetCardCapability error\n");
        }

        printf("start read cis\n");
        ret = SDIO_ReadCIS(&SDIO_Card, kSDIO_FunctionNum1, g_tupleList, 3);

        if (ret != kStatus_Success) {
            printf("SDIO_ReadCIS error\n");
        }

        i++;
    }

    ret = SDIO_SetBlockSize(&SDIO_Card, func->num, blksize);

    if (ret != kStatus_Success) {
        printf("SDIO_SetBlockSize error\n");
        return  1;
    }

    return 0;
}

int32_t sdio_enable_func(struct rt_sdio_function *func)
{
    if (func->num == 1) {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum1);
    } else {
        SDIO_SelectIO(&SDIO_Card, kSDIO_FunctionNum0);
    }

    SDIO_EnableIO(&SDIO_Card, func->num, 1);

    return 0;
}

int rand_r(unsigned int *seed)
{
    *seed = rand();
    return 0;
}

void prctl(int a, unsigned long b)
{

}

int
ethip6_output(void *a, void *b, void *c)
{
    return 0;
}

#ifndef CONFIG_TEE_CA
int32_t csi_tee_rand_generate(uint8_t *out, uint32_t out_len)
{
    return 0;
}
#endif


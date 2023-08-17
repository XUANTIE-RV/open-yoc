/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_viparam.c
 * Description:
 *   ....
 */

#ifndef CONFIG_BOARD_VO_OSD

#include "custom_param.h"

#define BIN_DATA_SIZE 347537
extern unsigned char gc2053_ir_gc2093_rgb_cvi_isp_default_param[];

PARAM_CLASSDEFINE(PARAM_SNS_CFG_S,SENSORCFG,CTX,Sensor)[] = {
    {
        .enSnsType = CONFIG_SNS0_TYPE,
        .s32Framerate = 30,
        .s32I2cAddr = -1,
        .s8I2cDev = 2,
        .u32Rst_port_idx = 0,//GPIOA_2
        .u32Rst_pin = 2,
        .u32Rst_pol = OF_GPIO_ACTIVE_LOW,

        .bSetDevAttr = 1,
        .u8MclkCam = 0,
        .u8MclkFreq = CAMPLL_FREQ_27M,
        .s16MacClk = RX_MAC_CLK_200M,
        .s32Devno = 0,

        .bSetDevAttrMipi = 1,
        .as16LaneId[0] = 2,
        .as16LaneId[1] = 0,
        .as16LaneId[2] = 1,
        .as16LaneId[3] = -1,
        .as16LaneId[4] = -1,
        .as8PNSwap[0] = 0,
        .as8PNSwap[1] = 0,
        .as8PNSwap[2] = 0,
        .as8PNSwap[3] = 0,
        .as8PNSwap[4] = 0,
    },
    {
        .enSnsType = CONFIG_SNS1_TYPE,
        .s32Framerate = 30,
        .s32I2cAddr = 0x7e,
        .s8I2cDev = 2,
        .u32Rst_port_idx = 0,//GPIOA_6
        .u32Rst_pin = 6,
        .u32Rst_pol = OF_GPIO_ACTIVE_LOW,

        .bSetDevAttr = 1,
        .u8MclkCam = 1,
        .u8MclkFreq = CAMPLL_FREQ_27M,
        .s16MacClk = RX_MAC_CLK_200M,
        .s32Devno = 1,

        .bSetDevAttrMipi = 1,
        .as16LaneId[0] = 5,
        .as16LaneId[1] = 3,
        .as16LaneId[2] = 4,
        .as16LaneId[3] = -1,
        .as16LaneId[4] = -1,
        .as8PNSwap[0] = 1,
        .as8PNSwap[1] = 1,
        .as8PNSwap[2] = 1,
        .as8PNSwap[3] = 0,
        .as8PNSwap[4] = 0,
    },
};

PARAM_CLASSDEFINE(PARAM_ISP_CFG_S,ISPCFG,CTX,ISP)[] = {
    {
        .bMonoSet = {1,0},
        .bUseSingleBin = 0,
        .stPQBinDes =
        {
            .pIspBinData = gc2053_ir_gc2093_rgb_cvi_isp_default_param,
            .u32IspBinDataLen = BIN_DATA_SIZE,
        },
    }
};

PARAM_VI_CFG_S g_stViCtx = {
    .u32WorkSnsCnt = 2,
    .pstSensorCfg = PARAM_CLASS(SENSORCFG,CTX,Sensor),
    .pstIspCfg = PARAM_CLASS(ISPCFG,CTX,ISP),
    .bFastConverge = CVI_TRUE,
};

PARAM_VI_CFG_S * PARAM_GET_VI_CFG(void) {
    return &g_stViCtx;
}

#endif







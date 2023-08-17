/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_sysparam.c
 * Description:
 *   ....
 */

#ifndef CONFIG_BOARD_VO_OSD

#include "custom_param.h"
#include "board_config.h"

PARAM_CLASSDEFINE(PARAM_VB_CFG_S,VBPOOL,CTX,VB)[] = {
    {
        .u16width = 1280,
        .u16height = 720,
        .u8VbBlkCnt = 6,
        .fmt = PIXEL_FORMAT_NV21,
        .enBitWidth = DATA_BITWIDTH_8,
        .enCmpMode = COMPRESS_MODE_NONE,
    },
    {
        .u16width = 640,
        .u16height = 480,
        .u8VbBlkCnt = 7,
        .fmt = PIXEL_FORMAT_NV21,
        .enBitWidth = DATA_BITWIDTH_8,
        .enCmpMode = COMPRESS_MODE_NONE,
    },
    {
        .u16width = 480,
        .u16height = 480,
        .u8VbBlkCnt = 4,
        .fmt = PIXEL_FORMAT_NV21,
        .enBitWidth = DATA_BITWIDTH_8,
        .enCmpMode = COMPRESS_MODE_NONE,
    },
    {
        .u16width = 608,
        .u16height = 342,
        .u8VbBlkCnt = 1,
        .fmt = PIXEL_FORMAT_RGB_888_PLANAR,
        .enBitWidth = DATA_BITWIDTH_8,
        .enCmpMode = COMPRESS_MODE_NONE,
    },
};

PARAM_SYS_CFG_S  g_stSysCtx = {
    .u8VbPoolCnt = 3,
    .u8ViCnt = 2,
    .stVIVPSSMode.aenMode[0] = VI_OFFLINE_VPSS_ONLINE,
    .stVIVPSSMode.aenMode[1] = VI_OFFLINE_VPSS_ONLINE,
    .stVPSSMode.enMode = VPSS_MODE_DUAL,
    .stVPSSMode.ViPipe[0] = 0,
    .stVPSSMode.aenInput[0] = VPSS_INPUT_MEM,
    .stVPSSMode.ViPipe[1] = 0,
    .stVPSSMode.aenInput[1] = VPSS_INPUT_ISP,
    .pstVbPool = PARAM_CLASS(VBPOOL,CTX,VB),
};

PARAM_SYS_CFG_S * PARAM_GET_SYS_CFG(void) {
    return &g_stSysCtx;
}

#endif

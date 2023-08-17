/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_sysparam.c
 * Description:
 *   ....
 */

#ifdef CONFIG_BOARD_VO_OSD

#include "custom_param.h"
#include "board_config.h"

PARAM_CLASSDEFINE(PARAM_VB_CFG_S,VBPOOL,CTX,VB)[] = {
    {
        .u16width = PANEL_WIDTH,
        .u16height = PANEL_HEIGHT,
        .u8VbBlkCnt = 2,
        .fmt = PIXEL_FORMAT_NV21,
        .enBitWidth = DATA_BITWIDTH_8,
        .enCmpMode = COMPRESS_MODE_NONE,
    },
};

PARAM_SYS_CFG_S  g_stSysCtx = {
    .u8VbPoolCnt = 1,
    .u8ViCnt = 1,
    .stVIVPSSMode.aenMode[0] = VI_OFFLINE_VPSS_OFFLINE,
    //.stVIVPSSMode.aenMode[1] =,
    .stVPSSMode.enMode = VPSS_MODE_SINGLE,
    .stVPSSMode.ViPipe[0] = 0,
    .stVPSSMode.aenInput[0] = VPSS_INPUT_MEM,
    .pstVbPool = PARAM_CLASS(VBPOOL,CTX,VB),
};

PARAM_SYS_CFG_S * PARAM_GET_SYS_CFG(void) {
    return &g_stSysCtx;
}

#endif

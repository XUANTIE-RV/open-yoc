/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_vencparam.c
 * Description:
 *   ....
 */

#ifndef CONFIG_BOARD_VO_OSD

#include "custom_param.h"
#include "cvi_venc.h"

PARAM_CLASSDEFINE(PARAM_VENC_CHN_CFG_S,VENCCFG,CTX,VENC)[] = {
    {
        .stChnParam = {
            .u8InitStatus = 0,
            .u8VencChn = 0,
            .u8ModId = CVI_ID_VPSS,
            .u8DevId = 0,
            .u8DevChnid = 0,
            .u8Profile = 0,
            .u16Width = 0,
            .u16Height = 0,
            .u8EsBufQueueEn = 0,
            .u16EnType = PT_H265,
            .u32BitStreamBufSize = 1024 * 1024,
        },
        .stGopParam = {
            .u16gopMode = 0,
            .s8IPQpDelta = 0,
        },
        .stRcParam = {
            .u16Gop = 30,
            .u8SrcFrameRate = 30,
            .u8DstFrameRate = 30,
            .u16BitRate = 1024,
            .u8Qfactor = 60,
            .u32MaxBitRate = CVI_H26X_FRAME_BITS_DEFAULT,
            .u8VariFpsEn = 0,
            .u8StartTime = 2,
            .u16RcMode = VENC_RC_MODE_H265CBR,
            .u16FirstFrmstartQp = 30,
            .u16InitialDelay = CVI_INITIAL_DELAY_DEFAULT, // RW = , Range:[10, 3000] Rate control initial delay (ms).
            .u16ThrdLv = 2,/*RW = , Range:[0, 4] = , Mad threshold for controlling the macroblock-level bit rate */
            .u16BgDeltaQp = 0, /* RW = , Range:[-51, 51] =  Backgournd Qp Delta */
            .u8MinIprop = 1,
            .u8MaxIprop = 100,
            .u8MinIqp = CVI_H26X_MINQP_DEFAULT,//0-51
            .u8MaxIqp = CVI_H26X_MAXQP_DEFAULT,//0-51
            .u8MinQp = CVI_H26X_MINQP_DEFAULT,//0-51
            .u8MaxQp = CVI_H26X_MAXQP_DEFAULT,//0-51
            .u8MaxReEncodeTimes = 0, /* RW = , Range:[0, 3]  Range:max number of re-encode times.*/
            .u8QpMapEn = CVI_FALSE, /* RW = , Range:[0, 1]  enable qpmap.*/
            .u8ChangePos = DEF_26X_CHANGE_POS, //VBRʹ��
        },
        .s8RoiNumber = -1,
    },
    {
        .stChnParam = {
            .u8InitStatus = 0,
            .u8VencChn = 1,
            .u8ModId = CVI_ID_VPSS,
            .u8DevId = 1,
            .u8DevChnid = 0,
            .u8Profile = 0,
            .u16Width = 0,
            .u16Height = 0,
            .u8EsBufQueueEn = 0,
            .u16EnType = PT_H264,
            .u32BitStreamBufSize = 1024 * 1024,
        },
        .stGopParam = {
            .u16gopMode = 0,
            .s8IPQpDelta = 0,
        },
        .stRcParam = {
            .u16Gop = 30,
            .u8SrcFrameRate = 30,
            .u8DstFrameRate = 30,
            .u16BitRate = 1024,
            .u8Qfactor = 60,
            .u32MaxBitRate = CVI_H26X_FRAME_BITS_DEFAULT,
            .u8VariFpsEn = 0,
            .u8StartTime = 2,
            .u16RcMode = VENC_RC_MODE_H264CBR,
            .u16FirstFrmstartQp = 30,
            .u16InitialDelay = CVI_INITIAL_DELAY_DEFAULT, // RW = , Range:[10, 3000] Rate control initial delay (ms).
            .u16ThrdLv = 2,/*RW = , Range:[0, 4] = , Mad threshold for controlling the macroblock-level bit rate */
            .u16BgDeltaQp = 0, /* RW = , Range:[-51, 51] =  Backgournd Qp Delta */
            .u8MinIprop = 1,
            .u8MaxIprop = 100,
            .u8MinIqp = CVI_H26X_MINQP_DEFAULT,//0-51
            .u8MaxIqp = CVI_H26X_MAXQP_DEFAULT,//0-51
            .u8MinQp = CVI_H26X_MINQP_DEFAULT,//0-51
            .u8MaxQp = CVI_H26X_MAXQP_DEFAULT,//0-51
            .u8MaxReEncodeTimes = 0, /* RW = , Range:[0, 3]  Range:max number of re-encode times.*/
            .u8QpMapEn = CVI_FALSE, /* RW = , Range:[0, 1]  enable qpmap.*/
            .u8ChangePos = DEF_26X_CHANGE_POS, //VBRʹ��
        },
        .s8RoiNumber = -1,
    }
};

PARAM_VENC_CFG_S  g_stVencCtx = {
    .s32VencChnCnt = 2,
    .pstVencChnCfg = PARAM_CLASS(VENCCFG,CTX,VENC),
};

PARAM_VENC_CFG_S * PARAM_GET_VENC_CFG(void) {
    return &g_stVencCtx;
}

#endif

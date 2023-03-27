/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_vencparam.c
 * Description:
 *   ....
 */
#include "custom_param.h"
#include "cvi_venc.h"


PARAM_VENC_CFG_S  g_stVencCtx = {
    .s32VencChnCnt = 0,
    .pstVencChnCfg = NULL,
};

PARAM_VENC_CFG_S * PARAM_GET_VENC_CFG(void) {
    return &g_stVencCtx;
}

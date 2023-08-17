/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_viparam.c
 * Description:
 *   ....
 */

#ifdef CONFIG_BOARD_VO_OSD

#include "custom_param.h"

PARAM_VI_CFG_S g_stViCtx = {
    .u32WorkSnsCnt = 0,
    .pstSensorCfg = NULL,
    .pstIspCfg = NULL,
};

PARAM_VI_CFG_S * PARAM_GET_VI_CFG(void) {
    return &g_stViCtx;
}

#endif






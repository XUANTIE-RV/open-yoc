/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_vpsscfg.c
 * Description:
 *   ....
 */

#ifdef CONFIG_BOARD_VO_OSD

#include "custom_param.h"
#include "board_config.h"
#include "cvi_buffer.h"

PARAM_VPSS_CFG_S  g_stVpssCtx = {
    .u8GrpCnt = 0,
    .pstVpssGrpCfg = NULL,
};

PARAM_VPSS_CFG_S * PARAM_GET_VPSS_CFG(void) {
    return &g_stVpssCtx;
}

#endif

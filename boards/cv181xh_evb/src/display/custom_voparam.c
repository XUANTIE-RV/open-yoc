/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_vocfg.c
 * Description:
 *   ....
 */
#include "custom_param.h"
#include "cvi_buffer.h"
#include "cvi_mipi_tx.h"

PARAM_CLASSDEFINE(VO_CHN_ATTR_S,VOCHN,VO,CHN)[] = {
    {
        .stRect = {0, 0, PANEL_WIDTH, PANEL_HEIGHT},
    }
};

PARAM_CLASSDEFINE(PARAM_VODEV_S,VOCFG,CTX,VO)[] = {
    {
        .stVoConfig = {
            .VoDev  = 0,
            .u32DisBufLen  = 3,
            .stVoPubAttr.enIntfType  = VO_INTF_MIPI,
            .stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60,
            .stVoPubAttr.u32BgColor = COLOR_10_RGB_BLACK,
            .stDispRect = {0, 0, PANEL_WIDTH, PANEL_HEIGHT},
            .stImageSize = {PANEL_WIDTH, PANEL_HEIGHT},
            .enPixFormat = PIXEL_FORMAT_NV21,
        },
        .stLayerAttr = {
            .stDispRect = {0, 0, PANEL_WIDTH, PANEL_HEIGHT},
            .u32DispFrmRt = 60,
            .stImageSize = {PANEL_WIDTH, PANEL_HEIGHT},
            .enPixFormat = PIXEL_FORMAT_NV21,
        },
        .u8ChnCnt = 1,
        .pstVOChnAttr = PARAM_CLASS(VOCHN,VO,CHN),
        .u8Bindmode = CVI_TRUE,
        .stSrcChn = {
            .enModId = CVI_ID_VPSS,
            .s32ChnId = 0,
            .s32DevId = 0,
        },
        .stDestChn = {
            .enModId = CVI_ID_VO,
            .s32ChnId = 0,
            .s32DevId = 0,
        },
    }
};

#if (CONFIG_BOARD_RUIXIANG_BOARD == 1)
PARAM_CLASSDEFINE(PARAM_PANELDEV_S,PANELCFG,CTX,PANEL)[] = {
    {
        .bset_pin = 1,
        .reset_pin = {
            .gpio_num = CVI_GPIOA_02,
            .active = 0
        },
        .power_ct_pin = {
            .gpio_num = -1,
            .active = 1
        },
        .pwm_pin = {
            .gpio_num = -1,
            .active = 1
        }
    }
};
#endif

PARAM_VO_CFG_S g_stVoCtx = {
    .u8VoCnt = 1,
    .pstVoCfg = PARAM_CLASS(VOCFG,CTX,VO),
#if (CONFIG_BOARD_RUIXIANG_BOARD == 1)
    .pstPanelTxCfg = PARAM_CLASS(PANELCFG,CTX,PANEL),
#else
    .pstPanelTxCfg = NULL,
#endif
};

PARAM_VO_CFG_S * PARAM_GET_VO_CFG(void) {
    return &g_stVoCtx;
}

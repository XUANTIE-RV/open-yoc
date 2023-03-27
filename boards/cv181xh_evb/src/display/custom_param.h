/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: custom_param.h
 * Description:
 *   ....
 */
#ifndef __CUSTOM_PARAM_H__
#define __CUSTOM_PARAM_H__
#include "param_sys.h"
#include "param_vi.h"
#include "param_vpss.h"
#include "param_venc.h"
#include "param_vo.h"

#define PARAM_CLASSDEFINE(CLASSNAME, CLASSTYPE, DEVFATHER, DEV) \
                        static CLASSNAME CLASSTYPE##_##DEVFATHER##_##DEV
#define PARAM_CLASS(CLASSTYPE, DEVFATHER, DEV) CLASSTYPE##_##DEVFATHER##_##DEV

typedef struct _PARAM_MANAGER_CFG_S {
    PARAM_SYS_CFG_S * pstSysCtx;
    PARAM_VI_CFG_S * pstViCtx;
    PARAM_VPSS_CFG_S * pstVpssCfg;
    PARAM_VENC_CFG_S * pstVencCfg;
    PARAM_VO_CFG_S * pstVoCfg;
} PARAM_MANAGER_CFG_S;

PARAM_SYS_CFG_S * PARAM_GET_SYS_CFG(void);
PARAM_VI_CFG_S * PARAM_GET_VI_CFG(void);
PARAM_VPSS_CFG_S * PARAM_GET_VPSS_CFG(void);
PARAM_VENC_CFG_S * PARAM_GET_VENC_CFG(void);
PARAM_VO_CFG_S * PARAM_GET_VO_CFG(void);
PARAM_MANAGER_CFG_S * PARAM_GET_MANAGER_CFG(void);
void PARAM_INIT_MANAGER_CFG();
void PARAM_SET_MANAGER_CFG_PIPE(int pipeline);
int PARAM_GET_MANAGER_CFG_PIPE();




#endif

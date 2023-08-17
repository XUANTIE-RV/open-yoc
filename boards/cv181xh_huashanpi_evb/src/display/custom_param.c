#include "custom_param.h"

PARAM_MANAGER_CFG_S  g_stManagerCtx = {
    .pstSysCtx = NULL,
    .pstViCtx = NULL,
    .pstVpssCfg = NULL,
    .pstVoCfg = NULL,
    .pstVencCfg = NULL,
};

PARAM_MANAGER_CFG_S * PARAM_GET_MANAGER_CFG(void)
{
    return &g_stManagerCtx;
}

void PARAM_SET_MANAGER_CFG_PIPE(int pipeline)
{
    //这里进行pipeline切换预处理
    g_stManagerCtx.pstSysCtx = PARAM_GET_SYS_CFG();
    g_stManagerCtx.pstViCtx = PARAM_GET_VI_CFG();
    g_stManagerCtx.pstVpssCfg = PARAM_GET_VPSS_CFG();
    g_stManagerCtx.pstVencCfg = PARAM_GET_VENC_CFG();
    g_stManagerCtx.pstVoCfg = PARAM_GET_VO_CFG();
}

void PARAM_INIT_MANAGER_CFG()
{
//决定初始化采用哪套pipeline
    PARAM_SET_MANAGER_CFG_PIPE(0);
}
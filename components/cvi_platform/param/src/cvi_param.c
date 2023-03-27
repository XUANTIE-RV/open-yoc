#include "cvi_param.h"

static int s_scene_mode = 0;

//parm 结构 入口
CVI_S32 PARAM_LoadCfg(CVI_VOID) {
    //Media 流媒体 能力集
    PARAM_INIT_MANAGER_CFG();
    return 0;
}
//VB
PARAM_SYS_CFG_S *PARAM_getSysCtx(void)
{
    if(PARAM_GET_MANAGER_CFG() == NULL) {
        return NULL;
    }
    return PARAM_GET_MANAGER_CFG()->pstSysCtx;
}
//VI
PARAM_VI_CFG_S *PARAM_getViCtx(void)
{
    return PARAM_GET_MANAGER_CFG()->pstViCtx;
}
//VPSS
PARAM_VPSS_CFG_S *PARAM_getVpssCtx(void)
{
    if(PARAM_GET_MANAGER_CFG() == NULL) {
        return NULL;
    }
    return PARAM_GET_MANAGER_CFG()->pstVpssCfg;
}
//VENC
PARAM_VENC_CFG_S *PARAM_getVencCtx(void)
{
    if(PARAM_GET_MANAGER_CFG() == NULL) {
        return NULL;
    }
    return PARAM_GET_MANAGER_CFG()->pstVencCfg;
}
//VO
PARAM_VO_CFG_S *PARAM_getVoCtx(void)
{
    if(PARAM_GET_MANAGER_CFG() == NULL) {
        return NULL;
    }
    return PARAM_GET_MANAGER_CFG()->pstVoCfg;
}

void PARAM_setPipeline(int pipeline)
{
    PARAM_SET_MANAGER_CFG_PIPE(pipeline);
}

int PARAM_getPipeline(void)
{
    return PARAM_GET_MANAGER_CFG_PIPE();
}

void PARAM_setSceneMode(int mode)
{
    s_scene_mode = mode;
}

int PARAM_getSceneMode()
{
    return s_scene_mode;
}

__attribute__((weak)) int PARAM_GET_MANAGER_CFG_PIPE()
{
    return 0;
}

__attribute__((weak)) PARAM_MANAGER_CFG_S * PARAM_GET_MANAGER_CFG(void)
{
    return NULL;
}

__attribute__((weak)) void PARAM_SET_MANAGER_CFG_PIPE(int pipeline)
{
}

__attribute__((weak)) void PARAM_INIT_MANAGER_CFG()
{
}
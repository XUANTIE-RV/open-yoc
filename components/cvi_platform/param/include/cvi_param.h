#ifndef __CVI_PARAM_H__
#define __CVI_PARAM_H__

#include "param_sys.h"
#include "param_vi.h"
#include "param_venc.h"
#include "param_vpss.h"
#include "param_vo.h"
#include "custom_param.h"

//多级别结构 确定使用的多级数组加number作标定 (多级限定场景 串行结构 不允许存在同一级别有多个多级存在) 大结构必须精简尽量使用指针

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CVI_S32 PARAM_LoadCfg(CVI_VOID);
PARAM_SYS_CFG_S *PARAM_getSysCtx(void);
PARAM_VI_CFG_S *PARAM_getViCtx(void);
PARAM_VPSS_CFG_S *PARAM_getVpssCtx(void);
PARAM_VENC_CFG_S *PARAM_getVencCtx(void);
PARAM_VO_CFG_S *PARAM_getVoCtx(void);
void PARAM_setPipeline(int pipeline);
int PARAM_getPipeline(void);
void PARAM_setSceneMode(int mode);
int PARAM_getSceneMode();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

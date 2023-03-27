#ifndef __CVI_VPSSCFG_H_
#define __CVI_VPSSCFG_H_
#include "cvi_comm_video.h"
#include "cvi_comm_sys.h"
#include "cvi_comm_vpss.h"
#include "cvi_defines.h"

//多级别结构 确定使用的多级数组加number作标定 (多级限定场景 串行结构 不允许存在同一级别有多个多级存在) 大结构必须精简尽量使用指针

typedef struct _PARAM_VPSS_CHN_CFG_S {
    CVI_U8 u8Rotation;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_CROP_INFO_S  stVpssChnCropInfo;
} PARAM_VPSS_CHN_CFG_S;

typedef struct _PARAM_VPSS_GRP_CFG_S {
    VPSS_GRP VpssGrp;
    VI_DEV   s32BindVidev;  /* get Grp from vi if not set -1*/
    CVI_U8 u8ViRotation; // vi rotation
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CROP_INFO_S stVpssGrpCropInfo;
    CVI_U8 u8ChnCnt;
    PARAM_VPSS_CHN_CFG_S * pstChnCfg;
    CVI_BOOL bBindMode;
    MMF_CHN_S astChn[2];
} PARAM_VPSS_GRP_CFG_S;

typedef struct _PARAM_VPSS_CFG_S {
    CVI_U8 u8GrpCnt;
    PARAM_VPSS_GRP_CFG_S *pstVpssGrpCfg;
} PARAM_VPSS_CFG_S;



#endif

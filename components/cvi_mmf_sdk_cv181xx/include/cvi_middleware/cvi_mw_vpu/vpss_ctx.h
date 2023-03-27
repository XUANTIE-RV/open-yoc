#ifndef MODULES_VPU_INCLUDE_VPSS_CTX_H_
#define MODULES_VPU_INCLUDE_VPSS_CTX_H_

#include <stdio.h>
#include <aos/aos.h>

#include <cvi_vip_sc.h>
#include "cvi_mw_base.h"
//#include "cvi_vpss_ctx.h"

struct VPSS_CHN_CFG {
	CVI_BOOL isEnabled;
	VPSS_CHN_ATTR_S stChnAttr;
	ROTATION_E enRotation;
	VPSS_LDC_ATTR_S stLDCAttr;
};

#if 1  //use osdrv cvi_vpss_ctx.h
struct cvi_vpss_ctx {
	CVI_BOOL isCreated;
	CVI_BOOL isStarted;
	VPSS_GRP_ATTR_S stGrpAttr;
	CVI_U8 chnNum;
	struct VPSS_CHN_CFG stChnCfgs[VPSS_MAX_CHN_NUM];
	CVI_S32 proc_amp[PROC_AMP_MAX];
};
#endif

struct cvi_vpss_ctx **vpss_get_ctx(void);

#endif /* MODULES_VPU_INCLUDE_VPSS_CTX_H_ */

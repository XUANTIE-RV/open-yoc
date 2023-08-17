/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: common_vi.h
 * Description:
 */

#ifndef __COMMON_VI_H__
#define __COMMON_VI_H__

#include "cvi_comm_vi.h"
#include "cvi_sns_ctrl.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct _SNS_SIZE_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
} SNS_SIZE_S;

CVI_S32 get_sensor_type(CVI_S32 dev_id);
CVI_S32 getSnsType(CVI_S32 *snsr_type, CVI_U8 *devNum);
ISP_SNS_OBJ_S *getSnsObj(SNS_TYPE_E enSnsType);
CVI_S32 getSnsMode(CVI_S32 dev_id, ISP_CMOS_SENSOR_IMAGE_MODE_S *snsr_mode);
CVI_S32 getPicSize(CVI_S32 dev_id, SNS_SIZE_S *pstSize);
CVI_S32 getDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstViDevAttr);
CVI_S32 getPipeAttr(VI_DEV ViDev, VI_PIPE_ATTR_S *pstViPipeAttr);
CVI_S32 getChnAttr(VI_DEV ViDev, VI_CHN_ATTR_S *pstViChnAttr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __APP_REGION_H__*/

#include <stdio.h>
// #include <iostream>
#include "cviai.h"

#define ALI_OS 1

#ifdef ALI_OS
#include <aos/cli.h>
#include <cvi_buffer.h>
#include <cvi_comm_vb.h>
#include <cvi_comm_vpss.h>
#include <cvi_math.h>
#include <cvi_sys.h>
#include <cvi_vb.h>
#include <cvi_vpss.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include "ai_utils.h"
#else
#include "core/cviai_types_mem_internal.h"
#include "core/utils/vpss_helper.h"
#include "ive/ive.h"

#endif


void extract_VPSS_GRP_DEFAULT_HELPER2_1(VPSS_GRP_ATTR_S *pstVpssGrpAttr, CVI_U32 srcWidth, CVI_U32 srcHeight,
                         PIXEL_FORMAT_E enSrcFormat, CVI_U8 dev) {
  memset(pstVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
  pstVpssGrpAttr->stFrameRate.s32SrcFrameRate = -1;
  pstVpssGrpAttr->stFrameRate.s32DstFrameRate = -1;
  pstVpssGrpAttr->enPixelFormat = enSrcFormat;
  pstVpssGrpAttr->u32MaxW = srcWidth;
  pstVpssGrpAttr->u32MaxH = srcHeight;
  pstVpssGrpAttr->u8VpssDev = dev;
}
void extract_VPSS_CHN_DEFAULT_HELPER_1(VPSS_CHN_ATTR_S *pastVpssChnAttr, CVI_U32 dstWidth, CVI_U32 dstHeight,
                        PIXEL_FORMAT_E enDstFormat, CVI_BOOL keepAspectRatio) {
  pastVpssChnAttr->u32Width = dstWidth;
  pastVpssChnAttr->u32Height = dstHeight;
  pastVpssChnAttr->enVideoFormat = VIDEO_FORMAT_LINEAR;
  pastVpssChnAttr->enPixelFormat = enDstFormat;

  pastVpssChnAttr->stFrameRate.s32SrcFrameRate = -1;
  pastVpssChnAttr->stFrameRate.s32DstFrameRate = -1;
  pastVpssChnAttr->u32Depth = 1;
  pastVpssChnAttr->bMirror = CVI_FALSE;
  pastVpssChnAttr->bFlip = CVI_FALSE;
  if (keepAspectRatio) {
    pastVpssChnAttr->stAspectRatio.enMode = ASPECT_RATIO_AUTO;
    pastVpssChnAttr->stAspectRatio.u32BgColor = RGB_8BIT(0, 0, 0);
  } else {
    pastVpssChnAttr->stAspectRatio.enMode = ASPECT_RATIO_NONE;
  }
  pastVpssChnAttr->stNormalize.bEnable = CVI_FALSE;
  pastVpssChnAttr->stNormalize.factor[0] = 0;
  pastVpssChnAttr->stNormalize.factor[1] = 0;
  pastVpssChnAttr->stNormalize.factor[2] = 0;
  pastVpssChnAttr->stNormalize.mean[0] = 0;
  pastVpssChnAttr->stNormalize.mean[1] = 0;
  pastVpssChnAttr->stNormalize.mean[2] = 0;
  pastVpssChnAttr->stNormalize.rounding = VPSS_ROUNDING_TO_EVEN;
}
void extract_VPSS_CHN_SQ_HELPER_1(VPSS_CHN_ATTR_S *pastVpssChnAttr, const CVI_U32 dstWidth,
                   const CVI_U32 dstHeight, const PIXEL_FORMAT_E enDstFormat,
                   const CVI_FLOAT *factor, const CVI_FLOAT *mean, const bool padReverse) {
  pastVpssChnAttr->u32Width = dstWidth;
  pastVpssChnAttr->u32Height = dstHeight;
  pastVpssChnAttr->enVideoFormat = VIDEO_FORMAT_LINEAR;
  pastVpssChnAttr->enPixelFormat = enDstFormat;
  pastVpssChnAttr->stFrameRate.s32SrcFrameRate = -1;
  pastVpssChnAttr->stFrameRate.s32DstFrameRate = -1;
  pastVpssChnAttr->u32Depth = 1;
  pastVpssChnAttr->bMirror = CVI_FALSE;
  pastVpssChnAttr->bFlip = CVI_FALSE;
  pastVpssChnAttr->stAspectRatio.enMode = ASPECT_RATIO_AUTO;
  pastVpssChnAttr->stAspectRatio.bEnableBgColor = CVI_TRUE;
  if (padReverse) {
    pastVpssChnAttr->stAspectRatio.u32BgColor = RGB_8BIT(
        (int)(mean[0] / factor[0]), (int)(mean[1] / factor[1]), (int)(mean[2] / factor[2]));
  } else {
    pastVpssChnAttr->stAspectRatio.u32BgColor = RGB_8BIT(0, 0, 0);
  }
  pastVpssChnAttr->stNormalize.bEnable = CVI_TRUE;
  for (uint32_t i = 0; i < 3; i++) {
    pastVpssChnAttr->stNormalize.factor[i] = factor[i];
  }
  for (uint32_t i = 0; i < 3; i++) {
    pastVpssChnAttr->stNormalize.mean[i] = mean[i];
  }
  pastVpssChnAttr->stNormalize.rounding = VPSS_ROUNDING_TO_EVEN;
}

int extract_sendFrameBase_1(const VIDEO_FRAME_INFO_S *frame,
                              const VPSS_CHN_ATTR_S *chn_attr,
                              const uint32_t enable_chns,VPSS_GRP vpss_grpid,VIDEO_FRAME_INFO_S *p_dst_frm) {
  uint32_t enabled_chn =1;
  CVI_S32 ret = CVI_SUCCESS;
  if (enable_chns >= enabled_chn) {
    for (uint32_t i = enabled_chn; i < enable_chns; i++) {
      ret = CVI_VPSS_EnableChn(vpss_grpid, i);
      printf("enable channel:%d,ret:%d\n",i,ret);
    }
  } else {
    for (uint32_t i = enable_chns; i < enabled_chn; i++) {
       CVI_VPSS_DisableChn(vpss_grpid, i);
    }
  }
  enabled_chn = enable_chns;

  VPSS_GRP_ATTR_S vpss_grp_attr;
  extract_VPSS_GRP_DEFAULT_HELPER2_1(&vpss_grp_attr, frame->stVFrame.u32Width, frame->stVFrame.u32Height,
                           frame->stVFrame.enPixelFormat, 0);



  ret = CVI_VPSS_SetGrpAttr(vpss_grpid, &vpss_grp_attr);
  printf("CVI_VPSS_SetGrpAttr ,ret:,%d\n", ret);
  if (ret != CVI_SUCCESS) {
    
    return ret;
  }
  VPSS_CROP_INFO_S m_crop_attr_reset;
  memset(&m_crop_attr_reset, 0, sizeof(VPSS_CROP_INFO_S));
  ret = CVI_VPSS_SetGrpCrop(vpss_grpid, &m_crop_attr_reset);
  printf("set vpss crop,ret:,%d\n",ret);
  for (uint32_t i = 0; i < enabled_chn; i++) {
    ret = CVI_VPSS_SetChnAttr(vpss_grpid, i, &chn_attr[i]);
    printf("set vpss channel,ret:,%d,grp:%d\n",ret,vpss_grpid);
    ret = CVI_VPSS_SetChnCrop(vpss_grpid, i, &m_crop_attr_reset);
    printf("set channel crop,ret:,%d,grp:%d\n",ret,vpss_grpid);
    ret = CVI_VPSS_SetChnScaleCoefLevel(vpss_grpid, i, VPSS_SCALE_COEF_BICUBIC);
    printf("set channel coeff,ret:,%d,grp:%d\n",ret,vpss_grpid);
    if (ret != CVI_SUCCESS) {
      printf( "CVI_VPSS_SetChnAttr failed with %#x\n", ret);
      return ret;
    }
  }

  printf("to send vpss frame");
  ret |= CVI_VPSS_SendFrame(vpss_grpid, frame, -1);
  printf("finish send vpss frame,ret:,%d\n",ret);
  memset(p_dst_frm, 0, sizeof(VIDEO_FRAME_INFO_S));
  ret |= CVI_VPSS_GetChnFrame(vpss_grpid, 0, p_dst_frm, 100);

  return ret;
}

void cviai_vpss1(int32_t argc, char **argv) {
  if(argc !=3){
    printf("test_vpss imgfile dstbin,current argc:,%d\n",argc);
    return;
  }
  CVI_S32 ret = CVI_SUCCESS;
  VIDEO_FRAME_INFO_S frame;

#ifdef ALI_OS
 
  VB_BLK blk_fr;	
  printf("to read image\n");
  if (CVI_SUCCESS != CVI_AI_ReadImage(argv[1], &blk_fr, &frame, PIXEL_FORMAT_RGB_888)){
    printf("read image failed\n");
    return;
  }

#else
  IVE_HANDLE ive_handle = CVI_IVE_CreateHandle();

  if (ive_handle == NULL) {
    printf("CreateHandle failed with %#x!\n", ret);
    return ;
  }
  
  IVE_IMAGE_S image = CVI_IVE_ReadImage(ive_handle, argv[1], IVE_IMAGE_TYPE_U8C3_PLANAR);
  if (image.u16Width == 0) {
    printf("read image failed:%s\n",argv[1]);
    return ;
  }

  ret = CVI_IVE_Image2VideoFrameInfo(&image, &frame, false);
  if (ret != CVI_SUCCESS) {
    // std::cout << "Convert to video frame failed with:" << ret << ",file:" << str_imgfile << std::endl;
    return ;
  }

#endif

  VPSS_GRP id = -1;

  cviai_handle_t ai_handle = NULL;
  ret = CVI_AI_CreateHandle(&ai_handle);
  if (ret != CVI_SUCCESS) {
    printf("Create ai handle failed with %#x!\n", ret);
    return;
  }
  VPSS_GRP *groups;
  uint32_t num;
  CVI_AI_GetVpssGrpIds(ai_handle,&groups, &num);
  printf("got numgroups:%d\n",num);
  id = groups[0];
  VPSS_CHN_ATTR_S vpss_chn_attr;

  CVI_FLOAT mean[3] = {0,0,0};
  CVI_FLOAT scale[3] = {0.501958,0.501958,0.501958};
  extract_VPSS_CHN_SQ_HELPER_1(&vpss_chn_attr,608,342,PIXEL_FORMAT_RGB_888_PLANAR,scale,mean,false);
  
  printf("got available vpssgrp:,%d\n",id);
  

  VIDEO_FRAME_INFO_S dst_frame;
  ret = extract_sendFrameBase_1(&frame,&vpss_chn_attr,1,id,&dst_frame);
  if(ret != CVI_SUCCESS){
    printf("failed to preprocess using vpss");
  }
  dump_vpss_frame_to_file(argv[2],&dst_frame);
  CVI_VPSS_ReleaseChnFrame(id, 0, &dst_frame);
#ifdef ALI_OS
  CVI_VB_ReleaseBlock(blk_fr);
#else
  CVI_SYS_FreeI(ive_handle, &image);
  CVI_IVE_DestroyHandle(ive_handle);
#endif
}

#ifdef ALI_OS
ALIOS_CLI_CMD_REGISTER(cviai_vpss1, cviai_vpss1, cviai_vpss1);
#else
int main(int argc, char *argv[]) { 
  CVI_S32 ret = CVI_SUCCESS;
  const CVI_S32 vpssgrp_width = 1920;
  const CVI_S32 vpssgrp_height = 1080;

  ret = MMF_INIT_HELPER2(vpssgrp_width, vpssgrp_height, PIXEL_FORMAT_RGB_888, 3,
                                 vpssgrp_width, vpssgrp_height, PIXEL_FORMAT_RGB_888_PLANAR, 3);
  if (ret != CVI_SUCCESS) {
    printf("Init sys failed with %#x!\n", ret);
    return ret;
  }
  test_vpss(argc,argv);
  return 0;
}
#endif



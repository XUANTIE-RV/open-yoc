#include <stdio.h>
#include "ai_utils.h"
#include <stdio.h>
// #include "cvi_vb.h"
// #include "cvi_vi.h"
#include "cvi_vo.h"
#include "cvi_comm_vpss.h"
#include "core/utils/vpss_helper.h"
#include <vfs.h>
// #define ENABLE_INIT_VI

#if 1
#define PIXEL_FORMAT_VO VI_PIXEL_FORMAT

#ifdef ENABLE_AI_APP_VO
#define VO_WIDTH 1280
#define VO_HEIGHT 720
#endif

CVI_S32 InitVPSS(VPSSConfigs *vpssConfigs, const CVI_BOOL isVOOpened);

CVI_S32 InitVideoSystem(VideoSystemContext *vs_ctx, SIZE_S *aiInputSize,
                        PIXEL_FORMAT_E aiInputFormat, int voType) {
  CVI_S32 s32Ret = CVI_SUCCESS;
  //****************************************************************
  // Init VI, VO, Vpss

//   load_ion_totalmem();

#ifdef ENABLE_INIT_VI
  SIZE_S viSize;
  s32Ret = InitVI(&vs_ctx->viConfig, &viSize, aiInputSize, aiInputFormat, &vs_ctx->ViPipe.DevNum);
  if (s32Ret != CVI_SUCCESS) {
    printf("Init video input failed with %d\n", s32Ret);
    return CVI_FAILURE;
  }

  if (vs_ctx->ViPipe.ViPipe >= vs_ctx->ViPipe.DevNum) {
    printf("Not enough devices. Found %u, required index %u.\n", vs_ctx->ViPipe.DevNum,
           vs_ctx->ViPipe.ViPipe);
    return CVI_FAILURE;
  }
#endif

  vs_ctx->vpssConfigs.vpssGrp = 0;
  vs_ctx->vpssConfigs.groupFormat = VI_PIXEL_FORMAT;
  vs_ctx->vpssConfigs.grpWidth = aiInputSize->u32Width;
  vs_ctx->vpssConfigs.grpHeight = aiInputSize->u32Height;
//   vs_ctx->vpssConfigs.grpWidth = viSize.u32Width;
//   vs_ctx->vpssConfigs.grpHeight = viSize.u32Height;

#ifdef ENABLE_AI_APP_VO
  // CHN for VO or encoding
  vs_ctx->vpssConfigs.voFormat = PIXEL_FORMAT_VO;
  vs_ctx->vpssConfigs.voWidth = VO_WIDTH;
  vs_ctx->vpssConfigs.voHeight = VO_HEIGHT;
  vs_ctx->vpssConfigs.vpssChnVideoOutput = VPSS_CHN1;
#endif

  // CHN for AI inference
  vs_ctx->vpssConfigs.aiFormat = aiInputFormat;
  vs_ctx->vpssConfigs.aiWidth = aiInputSize->u32Width;
  vs_ctx->vpssConfigs.aiHeight = aiInputSize->u32Height;
  vs_ctx->vpssConfigs.vpssChnAI = VPSS_CHN0;

  s32Ret = InitVPSS(&vs_ctx->vpssConfigs, voType != 0);
  if (s32Ret != CVI_SUCCESS) {
    printf("Init video process group 0 failed with %d\n", s32Ret);
    return CVI_FAILURE;
  }

#if ENABLE_AI_APP_VO
  if (voType) {
    OutputType outputType = voType == 1 ? OUTPUT_TYPE_PANEL : OUTPUT_TYPE_RTSP;
    s32Ret = InitOutput(outputType, vs_ctx->vpssConfigs.voWidth, vs_ctx->vpssConfigs.voHeight,
                        &vs_ctx->outputContext);
    if (s32Ret != CVI_SUCCESS) {
      printf("CVI_Init_Video_Output failed with %d\n", s32Ret);
      return CVI_FAILURE;
    }
  }
#endif

  return CVI_SUCCESS;
}

CVI_S32 InitVPSS(VPSSConfigs *vpssConfigs, const CVI_BOOL isVOOpened) {
  CVI_S32 s32Ret = CVI_SUCCESS;
  VPSS_GRP_ATTR_S stVpssGrpAttr;
  // CVI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
  VPSS_CHN_ATTR_S stVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];

  // abChnEnable[vpssConfigs->vpssChnAI] = CVI_TRUE;
  VPSS_CHN_DEFAULT_HELPER(&stVpssChnAttr[vpssConfigs->vpssChnAI], vpssConfigs->aiWidth,
                          vpssConfigs->aiHeight, vpssConfigs->aiFormat, true);

#ifdef ENABLE_AI_APP_VO
  if (isVOOpened) {
    // abChnEnable[vpssConfigs->vpssChnVideoOutput] = CVI_TRUE;
    VPSS_CHN_DEFAULT_HELPER(&stVpssChnAttr[vpssConfigs->vpssChnVideoOutput], vpssConfigs->voWidth,
                            vpssConfigs->voHeight, PIXEL_FORMAT_VO, true);
  }
#endif

  VPSS_GRP_DEFAULT_HELPER2(&stVpssGrpAttr, 1920, 1080, vpssConfigs->groupFormat, 0);

#if 0
  VPSS_GRP_DEFAULT_HELPER2(&stVpssGrpAttr, vpssConfigs->grpWidth, vpssConfigs->grpHeight,
                           vpssConfigs->groupFormat, 1);
#endif

  /*start vpss*/
  RETURN_IF_FAILED(CVI_VPSS_CreateGrp(vpssConfigs->vpssGrp, &stVpssGrpAttr), s32Ret);
  RETURN_IF_FAILED(CVI_VPSS_SetChnAttr(vpssConfigs->vpssGrp, vpssConfigs->vpssChnAI, &stVpssChnAttr[vpssConfigs->vpssChnAI]), s32Ret);
  RETURN_IF_FAILED(CVI_VPSS_EnableChn(vpssConfigs->vpssGrp, vpssConfigs->vpssChnAI), s32Ret);
  RETURN_IF_FAILED(CVI_VPSS_StartGrp(vpssConfigs->vpssGrp), s32Ret);
  MMF_CHN_S stSrcChn;
  MMF_CHN_S stDestChn;
  VI_DEV ViDev = 0;
  VI_CHN ViChn = 0;
  
  stSrcChn.enModId = CVI_ID_VI;
  stSrcChn.s32DevId = ViDev;
  stSrcChn.s32ChnId = ViChn;
  
  stDestChn.enModId = CVI_ID_VPSS;
  stDestChn.s32DevId = vpssConfigs->vpssGrp;
  stDestChn.s32ChnId = 0;
  RETURN_IF_FAILED(CVI_SYS_Bind(&stSrcChn, &stDestChn), s32Ret);

#if 0   /* old ai sample code */
  s32Ret = SAMPLE_COMM_VPSS_Init(vpssConfigs->vpssGrp, abChnEnable, &stVpssGrpAttr, stVpssChnAttr);
  if (s32Ret != CVI_SUCCESS) {
    printf("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
    return s32Ret;
  }

  s32Ret = SAMPLE_COMM_VPSS_Start(vpssConfigs->vpssGrp, abChnEnable, &stVpssGrpAttr, stVpssChnAttr);
  if (s32Ret != CVI_SUCCESS) {
    printf("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
    return s32Ret;
  }

  s32Ret = SAMPLE_COMM_VI_Bind_VPSS(vpssConfigs->vpssChnAI, vpssConfigs->vpssGrp);
  if (s32Ret != CVI_SUCCESS) {
    printf("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
    return s32Ret;
  }
#endif

  return s32Ret;
}



int dump_vpss_frame_to_file(const char *sz_file, VIDEO_FRAME_INFO_S *frame){
  int fd = aos_open(sz_file,  0100 | 02 | 01000);
  CVI_S32 s32Ret = CVI_SUCCESS;
	if (fd <= 0) {
		printf("aos_open dst file failed\n");
		return CVI_FAILURE;
	}
  aos_write(fd,&frame->stVFrame.u32Width,sizeof(uint32_t));
  aos_write(fd,&frame->stVFrame.u32Height,sizeof(uint32_t));

	for (int i = 0; i < 3; ++i) {
    size_t u32DataLen = frame->stVFrame.u32Stride[i] * frame->stVFrame.u32Height;
    if (u32DataLen == 0)
                    continue;
    if (i > 0 && ((frame->stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) ||
                    (frame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV12) ||
                    (frame->stVFrame.enPixelFormat == PIXEL_FORMAT_NV21)))
                    u32DataLen >>= 1;

    printf("plane(%d): paddr(%ld) vaddr(%p) stride(%d)\n",
                            i, frame->stVFrame.u64PhyAddr[i],
                            frame->stVFrame.pu8VirAddr[i],
                            frame->stVFrame.u32Stride[i]);
    printf("data_len(%ld) plane_len(%d)\n",
                                    u32DataLen, frame->stVFrame.u32Length[i]);
    s32Ret = aos_write(fd, (CVI_U8 *)frame->stVFrame.u64PhyAddr[i], u32DataLen);
    if (s32Ret <= 0) {
            printf("fwrite data(%d) error\n", i);
            break;
    }
	}
	aos_sync(fd);
	aos_close(fd);
  return s32Ret;
}

int dump_buffer_to_file(const char *sz_file,const uint8_t *p_buf,uint32_t size){
  int fd = aos_open(sz_file,  0100 | 02 | 01000);
  CVI_S32 s32Ret = CVI_SUCCESS;
	if (fd <= 0) {
		printf("aos_open dst file failed\n");
		return CVI_FAILURE;
	}
  printf("to dump buffer,len:%u\n",size);
  aos_write(fd,p_buf,size);
  aos_close(fd);
  return s32Ret;
}


#endif

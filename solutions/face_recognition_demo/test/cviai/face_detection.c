#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>
#include "cvi_vo.h"
#include "cviruntime.h"
#include "cviai.h"
#include "ai_utils.h"
#include "face_detection.h"
#include "fatfs_vfs.h"
#include <vfs.h>
#include "cvi_vpss.h"
#include "cvi_vb.h"
// #include "core/utils/vpss_helper.h"
#include "aos/cli.h"
#include <stdio.h>
#include <ulog/ulog.h>

int dump_face_info(const char *sz_file,cvai_face_t *p_face_meta){
  int fd = aos_open(sz_file,  0100 | 02 | 01000);
  CVI_S32 s32Ret = CVI_SUCCESS;
	if (fd <= 0) {
		printf("aos_open dst file failed\n");
		return CVI_FAILURE;
	}
  for(int i = 0; i < p_face_meta->size;i++){
    char szinfo[128];
    sprintf(szinfo,"x1y1x2y2=[%f,%f,%f,%f],score:%f\n",p_face_meta->info[i].bbox.x1,
    p_face_meta->info[i].bbox.y1,p_face_meta->info[i].bbox.x2,p_face_meta->info[i].bbox.y2,
    p_face_meta->info[i].bbox.score);
    aos_write(fd,szinfo,strlen(szinfo));
  }
  aos_close(fd);
  return s32Ret;
}

void cviai_face_detection(int32_t argc, char **argv) {
  if (argc != 1 && argc != 2) {
    printf("usage: cviai_fd <inference_num>\n");
    return;
  }
  int inference_num = -1;
  if (argc == 2) {
    inference_num = atoi(argv[1]);
    if (inference_num <= 0) {
      printf("inference_num must be larger than zero.\n");
      return;
    }
  }

  CVI_S32 s32Ret = CVIAI_SUCCESS;

  VideoSystemContext vs_ctx = {0};
  // SIZE_S aiInputSize = {.u32Width = 1920, .u32Height = 1080};
  SIZE_S aiInputSize = {.u32Width = 1280, .u32Height = 720};
  InitVideoSystem(&vs_ctx, &aiInputSize, VI_PIXEL_FORMAT, 0);

  const char *FD_MODEL_PATH = SD_FATFS_MOUNTPOINT"/retinaface_mnet0.25_342_608.cvimodel";
  printf("FD MODEL PATH = %s\n", FD_MODEL_PATH);

  cviai_handle_t ai_handle = NULL;
  s32Ret = CVI_AI_CreateHandle2(&ai_handle, 1, 0);
  printf("CVIAI CreateHandle Success.\n");

  s32Ret = CVI_AI_OpenModel(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, FD_MODEL_PATH);
  printf("CVIAI OpenModel Success.\n");

	VIDEO_FRAME_INFO_S stVideoFrame;
  /* Initialize VPSS config */
  VPSS_GRP vpssGrp = 0;
  VPSS_CHN vpssChnAI = VPSS_CHN0;

#ifdef USE_OUTSIDE_VPSS
  cvai_vpssconfig_t vpssconfig;
  // CVI_AI_GetVpssChnConfig(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, 1920, 1080, vpssChnAI, &vpssconfig);
  CVI_AI_GetVpssChnConfig(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, 1280, 720, vpssChnAI, &vpssconfig);
  // CVI_VPSS_SetGrpAttr(0, &vpss_grp_attr);
  CVI_VPSS_SetChnAttr(vpssGrp, vpssChnAI, &vpssconfig.chn_attr);
  CVI_AI_SetSkipVpssPreprocess(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, true);
  printf("CVIAI SetSkipVpssPreprocess Success.\n");
#else
  CVI_AI_SetSkipVpssPreprocess(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, false);
#endif
  aos_msleep(1000);

  cvai_face_t face_meta;
  memset(&face_meta, 0, sizeof(cvai_face_t));
  uint32_t counter = 0;
  while(inference_num == -1 || counter < inference_num){
	  printf("counter %u\n", counter);

    s32Ret = CVI_VPSS_GetChnFrame(vpssGrp, vpssChnAI, &stVideoFrame, 2000); 
	  if (s32Ret != CVI_SUCCESS) {
	  	printf("CVI_VPSS_GetChnFrame failed with %#x\n", s32Ret);
      break;
	  }
    printf("frame[%u]: width[%u], height[%u]\n", counter, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);

    if (counter > 0){
      CVI_AI_RetinaFace(ai_handle, &stVideoFrame, &face_meta);
    } else {
      printf("ignore counter 0 (workaround)\n");
    }
    CVI_AI_RescaleMetaRB(&stVideoFrame, &face_meta);

    printf("face meta: size[%u] (height[%u], width[%u], rescale_type[%x])\n", face_meta.size, 
           face_meta.height, face_meta.width, face_meta.rescale_type);
    for (uint32_t i = 0; i < face_meta.size; i++){
      printf("face[%u]: score[%.2f]\n", i, face_meta.info[i].bbox.score);
    }

    s32Ret = CVI_VPSS_ReleaseChnFrame(vpssGrp, vpssChnAI, &stVideoFrame);
    if (s32Ret != CVI_SUCCESS) {
      printf("CVI_VPSS_ReleaseChnFrame chn0 NG\n");
      break;
    }

    CVI_AI_Free(&face_meta);
    ++counter;
  }


  CVI_AI_DestroyHandle(ai_handle);
  LOGI(AI_TAG, "CVIAI Face Detection (END)");
}

ALIOS_CLI_CMD_REGISTER(cviai_face_detection, cviai_fd, cviai face detection);


void cviai_read_bin_face_detection(int32_t argc, char **argv) {
  if (argc != 1 && argc != 2) {
    printf("usage: cviai_fd <image_path>\n");
    return;
  }
  CVI_S32 s32Ret = CVI_SUCCESS;


  const char* image_path = argv[1];
  LOGI(AI_TAG, "CVIAI Face Detection (read image: %s)", image_path);


  const char *FD_MODEL_PATH = SD_FATFS_MOUNTPOINT"/retinaface_mnet0.25_342_608.cvimodel";
  printf("FD MODEL PATH = %s\n", FD_MODEL_PATH);

 // Init cviai handle.
  cviai_handle_t ai_handle = NULL;
  s32Ret = CVI_AI_CreateHandle2(&ai_handle,1, 0);
  if (s32Ret != CVI_SUCCESS) {
    printf("Create ai handle failed with %#x!\n", s32Ret);
    CVI_AI_DestroyHandle(ai_handle);
    return;
  }
  printf("CVIAI OpenModel Success.\n");
  s32Ret = CVI_AI_OpenModel(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, FD_MODEL_PATH);
  CVI_AI_SetSkipVpssPreprocess(ai_handle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, false);

  VB_BLK blk_fr;
	VIDEO_FRAME_INFO_S frame;
  printf("to read image\n");
  if (CVI_SUCCESS != CVI_AI_LoadBinImage(image_path, &blk_fr, &frame, PIXEL_FORMAT_RGB_888_PLANAR)){
    LOGE(AI_TAG, "cviai read image failed.");
    CVI_VB_ReleaseBlock(blk_fr);
    CVI_AI_DestroyHandle(ai_handle);
  }

  cvai_face_t face_meta;
  memset(&face_meta, 0, sizeof(cvai_face_t));
  LOGI(AI_TAG, "image read done,w:%d\n",frame.stVFrame.u32Width);
  CVI_AI_RetinaFace(ai_handle, &frame, &face_meta);
  LOGI(AI_TAG, "face meta: size[%u]\n",face_meta.size);

  printf("face_size:%u\n",face_meta.size);
  for (uint32_t i = 0; i < face_meta.size; ++i) {
    printf("x1y1x2y2=[%f,%f,%f,%f],score:%f\n",face_meta.info[i].bbox.x1 ,face_meta.info[i].bbox.y1,
           face_meta.info[i].bbox.x2,face_meta.info[i].bbox.y2,face_meta.info[i].bbox.score);
  }
  dump_face_info("/mnt/sd/fd_result.txt",&face_meta);
  CVI_AI_Free(&face_meta);
  
  LOGI(AI_TAG, "process done,w:%d\n",frame.stVFrame.u32Width);

  CVI_VB_ReleaseBlock(blk_fr);
  CVI_AI_DestroyHandle(ai_handle);
}

ALIOS_CLI_CMD_REGISTER(cviai_read_bin_face_detection, cviai_bin_fd, cviai_bin_fd);

//pre_processed_vpss.bin

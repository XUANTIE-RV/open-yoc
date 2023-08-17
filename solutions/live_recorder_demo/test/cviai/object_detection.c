
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>
// #include <rtos_malloc.h>
#include "cvi_vo.h"
#include "cviruntime.h"
#include "cviai.h"
#include "ai_utils.h"
#include "object_detection.h"
#include "fatfs_vfs.h"
#include <vfs.h>
#define USE_OUTSIDE_VPSS

#ifdef USE_OUTSIDE_VPSS
#include "core/utils/vpss_helper.h"
#endif

int dump_object_info(const char *sz_file,cvai_object_t *p_obj_meta){
  int fd = aos_open(sz_file,  0100 | 02 | 01000);
  CVI_S32 s32Ret = CVI_SUCCESS;
	if (fd <= 0) {
		printf("aos_open dst file failed\n");
		return CVI_FAILURE;
	}
  for(int i = 0; i < p_obj_meta->size;i++){
    char szinfo[128];
    sprintf(szinfo,"x1y1x2y2=[%f,%f,%f,%f],score:%f,class:%d\n",p_obj_meta->info[i].bbox.x1,
    p_obj_meta->info[i].bbox.y1,p_obj_meta->info[i].bbox.x2,p_obj_meta->info[i].bbox.y2,
    p_obj_meta->info[i].bbox.score,p_obj_meta->info[i].classes);
    aos_write(fd,szinfo,strlen(szinfo));
  }
  aos_close(fd);
  return s32Ret;
}

void cviai_object_detection(int32_t argc, char **argv) {
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
  LOGI(AI_TAG, "CVIAI Object Detection (BEGIN)");
  CVI_S32 s32Ret = CVIAI_SUCCESS;

  VideoSystemContext vs_ctx = {0};
  // SIZE_S aiInputSize = {.u32Width = 1920, .u32Height = 1080};
  SIZE_S aiInputSize = {.u32Width = 1280, .u32Height = 720};
  InitVideoSystem(&vs_ctx, &aiInputSize, VI_PIXEL_FORMAT, 0);

  const char *OD_MODEL_PATH = SD_FATFS_MOUNTPOINT"/mobiledetv2-vehicle-d0-ls.cvimodel";
  printf("OD MODEL PATH = %s\n", OD_MODEL_PATH);

  cviai_handle_t ai_handle = NULL;
  s32Ret = CVI_AI_CreateHandle2(&ai_handle, 1, 0);
  printf("CVIAI CreateHandle Success.\n");


  s32Ret = CVI_AI_OpenModel(ai_handle, CVI_AI_SUPPORTED_MODEL_MOBILEDETV2_VEHICLE, OD_MODEL_PATH);
  printf("CVIAI OpenModel Success.\n");

  VIDEO_FRAME_INFO_S stVideoFrame;
  /* Initialize VPSS config */
  VPSS_GRP vpssGrp = 0;
  VPSS_CHN vpssChnAI = VPSS_CHN0;

#ifdef USE_OUTSIDE_VPSS
  cvai_vpssconfig_t vpssconfig;
  // CVI_AI_GetVpssChnConfig(ai_handle, CVI_AI_SUPPORTED_MODEL_YOLOX, 1920, 1080, vpssChnAI, &vpssconfig);
  CVI_AI_GetVpssChnConfig(ai_handle, CVI_AI_SUPPORTED_MODEL_YOLOX, 1280, 720, vpssChnAI, &vpssconfig);
  // CVI_VPSS_SetGrpAttr(0, &vpss_grp_attr);
  CVI_VPSS_SetChnAttr(vpssGrp, vpssChnAI, &vpssconfig.chn_attr);
  CVI_AI_SetSkipVpssPreprocess(ai_handle, CVI_AI_SUPPORTED_MODEL_YOLOX, true);
  printf("CVIAI SetSkipVpssPreprocess Success.\n");
#else
  CVI_AI_SetSkipVpssPreprocess(ai_handle, CVI_AI_SUPPORTED_MODEL_YOLOX, false);
#endif
  aos_msleep(1000);

  cvai_object_t obj_meta;
  memset(&obj_meta, 0, sizeof(cvai_object_t));
  int counter = 0;
  while(inference_num == -1 || counter < inference_num){
	  printf("counter %u\n", counter);

    s32Ret = CVI_VPSS_GetChnFrame(vpssGrp, vpssChnAI, &stVideoFrame, 2000); 
	  if (s32Ret != CVI_SUCCESS) {
	  	printf("CVI_VPSS_GetChnFrame failed with %#x\n", s32Ret);
      break;
	  }
    printf("frame[%u]: height[%u], width[%u]\n", counter, stVideoFrame.stVFrame.u32Height, stVideoFrame.stVFrame.u32Width);

    if (counter > 0){
      CVI_AI_MobileDetV2_Vehicle(ai_handle, &stVideoFrame, &obj_meta);
    } else {
      printf("ignore counter 0 (workaround)\n");
    }

    printf("[OD] obj count: %u\n", obj_meta.size);
    for (uint32_t i = 0; i < obj_meta.size; i++){
      printf("obj[%u]: class[%d], score[%.2f]\n", i, obj_meta.info[i].classes, obj_meta.info[i].bbox.score);
    }

    s32Ret = CVI_VPSS_ReleaseChnFrame(vpssGrp, vpssChnAI, &stVideoFrame);
    if (s32Ret != CVI_SUCCESS) {
      printf("CVI_VPSS_ReleaseChnFrame chn0 NG\n");
      break;
    }

    CVI_AI_Free(&obj_meta);
    ++counter;
  }

  CVI_AI_DestroyHandle(ai_handle);

  LOGI(AI_TAG, "CVIAI Object Detection (END)");
}

ALIOS_CLI_CMD_REGISTER(cviai_object_detection, cviai_od, cviai object detection);

void cviai_read_vd_bin(int32_t argc, char **argv) {
  if (argc != 1 && argc != 2) {
    printf("usage: cviai_fd <image_path>\n");
    return;
  }
  const char* image_path = argv[1];
  LOGI(AI_TAG, "CVIAI Vehicle Detection (read image: %s)", image_path);
  // CVI_S32 s32Ret = CVIAI_SUCCESS;


  const char *OD_MODEL_PATH = SD_FATFS_MOUNTPOINT"/mobiledetv2-vehicle-d0-ls.cvimodel";
  printf("OD MODEL PATH = %s\n", OD_MODEL_PATH);

  cviai_handle_t ai_handle = NULL;
  CVI_AI_CreateHandle2(&ai_handle, 1, 0);
  printf("CVIAI CreateHandle Success.\n");


  CVI_AI_OpenModel(ai_handle, CVI_AI_SUPPORTED_MODEL_MOBILEDETV2_VEHICLE, OD_MODEL_PATH);
  printf("CVIAI OpenModel Success.\n");

  CVI_AI_SetSkipVpssPreprocess(ai_handle, CVI_AI_SUPPORTED_MODEL_MOBILEDETV2_VEHICLE, false);

  
  VB_BLK blk_fr;
	VIDEO_FRAME_INFO_S frame;
  printf("to read image\n");
  if (CVI_SUCCESS != CVI_AI_LoadBinImage(image_path, &blk_fr, &frame, PIXEL_FORMAT_RGB_888_PLANAR)){
    LOGE(AI_TAG, "cviai read image failed.");
    CVI_VB_ReleaseBlock(blk_fr);
    CVI_AI_DestroyHandle(ai_handle);
  }
  cvai_object_t vehicle_obj;
  memset(&vehicle_obj, 0, sizeof(cvai_object_t));

  CVI_AI_MobileDetV2_Vehicle(ai_handle, &frame, &vehicle_obj);
  printf("vehicle meta: size[%u], width[%u], height[%u], rescale_type[%d]\n", 
         vehicle_obj.size, vehicle_obj.width, vehicle_obj.height, vehicle_obj.rescale_type);
  for (uint32_t i = 0; i < vehicle_obj.size; ++i) {
    printf("vehicle[%u]: x1y1x2y2[%.2f,%.2f,%.2f,%.2f],class:%d\n", vehicle_obj.size,
           vehicle_obj.info[i].bbox.x1, vehicle_obj.info[i].bbox.y1,
           vehicle_obj.info[i].bbox.x2, vehicle_obj.info[i].bbox.y2,vehicle_obj.info[i].classes);
  }
  dump_object_info("/mnt/sd/vd_result.txt",&vehicle_obj);
  CVI_AI_Free(&vehicle_obj);
  CVI_VB_ReleaseBlock(blk_fr);
  
  CVI_AI_DestroyHandle(ai_handle);

  LOGI(AI_TAG, "CVIAI Face Detection (END)");
}

ALIOS_CLI_CMD_REGISTER(cviai_read_vd_bin, cviai_read_vd_bin, cviai_read_vd_bin);
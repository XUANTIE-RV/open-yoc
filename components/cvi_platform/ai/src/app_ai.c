#include <stdio.h>
#include <dirent.h>
#include <aos/kernel.h>
#include <pthread.h>
#include <unistd.h>
#include "fatfs_vfs.h"
#include <math.h>
#include <aos/cli.h>
#include <sys/stat.h>
#include "vfs.h"
#include "app_ai.h"
#include "cviai.h"
#include "cvi_tpu_interface.h"
#include "cvi_vpss.h"
#include "cvi_param.h"
#include "gui_display.h"
#include "lvgl.h"
#include "cvi_isp.h"
#include "cvi_ae.h"
#include "cvi_param.h"
#include "drv/tick.h"

#define GALLERYDIR "/mnt/sd/gallery"
#define RETINAFACEMODELFILE "/mnt/sd/retinaface_mnet0.25_342_608.cvimodel"
#define CVIFACEMODELFILE "/mnt/sd/cviface-v5-s.cvimodel"

static pthread_mutex_t g_AiFaceMutex;
static pthread_t g_AiThreadid;
static int g_RunStatus;
static cvai_feature_t g_AiFaceFeature = {0};
static cviai_handle_t g_AiHandle = NULL;
#ifndef CVIAI_VPSS_USE_GRP
#define CVIAI_VPSS_USE_GRP 0
#endif
#ifndef CVIAI_VPSS_USE_CHN
#define CVIAI_VPSS_USE_CHN 2
#endif
#ifndef CVIAI_VPSS_DEV
#define CVIAI_VPSS_DEV 0
#endif
#ifndef CVIAI_VPSS_CREATE_GRP
#define CVIAI_VPSS_CREATE_GRP 15
#endif
#define CVIAI_FACEAE_PIPE 0
#define CVIAI_FACEAE_KEEPFRAME_NUM 25 //FACE效果保持多少帧
#define CVIAI_WEIGHT 0.35
#define CVIAI_FACEAE_RECOGNITION_MAXNUM 5 //FACE识别最多人数
#if 0
static int ai_feature_copy(cvai_feature_t *src, cvai_feature_t *dst)
{
    if(dst->ptr) {
        free(dst->ptr);
        dst->ptr = NULL;
    }
    dst->ptr = (int8_t *)malloc(src->size);
    if(dst->ptr == NULL) {
        return CVI_FAILURE;
    }
    memcpy(dst->ptr,src->ptr,src->size);
    dst->size = src->size;
    printf("dst->size is %d \n",dst->size);
    dst->type = src->type;
    return CVI_SUCCESS;
}
#endif

static int ai_load_feature(const char *sz_file,cvai_feature_t *p_feat,uint32_t size)
{
    int fd = aos_open(sz_file, O_RDONLY);
    if(fd <0){
        printf("CVIAI open fature file failed,%s\n",sz_file);
        return -1;
    }
    memset(p_feat,0,sizeof(cvai_feature_t));
    p_feat->ptr = aos_malloc(size);
    p_feat->size = size;
    int nbytesread = aos_read(fd, p_feat->ptr, size);
    if(nbytesread != size){
        aos_close(fd);
        free(p_feat->ptr);
        p_feat->ptr = NULL;
        return -1;
    }
    aos_sync(fd);
    aos_close(fd);
    return 0;
}

static void ai_destory_feature(cvai_feature_t *p_feat)
{
    if(p_feat->ptr) {
        free(p_feat->ptr);
        p_feat->ptr = NULL;
        p_feat->size = 0;
        p_feat->type = 0;
    }
}

static CVI_S32 ai_cal_cos_sim(cvai_feature_t *a, cvai_feature_t *b, float *score)
{
    if (a->ptr == NULL || b->ptr == NULL || a->size != b->size) {
        return CVI_FAILURE;
    }
    float A = 0, B = 0, AB = 0;
    for (uint32_t i = 0; i < a->size; i++) {
        A += (int)a->ptr[i] * (int)a->ptr[i];
        B += (int)b->ptr[i] * (int)b->ptr[i];
        AB += (int)a->ptr[i] * (int)b->ptr[i];
    }
    A = sqrt(A);
    B = sqrt(B);
    *score = AB / (A * B);
    return CVI_SUCCESS;
}


static int ai_dump_buffer_to_file(const char *sz_file,const uint8_t *p_buf,uint32_t size)
{
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

static void APP_Ai_Handler(cvai_face_t *pfaceMeta,CVI_S32 *presult)
{
    aos_dir_t *dirp = NULL;
    char * tmpPoint = NULL;
    char filePath[128] = {0};
    char str[1024] = {0};
    int recognition_Number = 0;
    int pos = 0;

#if (CONFIG_FACEAE_SUPPORT == 1)
    ISP_SMART_INFO_S stSmartInfo = {0};
    ISP_SMART_EXPOSURE_ATTR_S stSmartExpAttr = {0};
    CVI_ISP_GetSmartExposureAttr(0,&stSmartExpAttr);
#endif

    if(pfaceMeta->size <= 0) {
#if (CONFIG_FACEAE_SUPPORT == 1)
        if(stSmartExpAttr.bEnable == 1) {
            //stSmartInfo.stROI[0].bEnable = 1;
            //stSmartInfo.stROI[0].bAvailable = 0;//无人脸的时候取消
            //CVI_ISP_SetSmartInfo(CVIAI_FACEAE_PIPE,&stSmartInfo,CVIAI_FACEAE_KEEPFRAME_NUM);
        }
#endif
        return ;
    } else {
#if (CONFIG_FACEAE_SUPPORT == 1)
        if(stSmartExpAttr.bEnable == 1) {
            //只需要设定stROI[0]
            stSmartInfo.stROI[0].bEnable = 1;
            stSmartInfo.stROI[0].bAvailable = 1;
            stSmartInfo.stROI[0].u8Num = pfaceMeta->size > SMART_MAX_NUM ? SMART_MAX_NUM : pfaceMeta->size;
            for(int i = 0; i < stSmartInfo.stROI[0].u8Num; i++) {
                stSmartInfo.stROI[0].u16PosX[i] = (CVI_U16)(pfaceMeta->info[i].bbox.x1);
                stSmartInfo.stROI[0].u16PosY[i] = (CVI_U16)(pfaceMeta->info[i].bbox.y1);
                stSmartInfo.stROI[0].u16Width[i] = (CVI_U16)(pfaceMeta->info[i].bbox.x2 - pfaceMeta->info[i].bbox.x1);
                stSmartInfo.stROI[0].u16Height[i] = (CVI_U16)(pfaceMeta->info[i].bbox.y2 - pfaceMeta->info[i].bbox.y1);
                //printf("The stSmartInfo.stROI[0].u16PosX[%d]is %d\n",i,stSmartInfo.stROI[0].u16PosX[i]);
                //printf("The stSmartInfo.stROI[0].u16PosY[%d]is %d\n",i,stSmartInfo.stROI[0].u16PosY[i]);
                //printf("The stSmartInfo.stROI[0].u16Width[%d]is %d\n",i,stSmartInfo.stROI[0].u16Width[i]);
                //printf("The stSmartInfo.stROI[0].u16Height[%d]is %d\n",i,stSmartInfo.stROI[0].u16Height[i]);
            }
            stSmartInfo.stROI[0].u16FrameWidth = (CVI_U16)pfaceMeta->width;//输入坐标frame的宽度
            stSmartInfo.stROI[0].u16FrameHeight = (CVI_U16)pfaceMeta->height;//输入坐标frame的高度
            CVI_ISP_SetSmartInfo(CVIAI_FACEAE_PIPE,&stSmartInfo,CVIAI_FACEAE_KEEPFRAME_NUM);
        }
#endif
    }
    dirp = aos_opendir(GALLERYDIR);
    if (dirp == NULL) {
        return ;
    }
    for (; ;) {
        aos_dirent_t *entryp = aos_readdir(dirp);
        if (entryp == NULL) {
            /* Finished with this directory */
            break;
        }
        if (entryp->d_type == DT_DIR || strcmp(entryp->d_name, ".") == 0 || strcmp(entryp->d_name, "..") == 0) {
            continue;
        }
        /* Call the handler with this directory entry */
        tmpPoint = strstr(entryp->d_name,"face_");
        if (tmpPoint) {
            float score = 0;
            float maxScore = 0;
            snprintf(filePath,sizeof(filePath),"%s/%s",GALLERYDIR,entryp->d_name);
            if(ai_load_feature(filePath,&g_AiFaceFeature,256) == 0) {
                for (uint32_t i = 0; i < pfaceMeta->size; i++) {
                    if (g_AiFaceFeature.ptr != NULL) {
                        if(ai_cal_cos_sim(&(pfaceMeta->info[i].feature), &g_AiFaceFeature,
                        &score) == 0) {
                            if (score > CVIAI_WEIGHT && recognition_Number < CVIAI_FACEAE_RECOGNITION_MAXNUM) {
                                if (maxScore < score) {
                                    maxScore = score;
                                }
                            }
                        }
                    }
                }
                if (maxScore != 0) {
                    pos += sprintf(str+pos, "%s score %3.2f\n",tmpPoint,maxScore);
                    recognition_Number ++;
                }
                ai_destory_feature(&g_AiFaceFeature);
            }
        }
    }
    if (recognition_Number > 0) {
        #if (CONFIG_APP_GUI_SUPPORT == 1)
        Gui_Label_SendEvent(LV_EVENT_PRESSED,str);
        #endif
        *presult = 1;
    }
    aos_closedir(dirp);
}

void *APP_Ai_Task(void *arg)
{
    CVI_S32 s32Ret = 0;
    cvai_face_t faceMeta;
    VIDEO_FRAME_INFO_S stfdFrame;
    CVI_S32 s32Count = 0;
    CVI_S32 s32Result = 0;
    g_RunStatus = 1;
    pthread_mutex_init(&g_AiFaceMutex, NULL);
    cvi_tpu_init();
    aos_msleep(1000);
    if (access(RETINAFACEMODELFILE,F_OK) != 0) {
        printf("retinaface_mnet0.25_342_608.cvimodel place in SD care\n");
        goto EXIT;
    }
    if (access(CVIFACEMODELFILE,F_OK) != 0) {
        printf("cviface-v5-s.cvimodel place in SD care\n");
        goto EXIT;
    }
    s32Ret = CVI_AI_CreateHandle2(&g_AiHandle, CVIAI_VPSS_CREATE_GRP, CVIAI_VPSS_DEV);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_AI_CreateHandle2 err s32Ret:%d\n",s32Ret);
        goto EXIT;
    }
    s32Ret = CVI_AI_OpenModel(g_AiHandle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, RETINAFACEMODELFILE);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_AI_OpenModel err s32Ret:%d\n",s32Ret);
        goto EXIT;
    }
    s32Ret = CVI_AI_OpenModel(g_AiHandle, CVI_AI_SUPPORTED_MODEL_FACERECOGNITION, CVIFACEMODELFILE);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_AI_OpenModel err s32Ret:%d\n",s32Ret);
        goto EXIT;
    }
    CVI_AI_SetSkipVpssPreprocess(g_AiHandle, CVI_AI_SUPPORTED_MODEL_RETINAFACE, false);
    CVI_AI_SetSkipVpssPreprocess(g_AiHandle, CVI_AI_SUPPORTED_MODEL_FACERECOGNITION, false);
    CVI_AI_SetVpssTimeout(g_AiHandle, 2000);
#if (CONFIG_APP_AI_ATTACHVBPOOL && CONFIG_APP_AI_ATTACHVBPOOL > 0)
    CVI_AI_SetVBPool(g_AiHandle, 0, CONFIG_APP_AI_ATTACHVBPOOL);
#endif
    while (g_RunStatus) {
        pthread_mutex_lock(&g_AiFaceMutex);
        s32Ret = CVI_VPSS_GetChnFrame(CVIAI_VPSS_USE_GRP, CVIAI_VPSS_USE_CHN,
                                    &stfdFrame, 2000);
        if (s32Ret != CVI_SUCCESS) {
            usleep(1000);
            pthread_mutex_unlock(&g_AiFaceMutex);
            continue;
        }
        memset(&faceMeta, 0, sizeof(cvai_face_t));
        CVI_AI_RetinaFace(g_AiHandle, &stfdFrame, &faceMeta);
        CVI_AI_FaceRecognition(g_AiHandle, &stfdFrame, &faceMeta);
        s32Ret = CVI_VPSS_ReleaseChnFrame(CVIAI_VPSS_USE_GRP, CVIAI_VPSS_USE_CHN, &stfdFrame);
        if (s32Ret != CVI_SUCCESS) {
            printf("[err] app_ai CVI_VPSS_ReleaseChnFrame err %d\n", s32Ret);
        }
        pthread_mutex_unlock(&g_AiFaceMutex);
        APP_Ai_Handler(&faceMeta,&s32Result);
        if (faceMeta.size == 0 && s32Result == 1) {
            s32Count ++;
            if(s32Count >= 30) {
                s32Result = 0;
                s32Count = 0;
#if (CONFIG_APP_GUI_SUPPORT == 1)
                Gui_Label_SendEvent(LV_EVENT_PRESS_LOST,NULL);
#endif
            }
        }
        CVI_AI_Free(&faceMeta);
    }
EXIT:
    g_RunStatus = 0;
    if (g_AiHandle) {
        CVI_AI_CloseAllModel(g_AiHandle);
        CVI_AI_DestroyHandle(g_AiHandle);
        g_AiHandle = NULL;
    }
    cvi_tpu_deinit();
    pthread_mutex_destroy(&g_AiFaceMutex);
    return 0;
}


CVI_S32 APP_AiStart()
{
    if(PARAM_getPipeline() == 1) {
        printf("APP_Ai_Task not support ir pipeline \n");
        return CVI_SUCCESS;
    }
    int s32Ret = 0;
    pthread_attr_t TaskAttr = {0};
    s32Ret = pthread_attr_init(&TaskAttr);
    s32Ret |= pthread_attr_setstacksize(&TaskAttr, 8192);
    s32Ret |= pthread_create(&g_AiThreadid, &TaskAttr, APP_Ai_Task, NULL);
    if(s32Ret < 0) {
        printf("pthread_create APP_Ai_Task err \n");
        return CVI_FAILURE;
    }
    pthread_setname_np(g_AiThreadid,"app_ai_task");
    return CVI_SUCCESS;
}

void APP_AiStop()
{
    if(g_RunStatus == 1) {
        g_RunStatus = 0;
        pthread_join(g_AiThreadid,NULL);
    }
}

void APP_AI_Register_Face(int argc,char **argv)
{
    CVI_S32 s32Ret = 0;
    cvai_face_t faceMeta = {0};
    VIDEO_FRAME_INFO_S stfdFrame;
    char FileName[128] = {0};

    if(argc < 2) {
        printf("Please Enter FaceFeature Name \n");
        printf("Example :AI_Register_Face wuyifan \n");
        return ;
    }
    if(access(GALLERYDIR,F_OK) != 0) {
        mkdir(GALLERYDIR,0777);
    }
    snprintf(FileName,sizeof(FileName),"%s/face_%s",GALLERYDIR,argv[1]);
    if(g_RunStatus == 0 ) {
        return ;
    }
    pthread_mutex_lock(&g_AiFaceMutex);
    s32Ret = CVI_VPSS_GetChnFrame(CVIAI_VPSS_USE_GRP, CVIAI_VPSS_USE_CHN,
                                    &stfdFrame, 4000);
    if(s32Ret != CVI_SUCCESS) {
        printf("GetFrame failed Register Face failed \n");
        pthread_mutex_unlock(&g_AiFaceMutex);
        return ;
    }
    memset(&faceMeta, 0, sizeof(cvai_face_t));
    CVI_AI_RetinaFace(g_AiHandle, &stfdFrame, &faceMeta);
    CVI_AI_FaceRecognition(g_AiHandle, &stfdFrame, &faceMeta);
    if(faceMeta.size == 1) {
        const uint8_t*ptr = (const uint8_t*)faceMeta.info[0].feature.ptr;
        ai_dump_buffer_to_file(FileName,ptr,faceMeta.info[0].feature.size);
        printf("Register Face success \n");
    } else {
        printf("Register Face failed can't find face\n");
    }
    CVI_AI_Free(&faceMeta);
    CVI_VPSS_ReleaseChnFrame(CVIAI_VPSS_USE_GRP, CVIAI_VPSS_USE_CHN,&stfdFrame);
    pthread_mutex_unlock(&g_AiFaceMutex);
}
ALIOS_CLI_CMD_REGISTER(APP_AI_Register_Face,AI_Register_Face,CVI_AI_Register_Face)

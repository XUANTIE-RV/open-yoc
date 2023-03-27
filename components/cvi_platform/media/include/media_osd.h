#ifndef APP_OSD_H
#define APP_OSD_H

#include <stdio.h>
#include "cvi_type.h"
#include "cvi_comm_region.h"
#include "cvi_comm_video.h"
#include "cvi_comm_vpss.h"
#include <pthread.h>
#include <cvi_region.h>
#include <cvi_vpss.h>

typedef enum {
    APP_MEDIAOSD_TIMESTAMP,
    APP_MEDIAOSD_STRING,
    APP_MEDIAOSD_PICTURE,
    APP_MEDIAOSD_MAX,
} APP_MEDIAOSD_E;

typedef enum {
    APP_OSDTYPE_CHAR,
    APP_OSDTYPE_PICTURE,
    APP_OSDTYPE_MAX,
} APP_MEDIAOSDTYPE_E;

typedef struct {
    CVI_S32 s32OsdDevEnable;
    CVI_S32 s32Enable;
    CVI_S32 s32Handle;
    APP_MEDIAOSDTYPE_E s32Type;
    RECT_S Pos;
    MMF_CHN_S stChn;
    ROTATION_E Rotation;
    PIXEL_FORMAT_E PixelFormat;
    CVI_CHAR *String;
    CVI_CHAR *PicFilePath;
    pthread_t PthreadId;
} APP_OSD_S;



CVI_S32 APP_OSD_Init(void);
CVI_S32 APP_OSD_DeInit(void);
CVI_S32 APP_OSD_Create(APP_OSD_S *pstAppOsd);
CVI_S32 APP_OSD_Destory(APP_OSD_S *pstAppOsd);
CVI_S32 APP_OSD_SetString(APP_OSD_S *pstAppOsd, CVI_S32 s32Handle, CVI_CHAR *String);
CVI_S32 APP_OSD_SetPicture(APP_OSD_S *pstAppOsd, CVI_S32 s32Handle, CVI_CHAR *PictureFilePath);
CVI_S32 APP_OSD_GetAttr(APP_MEDIAOSD_E OsdType, APP_OSD_S *pstAppOsd);
CVI_S32 APP_OSD_SetAttr(APP_MEDIAOSD_E OsdType, const APP_OSD_S *pstAppOsd);





#endif

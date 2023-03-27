#include "media_osd.h"
#include <sys/prctl.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "loadbmp.h"
#include <math.h>
#include "fatfs_vfs.h"
#if (CONFIG_DEBUG_CVITEST_CLIMODE == 0)
#include <fontmod.h>
#else
extern unsigned char g_fontLib[];
#if (CONFIG_SIMPLE_FONTMOD == 1)
#define USE_SIMPLE_FONTMOD  1
#else
#define USE_SIMPLE_FONTMOD  0
#endif
#endif
#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#if !defined(MAX)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#define BYTE_BITS				8
#define NOASCII_CHARACTER_BYTES 2
#define OSD_LIB_FONT_W			24
#define OSD_LIB_FONT_H			24
#define OSD_LEN_MAXSIZE         64
#define IsASCII(a)              (((a) >= 0x00 && (a) <= 0x7F) ? 1 : 0)
#define STRING_X 60 //相对与Vpss左侧开始计算 加上字符串长度不可以超过最大宽度
#define STRING_Y 20 //相对与Vpss左侧开始计算 加上字符串高度不可以超过最大高度
#define PICTUREFILENAME SD_FATFS_MOUNTPOINT"/dog.bmp"
#define PICTURE_X  20 //相对与Vpss左侧开始计算 加上图片长度不可以超过最大宽度
#define PICTURE_Y  20 //相对与Vpss顶部开始计算 加上图片高度不可以超过最大高度
#define OSDSTRING "00:00:00"

static APP_OSD_S s_APP_OSD_Manager[APP_MEDIAOSD_MAX] = {
    {
        .s32OsdDevEnable = 1,
        .s32Enable = 0,
        .s32Handle = APP_MEDIAOSD_TIMESTAMP,
        .s32Type = APP_OSDTYPE_CHAR,
        .Pos.s32X = 0,
        .Pos.s32Y = 0,
        .PixelFormat = PIXEL_FORMAT_ARGB_1555,
        .stChn = {
            .enModId = CVI_ID_VPSS,
            .s32ChnId = 1,
            .s32DevId = 0,
        }
    },
    {
        .s32OsdDevEnable = 0, //REC录像
        .s32Enable = 0,
        .s32Handle = APP_MEDIAOSD_STRING,
        .s32Type = APP_OSDTYPE_CHAR,
        .Pos.s32X = STRING_X,
        .Pos.s32Y = STRING_Y,
        .String = OSDSTRING,
        .PixelFormat = PIXEL_FORMAT_ARGB_1555,
        .stChn = {
            .enModId = CVI_ID_VPSS,
            .s32ChnId = 0,
            .s32DevId = 0,
        }
    },
    {
        .s32OsdDevEnable = 0,//默认不启用
        .s32Enable = 0,
        .s32Handle = APP_MEDIAOSD_PICTURE,
        .s32Type = APP_OSDTYPE_PICTURE,
        .Pos.s32X = PICTURE_X,
        .Pos.s32Y = PICTURE_Y,
        .Pos.u32Width = 72,
        .Pos.u32Height = 60,
        .PicFilePath = PICTUREFILENAME,
        .PixelFormat = PIXEL_FORMAT_ARGB_1555,
        .stChn = {
            .enModId = CVI_ID_VPSS,
            .s32ChnId = 0,
            .s32DevId = 0,
        }
    },
};

static int MediaOsd_GetNonASCNum(char *string, int len)
{
    int i;
    int n = 0;

    for (i = 0; i < len; i++) {
        if (string[i] == '\0')
            break;
        if (!IsASCII(string[i])) {
            i++;
            n++;
        }
    }

    return n;
}

static CVI_S32 APP_OSD_GetFontMod(CVI_CHAR *Character, CVI_U8 **FontMod, CVI_S32 *FontModLen)
{
    CVI_U32 offset = 0;
    CVI_U32 areacode = 0;
    CVI_U32 bitcode = 0;
    #if (USE_SIMPLE_FONTMOD == 1)
    if ((Character[0] < 0x20) || (Character[0] > 0x3A)) {
        printf("Unsupported character!\n");
        return CVI_FAILURE;
    }
    areacode = 1;
    bitcode = (uint32_t)((uint8_t)Character[0] - 0x1F);
    #else
    if (IsASCII(Character[0])) {
        areacode = 3;
        bitcode = (CVI_U32)((CVI_U8)Character[0] - 0x20);
    } else {
        areacode = (CVI_U32)((CVI_U8)Character[0] - 0xA0);
        bitcode = (CVI_U32)((CVI_U8)Character[1] - 0xA0);
    }
    #endif
    offset = (94 * (areacode - 1) + (bitcode - 1)) * (OSD_LIB_FONT_W * OSD_LIB_FONT_H / 8);
    *FontMod = (CVI_U8 *)g_fontLib + offset;
    *FontModLen = OSD_LIB_FONT_W * OSD_LIB_FONT_H / 8;

    return CVI_SUCCESS;
}

static void APP_OSD_GetTimeStr(const struct tm *pstTime, char *pazStr, int s32MaxLen)
{
    time_t nowTime;
    struct tm stTime = {
        0,
    };

    if (!pstTime) {
        time(&nowTime);
        gmtime_r(&nowTime, &stTime);
        pstTime = &stTime;
    }

    snprintf(pazStr, s32MaxLen, "%04d-%02d-%02d %02d:%02d:%02d",
             pstTime->tm_year + 1900, pstTime->tm_mon + 1, pstTime->tm_mday,
             pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec);
}

static int APP_OSD_UpdateBitmap(RGN_HANDLE RgnHdl, CVI_CHAR *szStr, BITMAP_S *pstBitmap,
    CVI_U32 *pu32Color,ROTATION_E Rotation)
{
    CVI_S32 s32Ret;
    CVI_U32 u32CanvasWidth, u32CanvasHeight, u32BgColor, u32Color;
    SIZE_S stFontSize;
    CVI_S32 s32StrLen = strlen(szStr);
    CVI_S32 NonASCNum = MediaOsd_GetNonASCNum(szStr, s32StrLen);
    CVI_U16 *puBmData = (CVI_U16 *)pstBitmap->pData;
    CVI_U32 u32BmRow, u32BmCol;

    u32CanvasWidth = OSD_LIB_FONT_W * (s32StrLen - NonASCNum * (NOASCII_CHARACTER_BYTES - 1));
    u32CanvasHeight = OSD_LIB_FONT_H;
    stFontSize.u32Width = OSD_LIB_FONT_W;
    stFontSize.u32Height = OSD_LIB_FONT_H;
    u32BgColor = 0x7fff;

    for (u32BmRow = 0; u32BmRow < u32CanvasHeight; ++u32BmRow) {
        CVI_S32 NonASCShow = 0;

        for (u32BmCol = 0; u32BmCol < u32CanvasWidth; ++u32BmCol) {
            CVI_S32 s32BmDataIdx = u32BmRow * pstBitmap->u32Width + u32BmCol;
            CVI_S32 s32CharIdx = u32BmCol / stFontSize.u32Width;
            CVI_S32 s32StringIdx = s32CharIdx + NonASCShow * (NOASCII_CHARACTER_BYTES - 1);
            if (Rotation == ROTATION_90) {
                s32BmDataIdx = ((u32CanvasWidth-1) - u32BmCol) * pstBitmap->u32Width + u32BmRow;
            } else if (Rotation == ROTATION_180) {
                s32BmDataIdx = ((u32CanvasHeight-1) - u32BmRow) * pstBitmap->u32Width + ((u32CanvasWidth-1) - u32BmCol);
            } else if (Rotation == ROTATION_270) {
                s32BmDataIdx = u32BmCol * pstBitmap->u32Width + ((u32CanvasHeight-1) - u32BmRow);
            }
            if (NonASCNum > 0 && s32CharIdx > 0) {
                NonASCShow = MediaOsd_GetNonASCNum(szStr, s32StringIdx);
                s32StringIdx = s32CharIdx + NonASCShow * (NOASCII_CHARACTER_BYTES - 1);
            }
            CVI_S32 s32CharCol = (u32BmCol - (stFontSize.u32Width * s32CharIdx)) * OSD_LIB_FONT_W /
                                 stFontSize.u32Width;//0-23
            CVI_S32 s32CharRow = u32BmRow * OSD_LIB_FONT_H / stFontSize.u32Height;//u32BmRow 0-24
            CVI_S32 s32HexOffset = s32CharRow * OSD_LIB_FONT_W / BYTE_BITS + s32CharCol / BYTE_BITS;
            CVI_S32 s32BitOffset = s32CharCol % BYTE_BITS;
            CVI_U8 *FontMod = NULL;
            CVI_S32 FontModLen = 0;

            if (APP_OSD_GetFontMod(&szStr[s32StringIdx], &FontMod, &FontModLen) == CVI_SUCCESS) {
                if (FontMod != NULL && s32HexOffset < FontModLen) {
                    CVI_U8 temp = FontMod[s32HexOffset];
                    u32Color = *(pu32Color + s32CharIdx);
                    if ((temp >> ((BYTE_BITS - 1) - s32BitOffset)) & 0x1)
                        puBmData[s32BmDataIdx] = (CVI_U16)u32Color;
                    else
                        puBmData[s32BmDataIdx] = (CVI_U16)u32BgColor;
                    continue;
                }
            }
            printf("GetFontMod Fail\n");
            return CVI_FAILURE;
        }
    }
    s32Ret = CVI_RGN_SetBitMap(RgnHdl, pstBitmap);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_SetBitMap failed with %#x!\n", s32Ret);
        return CVI_FAILURE;
    }

    return s32Ret;
}

CVI_VOID *APP_OSDTimeStamp_Task(CVI_VOID *args)
{
    APP_OSD_S *pstAppOsd = (APP_OSD_S *)args;
    CVI_CHAR szStr[OSD_LEN_MAXSIZE] = {0};
    BITMAP_S stBitmap = {0};
    CVI_S32 s32StrLen = 0;
    CVI_U32 *pu32Color = NULL;
    RGN_CHN_ATTR_S stChnAttr;
    CVI_S32 ret;

    prctl(PR_SET_NAME, "App_OsdTimeStamp_Task");
    APP_OSD_GetTimeStr(NULL, szStr, OSD_LEN_MAXSIZE);

    s32StrLen = strlen(szStr);
    pu32Color = (CVI_U32 *)malloc(s32StrLen * sizeof(CVI_U32));
    if (pu32Color == NULL) {
        printf("%s pu32Color malloc err!\n", __func__);
        goto TASK_EXIT;
    }
    stBitmap.u32Width = OSD_LIB_FONT_W * (strlen(szStr) - MediaOsd_GetNonASCNum(szStr, strlen(szStr)));
    stBitmap.u32Height = OSD_LIB_FONT_H;
    stBitmap.pData = malloc(2 * stBitmap.u32Width * stBitmap.u32Height);
    if (stBitmap.pData == NULL) {
        printf("malloc osd memroy err!\n");
        goto TASK_EXIT;
    }
    while (pstAppOsd ->s32Enable) {
        CVI_RGN_GetDisplayAttr(pstAppOsd->s32Handle, &pstAppOsd->stChn, &stChnAttr);
        if (stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn)
            CVI_RGN_Invert_Color(pstAppOsd->s32Handle, &pstAppOsd->stChn, pu32Color);
        else
            memset(pu32Color, 0xffff, sizeof(CVI_U32) * s32StrLen);
        APP_OSD_GetTimeStr(NULL, szStr, OSD_LEN_MAXSIZE);
        stBitmap.u32Width = pstAppOsd->Pos.u32Width;
        stBitmap.u32Height = pstAppOsd->Pos.u32Height;
        stBitmap.enPixelFormat = pstAppOsd->PixelFormat;
        ret = APP_OSD_UpdateBitmap(pstAppOsd->s32Handle, szStr, &stBitmap, pu32Color, pstAppOsd->Rotation);
        if (ret != CVI_SUCCESS) {
            printf("APP_OSD_UpdateBitmap failed!\n");
            goto TASK_EXIT;
        }
        sleep(1);
    }
    return 0;
TASK_EXIT:
    if (pu32Color)
        free(pu32Color);
    if (stBitmap.pData)
        free(stBitmap.pData);
    return 0;
}

CVI_S32 APP_OSD_Destory(APP_OSD_S *pstAppOsd)
{
    if (!pstAppOsd) {
        printf("%s pstAppOsd is NULL\n", __func__);
        return CVI_FAILURE;
    }
    CVI_S32 ret;
    RGN_HANDLE handle = pstAppOsd->s32Handle;

    pstAppOsd->s32Enable = 0;
    if (pstAppOsd->s32Handle == APP_MEDIAOSD_TIMESTAMP)
        pthread_join(pstAppOsd->PthreadId, NULL);
    CVI_RGN_DetachFromChn(handle, &pstAppOsd->stChn);
    ret = CVI_RGN_Destroy(handle);
    if (ret != CVI_SUCCESS) {
        printf("CVI_RGN_Destroy failed with %#x!\n", ret);
        return CVI_FAILURE;
    }
    return CVI_SUCCESS;
}

static CVI_S32 APP_OSD_GetRoationPos(APP_OSD_S *pstAppOsd, RECT_S *RotationPos)
{
    if (!pstAppOsd || !RotationPos) {
        printf("%s pstAppOsd or Pos err\n", __func__);
        return CVI_FAILURE;
    }
    double proportionx = 0;
    double proportiony = 0;
    double proportionVideo = 0;
    VPSS_CHN_ATTR_S stVpssAttr = {0};
    CVI_CHAR *ptmpStr = NULL;

    if (CVI_VPSS_GetChnAttr(pstAppOsd->stChn.s32DevId, pstAppOsd->stChn.s32ChnId, &stVpssAttr) != CVI_SUCCESS) {
        printf("%s CVI_VPSS_GETCHNATTR Err\n", __func__);
        return CVI_FAILURE;
    }
    CVI_VPSS_GetChnRotation(pstAppOsd->stChn.s32DevId, pstAppOsd->stChn.s32ChnId, &pstAppOsd->Rotation);
    proportionVideo = (double)stVpssAttr.u32Width/(double)stVpssAttr.u32Height;
    if (pstAppOsd->s32Type == APP_OSDTYPE_CHAR) {
        CVI_CHAR szStr[OSD_LEN_MAXSIZE] = {0};
        if (pstAppOsd->s32Handle == APP_MEDIAOSD_TIMESTAMP) {
            APP_OSD_GetTimeStr(NULL, szStr, OSD_LEN_MAXSIZE);
            ptmpStr = szStr;
            pstAppOsd->Pos.s32X = stVpssAttr.u32Width/2 - OSD_LIB_FONT_W * strlen(ptmpStr) / 2 ;
            pstAppOsd->Pos.s32Y = stVpssAttr.u32Height - OSD_LIB_FONT_H - 20;
        } else if (pstAppOsd->s32Handle == APP_MEDIAOSD_STRING) {
            ptmpStr = pstAppOsd->String;
        }
        if ( pstAppOsd->Pos.s32X <= 0) {
            pstAppOsd->Pos.s32X = 0;
        } else if (pstAppOsd->Pos.s32Y <= 0) {
            pstAppOsd->Pos.s32Y = 0;
        }
        pstAppOsd->Pos.u32Width = OSD_LIB_FONT_W * strlen(ptmpStr);
        pstAppOsd->Pos.u32Height = OSD_LIB_FONT_H;
        if (pstAppOsd->Rotation == ROTATION_90) {
            pstAppOsd->Pos.u32Width = OSD_LIB_FONT_H;
            pstAppOsd->Pos.u32Height = OSD_LIB_FONT_W * strlen(ptmpStr);
            proportionx = (CVI_DOUBLE)stVpssAttr.u32Width/(CVI_DOUBLE)stVpssAttr.u32Height;
            proportiony = (CVI_DOUBLE)stVpssAttr.u32Height;
            RotationPos->s32X = (CVI_S32)(proportionx * (CVI_DOUBLE)pstAppOsd->Pos.s32Y);
            RotationPos->s32Y =
                (CVI_S32)(proportiony
                * (1.0 - ((CVI_DOUBLE)pstAppOsd->Pos.s32X / proportionVideo) / ((CVI_DOUBLE)stVpssAttr.u32Width))
                - pstAppOsd->Pos.u32Height);
        } else if (pstAppOsd->Rotation == ROTATION_180) {
            RotationPos->s32X = stVpssAttr.u32Width - pstAppOsd->Pos.u32Width - pstAppOsd->Pos.s32X;
            RotationPos->s32Y = stVpssAttr.u32Height - pstAppOsd->Pos.u32Height - pstAppOsd->Pos.s32Y;
        } else if (pstAppOsd->Rotation == ROTATION_270) {
            pstAppOsd->Pos.u32Width = OSD_LIB_FONT_H;
            pstAppOsd->Pos.u32Height = OSD_LIB_FONT_W * strlen(ptmpStr);
            proportionx = (CVI_DOUBLE)stVpssAttr.u32Height / (CVI_DOUBLE)stVpssAttr.u32Width;
            proportiony = (CVI_DOUBLE)stVpssAttr.u32Width;
            RotationPos->s32X =
                proportiony
                * (1.0 - ((CVI_DOUBLE)pstAppOsd->Pos.s32Y / (CVI_DOUBLE)stVpssAttr.u32Height))
                - pstAppOsd->Pos.u32Width;
            RotationPos->s32Y = proportionx * pstAppOsd->Pos.s32X / proportionVideo;
        } else {
            RotationPos->s32X = pstAppOsd->Pos.s32X;
            RotationPos->s32Y = pstAppOsd->Pos.s32Y;
        }
    }
    if (RotationPos->s32X < 0) {
        printf("Osd[%d] Point_x err\n", pstAppOsd->s32Handle);
        return CVI_FAILURE;
    } else if (RotationPos->s32Y < 0) {
        printf("Osd[%d] s32Point_y err\n",pstAppOsd->s32Handle);
        return CVI_FAILURE;
    } else if (RotationPos->s32X + pstAppOsd->Pos.u32Width > stVpssAttr.u32Width) {
        printf("Osd[%d] Point_x err\n", pstAppOsd->s32Handle);
        return CVI_FAILURE;
    } else if (RotationPos->s32Y + pstAppOsd->Pos.u32Height > stVpssAttr.u32Height) {
        printf("Osd[%d] Point_y err\n", pstAppOsd->s32Handle);
        return CVI_FAILURE;
    }
    return CVI_SUCCESS;
}

CVI_S32 APP_OSD_Create(APP_OSD_S *pstAppOsd)
{
    if (!pstAppOsd) {
        printf("%s pstAppOsd null\n", __func__);
        return CVI_FAILURE;
    }
    RECT_S Pos = {0};
    RGN_ATTR_S stRegion ;
    RGN_CHN_ATTR_S stChnAttr;
    CVI_S32 s32Ret = CVI_FAILURE;

    if (APP_OSD_GetRoationPos(pstAppOsd, &Pos) != CVI_SUCCESS) {
        printf("APP_OSD_GetRoationPos err\n");
        return CVI_FAILURE;
    }

    stRegion.enType = OVERLAY_RGN;
    stRegion.unAttr.stOverlay.enPixelFormat = pstAppOsd->PixelFormat;
    stRegion.unAttr.stOverlay.stSize.u32Height = pstAppOsd->Pos.u32Height;
    stRegion.unAttr.stOverlay.stSize.u32Width = pstAppOsd->Pos.u32Width;
    stRegion.unAttr.stOverlay.u32BgColor = 0x7fff;
    stRegion.unAttr.stOverlay.u32CanvasNum = 1;
    s32Ret = CVI_RGN_Create(pstAppOsd->s32Handle, &stRegion);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_Create failed with %#x!\n", s32Ret);
        goto EXIT0;
    }
    stChnAttr.bShow = CVI_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = OSD_LIB_FONT_W;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = OSD_LIB_FONT_H;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = MORETHAN_LUM_THRESH;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = Pos.s32X;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = Pos.s32Y;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = CVI_FALSE;
    s32Ret = CVI_RGN_AttachToChn(pstAppOsd->s32Handle, &pstAppOsd->stChn, &stChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        goto EXIT0;
    }
    pstAppOsd->s32Enable = 1;
    if (pstAppOsd->s32Handle == APP_MEDIAOSD_TIMESTAMP) {
        if (pthread_create(&pstAppOsd->PthreadId, NULL, APP_OSDTimeStamp_Task, pstAppOsd) < 0) {
            printf("%s OsdEnum :%d pthread create err\n", __func__, pstAppOsd->s32Handle);
            pstAppOsd->s32Enable = 0;
            APP_OSD_Destory(pstAppOsd);
            goto EXIT;
        }
    }
    return CVI_SUCCESS;
EXIT0:
    s32Ret = CVI_RGN_Destroy(pstAppOsd->s32Handle);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_Destroy failed with %#x!\n", s32Ret);
    }
EXIT:
    return s32Ret;
}

CVI_S32 APP_OSD_SetString(APP_OSD_S *pstAppOsd, CVI_S32 s32Handle, CVI_CHAR *String)
{
    RGN_CHN_ATTR_S stChnAttr;
    APP_OSD_S *pstTempAppOsd = pstAppOsd;
    CVI_S32 ret = CVI_SUCCESS;
    if (!pstAppOsd) {
        pstTempAppOsd = &s_APP_OSD_Manager[s32Handle];
    }
    if (pstTempAppOsd ->s32OsdDevEnable == 0 || pstTempAppOsd->s32Enable == 0) {
        printf("%s s32OsdDevEnable or s32Enable err\n", __func__);
        return CVI_FAILURE;
    }
    if (strlen(String) > strlen(pstTempAppOsd->String)) {
        printf("%s err input string over create area please create again\n", __func__);
        return CVI_FAILURE;
    }
    CVI_U32 *pu32Color = NULL;
    BITMAP_S stBitmap = {0};

    pu32Color = (CVI_U32 *)malloc(strlen(String) * sizeof(CVI_U32));
    if (pu32Color == NULL) {
        printf("%s pu32Color malloc err!\n", __func__);
        ret = CVI_FAILURE;
        goto EXIT;
    }
    CVI_RGN_GetDisplayAttr(pstTempAppOsd->s32Handle, &pstTempAppOsd->stChn, &stChnAttr);
    if (stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn)
        CVI_RGN_Invert_Color(pstTempAppOsd->s32Handle, &pstTempAppOsd->stChn, pu32Color);
    else
        memset(pu32Color, 0xffff, sizeof(CVI_U32) * strlen(String));
    stBitmap.u32Width = OSD_LIB_FONT_W * (strlen(String) - MediaOsd_GetNonASCNum(String, strlen(String)));
    stBitmap.u32Height = OSD_LIB_FONT_H;
    stBitmap.pData = malloc(2 * stBitmap.u32Width * stBitmap.u32Height);
    if (!stBitmap.pData) {
        printf("%s stBitmap malloc err\n", __func__);
        ret = CVI_FAILURE;
        goto EXIT;
    }
    stBitmap.u32Width = pstTempAppOsd->Pos.u32Width;
    stBitmap.u32Height = pstTempAppOsd->Pos.u32Height;
    stBitmap.enPixelFormat = pstTempAppOsd->PixelFormat;
    ret = APP_OSD_UpdateBitmap(pstTempAppOsd->s32Handle, String, &stBitmap, pu32Color, pstTempAppOsd->Rotation);
    if (ret != CVI_SUCCESS) {
        printf("%s APP_OSD_UpdateBitmap failed!\n", __func__);
        ret = CVI_FAILURE;
        goto EXIT;
    }
EXIT:
    if (pu32Color)
        free(pu32Color);
    if (stBitmap.pData)
        free(stBitmap.pData);
    return ret;
}

static CVI_S32 APP_OSD_MST_UpdateCanvas(const CVI_CHAR *filename, BITMAP_S *pstBitmap, CVI_BOOL bFil, CVI_U32 u16FilColor,
                   SIZE_S *pstSize, CVI_U32 u32Stride, PIXEL_FORMAT_E enPixelFormat)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
        printf("GetBmpInfo err!\n");
        return CVI_FAILURE;
    }

    if (enPixelFormat == PIXEL_FORMAT_ARGB_1555) {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
    } else if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
    } else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
    } else {
        printf("Pixel format is not support!\n");
        return CVI_FAILURE;
    }

    if (pstBitmap->pData == NULL) {
        printf("malloc osd memroy err!\n");
        return CVI_FAILURE;
    }

    CreateSurfaceByCanvas(filename, &Surface, (CVI_U8 *)(pstBitmap->pData)
                , pstSize->u32Width, pstSize->u32Height, u32Stride);

    pstBitmap->u32Width = Surface.u16Width;
    pstBitmap->u32Height = Surface.u16Height;
    pstBitmap->enPixelFormat = enPixelFormat;

    // if pixel value match color, make it transparent.
    // Only works for ARGB1555
    if (bFil) {
        CVI_U32 i, j;
        CVI_U16 *pu16Temp;

        pu16Temp = (CVI_U16 *)pstBitmap->pData;
        for (i = 0; i < pstBitmap->u32Height; i++) {
            for (j = 0; j < pstBitmap->u32Width; j++) {
                if (u16FilColor == *pu16Temp)
                    *pu16Temp &= 0x7FFF;

                pu16Temp++;
            }
        }
    }

    return CVI_SUCCESS;
}

CVI_S32 APP_OSD_SetPicture(APP_OSD_S *pstAppOsd, CVI_S32 s32Handle, CVI_CHAR *PictureFilePath)
{
    APP_OSD_S *pstTempAppOsd = pstAppOsd;
    CVI_S32 s32Ret = CVI_SUCCESS;
    if (!pstAppOsd) {
        pstTempAppOsd = &s_APP_OSD_Manager[s32Handle];
    }
    if (pstTempAppOsd ->s32OsdDevEnable == 0 || pstTempAppOsd->s32Enable == 0) {
        printf("%s s32OsdDevEnable or s32Enable err\n", __func__);
        return CVI_FAILURE;
    }
    if (!pstTempAppOsd->PicFilePath) {
        printf("%s PicFilePath null\n", __func__);
        return CVI_FAILURE;
    }
    if (access(pstTempAppOsd->PicFilePath,F_OK) != 0) {
        printf("%s PicFilePath no exist\n", __func__);
        return CVI_FAILURE;
    }

    SIZE_S stSize;
    BITMAP_S stBitmap;
    RGN_CANVAS_INFO_S stCanvasInfo;

    s32Ret = CVI_RGN_GetCanvasInfo(s32Handle, &stCanvasInfo);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
        return CVI_FAILURE;
    }

    stBitmap.pData = stCanvasInfo.pu8VirtAddr;
    stSize.u32Width = stCanvasInfo.stSize.u32Width;
    stSize.u32Height = stCanvasInfo.stSize.u32Height;
    s32Ret = APP_OSD_MST_UpdateCanvas(pstTempAppOsd->PicFilePath, &stBitmap, CVI_FALSE, 0, &stSize,
        stCanvasInfo.u32Stride, pstTempAppOsd->PixelFormat);
    if (s32Ret != CVI_SUCCESS) {
        printf("APP_OSD_MST_UpdateCanvas failed!\n");
        return CVI_FAILURE;
    }
    s32Ret = CVI_RGN_UpdateCanvas(pstTempAppOsd->s32Handle);
    if (s32Ret != CVI_SUCCESS) {
        printf("CVI_RGN_UpdateCanvas failed!\n");
        return CVI_FAILURE;
    }

    return s32Ret;
}

CVI_S32 APP_OSD_Init(void)
{
    for (CVI_S32 i = 0; i < APP_MEDIAOSD_MAX; i++) {
        if (s_APP_OSD_Manager[i].s32OsdDevEnable == 0) {
            printf("Osd[%d] OsdDevDisEnable\n", i);
            continue;
        }
        if (s_APP_OSD_Manager[i].s32Enable == 1) {
            printf("%s OSD:%d Enable == 1\n", __func__, i);
            continue;
        }
        if (APP_OSD_Create(&s_APP_OSD_Manager[i]) == CVI_FAILURE)
            continue;
        if (i == APP_MEDIAOSD_TIMESTAMP) {
        } else if (i == APP_MEDIAOSD_STRING) {
            APP_OSD_SetString(NULL, i, OSDSTRING);
        } else if (i == APP_MEDIAOSD_PICTURE) {
            APP_OSD_SetPicture(NULL, i, PICTUREFILENAME);
        }
    }
    printf("APP_OSD_Init Success\n");
    return CVI_SUCCESS;
}

CVI_S32 APP_OSD_DeInit(void)
{
    for (CVI_S32 i = 0; i < APP_MEDIAOSD_MAX; i++) {
        if (s_APP_OSD_Manager[i].s32Enable == 1) {
            APP_OSD_Destory(&s_APP_OSD_Manager[i]);
        }
    }
    return CVI_SUCCESS;
}

CVI_S32 APP_OSD_GetAttr(APP_MEDIAOSD_E OsdType, APP_OSD_S *pstAppOsd)
{
    if (!pstAppOsd) {
        printf("%s pstAppOsd null\n", __func__);
        return CVI_FAILURE;
    }
    memcpy(pstAppOsd, &s_APP_OSD_Manager[OsdType], sizeof(APP_OSD_S));
    return CVI_SUCCESS;
}

CVI_S32 APP_OSD_SetAttr(APP_MEDIAOSD_E OsdType, const APP_OSD_S *pstAppOsd)
{
    if (!pstAppOsd) {
        printf("%s pstAppOsd null\n", __func__);
        return CVI_FAILURE;
    }
    memcpy(&s_APP_OSD_Manager[OsdType], pstAppOsd, sizeof(APP_OSD_S));
    return CVI_SUCCESS;
}

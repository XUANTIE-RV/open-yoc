#ifndef __CVI_VICFG_H_
#define __CVI_VICFG_H_
#include "cvi_type.h"
#include "cvi_common.h"
#include "cvi_comm_vi.h"
#include "cvi_comm_sys.h"
#include "cvi_sns_ctrl.h"

//多级别结构 确定使用的多级数组加number作标定 (多级限定场景 串行结构 不允许存在同一级别有多个多级存在) 大结构必须精简尽量使用指针
//Sensor 能力集
typedef struct _PARAM_SNS_CFG_S {
    CVI_S32 s32SnsId;

    SNS_TYPE_E enSnsType;
    CVI_S32 s32WDRMode;
    CVI_S32 s32Framerate;

    CVI_S8 s8I2cDev;
    CVI_S32 s32BusId;
    CVI_S32 s32I2cAddr;
    CVI_BOOL bSetDevAttr;
    CVI_BOOL bSetDevAttrMipi;
    CVI_S32 s32Devno;
    CVI_S16 as16LaneId[5];
    CVI_S8  as8PNSwap[5];
    CVI_S16 s16MacClk;
    CVI_U8 u8MclkFreq;
    CVI_U8 u8MclkCam;
    CVI_S32 u8Orien;
    CVI_BOOL bHwSync;
    CVI_U8 u8UseDualSns;
    CVI_U8 u8DisableRst;

    CVI_U32 u32Rst_port_idx;
    CVI_U32 u32Rst_pin;
    CVI_U32 u32Rst_pol;
    CVI_U8 u8Rotation;
}PARAM_SNS_CFG_S;

typedef struct _PARAM_DEV_CFG_S {
    void * pViDmaBuf;
    CVI_U32 u32ViDmaBufSize;
} PARAM_DEV_CFG_S;

typedef struct _PARAM_CHN_CFG_S {
    CVI_S32 s32ChnId;
    CVI_U32 u32Width;
    CVI_U32 u32Height;
    CVI_FLOAT f32Fps;
    PIXEL_FORMAT_E enPixFormat;
    WDR_MODE_E enWDRMode;
    DYNAMIC_RANGE_E enDynamicRange;
    VIDEO_FORMAT_E enVideoFormat;
    COMPRESS_MODE_E enCompressMode;
} PARAM_CHN_CFG_S;

typedef struct _PARAM_SNAP_INFO_S {
    CVI_BOOL bSnap;
    CVI_BOOL bDoublePipe;
    VI_PIPE VideoPipe;
    VI_PIPE SnapPipe;
    VI_VPSS_MODE_E enVideoPipeMode;
    VI_VPSS_MODE_E enSnapPipeMode;
} PARAM_SNAP_INFO_S;

typedef struct _PARAM_PQ_BIN_DESC_S
{
    CVI_UCHAR * pIspBinData;
    CVI_U32 u32IspBinDataLen;
} PARAM_PQ_BIN_DESC_S;

typedef struct _PARAM_ISP_CFG_S {
    CVI_BOOL bMonoSet[VI_MAX_DEV_NUM];
    CVI_BOOL bUseSingleBin;
    PARAM_PQ_BIN_DESC_S stPQBinDes;
} PARAM_ISP_CFG_S;

typedef struct _PARAM_VI_CFG_S {
    CVI_U32 u32WorkSnsCnt;
    CVI_U32 u32IspSceneNum;
    CVI_BOOL bFastConverge;
    //CVI_U32 u32Depth;
    //SIZE_S stSize;
    PARAM_SNS_CFG_S *pstSensorCfg;
    PARAM_DEV_CFG_S *pstDevInfo;
    //PARAM_CHN_CFG_S astChnInfo[VI_MAX_DEV_NUM];
    //PARAM_SNAP_INFO_S astSnapInfo[VI_MAX_DEV_NUM];
    PARAM_ISP_CFG_S *pstIspCfg;
} PARAM_VI_CFG_S;




#endif

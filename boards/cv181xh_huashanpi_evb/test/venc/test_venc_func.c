#include "board.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <posix/timer.h>
#include <sys/time.h>
#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_vpss.h"
#include "cvi_buffer.h"
#include "cvi_venc.h"
//#include "test_rtsp.h"
#include "sys/prctl.h"
#include "fatfs_vfs.h"
#include "vfs.h"
#include "rtsp_func.h"

#define cvi_printf printf

#define TAG "app"
#define DUMP_BS 0
#define VENC_TEST_MAX_CHN 2
#define APP_CHECK_RET(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
			return;																\
		}																		 \
	} while (0)

#define APP_CHECK_RET2(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
			return actual;														 \
		}																		 \
	} while (0)

#define APP_CHECK_RET3(actual, fmt, arg...)                                   \
	do {																		 \
		if ((actual) != 0) {													 \
			aos_cli_printf("[%d]:%s():ret=%d \n" fmt, __LINE__, __func__, actual, ## arg); \
			return (void *)CVI_SUCCESS;															\
		}																		 \
	} while (0)

typedef enum _VENC_RC_E {
	VENC_RC_CBR = 0,
	VENC_RC_VBR,
	VENC_RC_AVBR,
	VENC_RC_QVBR,
	VENC_RC_FIXQP,
	VENC_RC_QPMAP,
	VENC_RC_UBR,
	VENC_RC_MAX
} VENC_RC_E;


typedef struct _venChnAttr {
	VENC_CHN VeChn;
	PAYLOAD_TYPE_E enType;
	unsigned int width;
	unsigned int height;
	CVI_S32 num_frames;
	CVI_S32 bsMode;
	CVI_U32 u32Profile;
	VENC_RC_E rcMode;
	CVI_S32 iqp;
	CVI_S32 pqp;
	CVI_S32 gop;
	CVI_U32 gopMode;
	CVI_S32 bitrate;
	CVI_S32 minIprop;
	CVI_S32 maxIprop;
	CVI_U32 u32RowQpDelta;
	CVI_S32 firstFrmstartQp;
	CVI_S32 minIqp;
	CVI_S32 maxIqp;
	CVI_S32 minQp;
	CVI_S32 maxQp;
	CVI_S32 framerate;
	CVI_S32 quality;
	CVI_S32 maxbitrate;
	CVI_S32 s32ChangePos;
	CVI_S32 s32MinStillPercent;
	CVI_U32 u32MaxStillQP;
	CVI_U32 u32MotionSensitivity;
	CVI_S32	s32AvbrFrmLostOpen;
	CVI_S32 s32AvbrFrmGap;
	CVI_S32 s32AvbrPureStillThr;
	CVI_S32 statTime;
	CVI_S32 bind_mode;
	CVI_S32 pixel_format;
	CVI_S32 posX;
	CVI_S32 posY;
	CVI_S32 inWidth;
	CVI_S32 inHeight;
	CVI_S32 srcFramerate;
	CVI_U32 bitstreamBufSize;
	CVI_S32 single_LumaBuf;
	CVI_S32 single_core;
	CVI_S32 vpssGrp;
	CVI_S32 vpssChn;
	CVI_S32 viDev;
	CVI_S32 viChn;
	CVI_S32 forceIdr;
	CVI_S32 chgNum;
	CVI_S32 chgBitrate;
	CVI_S32 chgFramerate;
	CVI_S32 tempLayer;
	CVI_S32 testRoi;
	CVI_S32 bgInterval;
	CVI_S32 frameLost;
	CVI_U32 frameLostGap;
	CVI_U32 frameLostBspThr;
	CVI_S32 MCUPerECS;
	CVI_S32 getstream_timeout;
	CVI_S32 sendframe_timeout;
	CVI_S32 s32IPQpDelta;
	CVI_S32 s32BgQpDelta;
	CVI_S32 s32ViQpDelta;
	CVI_S32 bVariFpsEn;
	CVI_S32 initialDelay;
	CVI_U32 u32IntraCost;
	CVI_U32 u32ThrdLv;
	CVI_BOOL bBgEnhanceEn;
	CVI_S32 s32BgDeltaQp;
	CVI_U32 h264EntropyMode;
	CVI_S32 h264ChromaQpOffset;
	CVI_S32 h265CbQpOffset;
	CVI_S32 h265CrQpOffset;
	CVI_U32 enSuperFrmMode;
	CVI_U32 u32SuperIFrmBitsThr;
	CVI_U32 u32SuperPFrmBitsThr;
	CVI_S32 s32MaxReEncodeTimes;

	CVI_U8 aspectRatioInfoPresentFlag;
	CVI_U8 aspectRatioIdc;
	CVI_U8 overscanInfoPresentFlag;
	CVI_U8 overscanAppropriateFlag;
	CVI_U16 sarWidth;
	CVI_U16 sarHeight;

	CVI_U8 timingInfoPresentFlag;
	CVI_U8 fixedFrameRateFlag;
	CVI_U32 numUnitsInTick;
	CVI_U32 timeScale;

	CVI_U8 videoSignalTypePresentFlag;
	CVI_U8 videoFormat;
	CVI_U8 videoFullRangeFlag;
	CVI_U8 colourDescriptionPresentFlag;
	CVI_U8 colourPrimaries;
	CVI_U8 transferCharacteristics;
	CVI_U8 matrixCoefficients;

	CVI_U32 u32FrameQp;
	CVI_BOOL bTestUbrEn;

	CVI_BOOL bEsBufQueueEn;
	CVI_BOOL bStopVenc;
} venChnAttr;

static pthread_t VencTask[VENC_MAX_CHN_NUM];
static venChnAttr attr[VENC_TEST_MAX_CHN] = {
	{
		.VeChn = 0,
		.enType = PT_H265,
		.width = 1280,
		.height = 720,
		.bind_mode = VENC_BIND_VPSS,
		.vpssGrp = 0,
		.vpssChn = 0,
		.bsMode = 0,
		.rcMode = VENC_RC_CBR,
		.iqp = 30,
		.pqp = 30,
		.gop = 25,
		.bitrate = 1024 * 2,
		.firstFrmstartQp = 30,
		.minIqp = CVI_H26X_MINQP_DEFAULT,
		.maxIqp = CVI_H26X_MAXQP_DEFAULT,
		.minQp = CVI_H26X_MINQP_DEFAULT,
		.maxQp = CVI_H26X_MAXQP_DEFAULT,
		.srcFramerate = 25,
		.framerate = 25,
		.timingInfoPresentFlag = 1,
		.bVariFpsEn = 0,
		.maxbitrate = CVI_H26X_FRAME_BITS_DEFAULT,
		.statTime = 2,
		.chgNum = -1,
		.quality = 60,
		.pixel_format = PIXEL_FORMAT_YUV_PLANAR_420,
		.bitstreamBufSize = 1024 * 1024,
		.single_LumaBuf = 0,
		.single_core = 0,
		.forceIdr = 0,
		.tempLayer = 0,
		.testRoi = 0,
		.gopMode = 0,
		.initialDelay = CVI_INITIAL_DELAY_DEFAULT,
		.maxIprop = 100,
		.minIprop = 1,
		.u32ThrdLv = 2,
		.s32ChangePos = DEF_26X_CHANGE_POS,
	},

	{
		.VeChn = 1,
		.enType = PT_H264,
		.width = 1280,
		.height = 720,
		.bind_mode = VENC_BIND_VPSS,
#if (1 == CONFIG_VIDEO_DOORBELL)
		.vpssGrp = 2,
		.vpssChn = 1,
#else
		.vpssGrp = 1,
		.vpssChn = 0,
#endif
		.bsMode = 0,
		.rcMode = VENC_RC_CBR,
		.iqp = 30,
		.pqp = 30,
		.gop = 25,
		.bitrate = 1024 * 2,
		.firstFrmstartQp = 30,
		.minIqp = CVI_H26X_MINQP_DEFAULT,
		.maxIqp = CVI_H26X_MAXQP_DEFAULT,
		.minQp = CVI_H26X_MINQP_DEFAULT,
		.maxQp = CVI_H26X_MAXQP_DEFAULT,
		.srcFramerate = 25,
		.framerate = 25,
		.timingInfoPresentFlag = 1,
		.bVariFpsEn = 0,
		.maxbitrate = CVI_H26X_FRAME_BITS_DEFAULT,
		.statTime = 2,
		.chgNum = -1,
		.quality = 60,
		.pixel_format = PIXEL_FORMAT_YUV_PLANAR_420,
		.bitstreamBufSize = 1024 * 1024,
		.single_LumaBuf = 0,
		.single_core = 0,
		.forceIdr = 0,
		.tempLayer = 0,
		.testRoi = 0,
		.gopMode = 0,
		.initialDelay = CVI_INITIAL_DELAY_DEFAULT,
		.maxIprop = 100,
		.minIprop = 1,
		.u32ThrdLv = 2,
		.s32ChangePos = DEF_26X_CHANGE_POS,
	},
};

static CVI_S32 venc_init(VENC_CHN VeChn, venChnAttr attr)
{
	VENC_CHN_ATTR_S stAttr = {0};
	VENC_PARAM_MOD_S stModParam = {0};
	VENC_RC_PARAM_S stRcParam = {0};
	VENC_REF_PARAM_S stRefParam = {0};
	VENC_CHN_PARAM_S stChnParam = {0};
	VENC_ROI_ATTR_S stRoiAttr = {0};
	VENC_FRAMELOST_S stFrmLostParam = {0};
	VENC_SUPERFRAME_CFG_S stSuperFrmParam = {0};
	VENC_CU_PREDICTION_S stCuPrediction = {0};
	VENC_H265_TRANS_S stH265Trans = {0};
	VENC_H265_VUI_S stH265Vui = {0};
	VENC_H264_TRANS_S stH264Trans = {0};
	VENC_H264_ENTROPY_S stH264EntropyEnc = {0};
	VENC_H264_VUI_S stH264Vui = {0};
	VENC_JPEG_PARAM_S stJpegParam = {0};
	VENC_RECV_PIC_PARAM_S stRecvParam = {0};
	VPSS_CHN_ATTR_S stVpssChnAttr = {0};
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;
	ROTATION_E enRotation;

	if (VENC_BIND_VI != attr.bind_mode) {

		// get width and height from vpss
		APP_CHECK_RET2(CVI_VPSS_GetChnAttr(attr.vpssGrp, attr.vpssChn, &stVpssChnAttr), "CVI_VPSS_GetChnAttr");

		// if VPSS enable rotation, need exchange width and height
		APP_CHECK_RET2(CVI_VPSS_GetChnRotation(attr.vpssGrp, attr.vpssChn, &enRotation), "CVI_VPSS_GetChnRotation");

		if (ROTATION_90 == enRotation || ROTATION_270 == enRotation) {
			attr.width = stVpssChnAttr.u32Height;
			attr.height = stVpssChnAttr.u32Width;
		} else {
			attr.width = stVpssChnAttr.u32Width;
			attr.height = stVpssChnAttr.u32Height;
		}
	}
	printf("venc chn%d enType:%d, VppGrp:%d VpssChn%d, width:%d height:%d \n", attr.VeChn, attr.enType, attr.vpssGrp, attr.vpssChn,attr.width, attr.height);

	stAttr.stVencAttr.enType = attr.enType;

	stAttr.stVencAttr.u32MaxPicWidth = attr.width;
	stAttr.stVencAttr.u32MaxPicHeight = attr.height;

	stAttr.stVencAttr.u32BufSize = attr.bitstreamBufSize;
	stAttr.stVencAttr.u32Profile = attr.u32Profile;
	stAttr.stVencAttr.bByFrame = CVI_TRUE;
	stAttr.stVencAttr.u32PicWidth = attr.width;
	stAttr.stVencAttr.u32PicHeight = attr.height;
	stAttr.stVencAttr.bSingleCore = attr.single_core;
	stAttr.stVencAttr.bEsBufQueueEn = attr.bEsBufQueueEn;


	switch (attr.enType) {
		case PT_H264: {
			if (attr.rcMode == VENC_RC_CBR) {
				stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
				stAttr.stRcAttr.stH264Cbr.bVariFpsEn = attr.bVariFpsEn;
				stAttr.stRcAttr.stH264Cbr.u32SrcFrameRate = attr.srcFramerate;
				stAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = attr.framerate;
				stAttr.stRcAttr.stH264Cbr.u32BitRate = attr.bitrate;
				stAttr.stRcAttr.stH264Cbr.u32Gop = attr.gop;
				stAttr.stRcAttr.stH264Cbr.u32StatTime = attr.statTime;
			} else if (attr.rcMode == VENC_RC_VBR) {
				stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
				stAttr.stRcAttr.stH264Vbr.bVariFpsEn = attr.bVariFpsEn;
				stAttr.stRcAttr.stH264Vbr.u32SrcFrameRate = attr.srcFramerate;
				stAttr.stRcAttr.stH264Vbr.fr32DstFrameRate = attr.framerate;
				stAttr.stRcAttr.stH264Vbr.u32MaxBitRate = attr.bitrate;
				stAttr.stRcAttr.stH264Vbr.u32Gop = attr.gop;
				stAttr.stRcAttr.stH264Vbr.u32StatTime = attr.statTime;
			} else {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			}
			stAttr.stVencAttr.stAttrH264e.bSingleLumaBuf = attr.single_LumaBuf;
			break;
		}

		case PT_H265: {
			if (attr.rcMode == VENC_RC_CBR) {
				stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
				stAttr.stRcAttr.stH265Cbr.bVariFpsEn = attr.bVariFpsEn;
				stAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = attr.srcFramerate;
				stAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = attr.framerate;
				stAttr.stRcAttr.stH265Cbr.u32BitRate = attr.bitrate;
				stAttr.stRcAttr.stH265Cbr.u32Gop = attr.gop;
				stAttr.stRcAttr.stH265Cbr.u32StatTime = attr.statTime;
			} else if (attr.rcMode == VENC_RC_VBR) {
				stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
				stAttr.stRcAttr.stH265Vbr.bVariFpsEn = attr.bVariFpsEn;
				stAttr.stRcAttr.stH265Vbr.u32SrcFrameRate = attr.srcFramerate;
				stAttr.stRcAttr.stH265Vbr.fr32DstFrameRate = attr.framerate;
				stAttr.stRcAttr.stH265Vbr.u32MaxBitRate = attr.bitrate;
				stAttr.stRcAttr.stH265Vbr.u32Gop = attr.gop;
				stAttr.stRcAttr.stH265Vbr.u32StatTime = attr.statTime;
			} else {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			}
			break;
		}

		case PT_MJPEG: {
			if (attr.rcMode == VENC_RC_CBR) {
				stAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
				stAttr.stRcAttr.stMjpegCbr.bVariFpsEn = attr.bVariFpsEn;
				stAttr.stRcAttr.stMjpegCbr.fr32DstFrameRate = attr.framerate;
				stAttr.stRcAttr.stMjpegCbr.u32BitRate = attr.bitrate;
				stAttr.stRcAttr.stMjpegCbr.u32StatTime = attr.statTime;
			} else if (attr.rcMode == VENC_RC_VBR) {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			} else {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			}
			break;
		}

		case PT_JPEG: {
			stAttr.stVencAttr.stAttrJpege.bSupportDCF = CVI_FALSE;
			stAttr.stVencAttr.stAttrJpege.enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
			stAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum = 0;
			break;
		}

		default : {
			printf("chn:%d enType:%d not support \n", attr.VeChn, attr.enType);
			return -1;
		}
	}
	stAttr.stGopAttr.enGopMode = attr.gopMode;
	stAttr.stGopAttr.stNormalP.s32IPQpDelta = attr.s32IPQpDelta;
	APP_CHECK_RET2(CVI_VENC_CreateChn(VeChn, &stAttr), "CVI_VENC_CreateChn");

	APP_CHECK_RET2(CVI_VENC_GetModParam(&stModParam), "CVI_VENC_GetModParam");
	APP_CHECK_RET2(CVI_VENC_SetModParam(&stModParam), "CVI_VENC_SetModParam");

	APP_CHECK_RET2(CVI_VENC_GetRcParam(VeChn, &stRcParam), "CVI_VENC_GetRcParam");
	stRcParam.s32FirstFrameStartQp = attr.firstFrmstartQp;
	stRcParam.s32InitialDelay = attr.initialDelay;
	stRcParam.u32ThrdLv = attr.u32ThrdLv;
	stRcParam.s32BgDeltaQp = attr.s32BgDeltaQp;

	switch (attr.enType) {
		case PT_H264: {
			if (attr.rcMode == VENC_RC_CBR) {
				stRcParam.stParamH264Cbr.bQpMapEn = CVI_FALSE;
				stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = attr.s32MaxReEncodeTimes;
				stRcParam.stParamH264Cbr.u32MaxIprop = attr.maxIprop;
				stRcParam.stParamH264Cbr.u32MinIprop = attr.minIprop;
				stRcParam.stParamH264Cbr.u32MaxIQp = attr.maxIqp;
				stRcParam.stParamH264Cbr.u32MaxQp = attr.maxQp;
				stRcParam.stParamH264Cbr.u32MinIQp = attr.minIqp;
				stRcParam.stParamH264Cbr.u32MinQp = attr.minQp;
			} else if (attr.rcMode == VENC_RC_VBR) {
				stRcParam.stParamH264Vbr.bQpMapEn = CVI_FALSE;
				stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = attr.s32MaxReEncodeTimes;
				stRcParam.stParamH264Vbr.u32MaxIprop = attr.maxIprop;
				stRcParam.stParamH264Vbr.u32MinIprop = attr.minIprop;
				stRcParam.stParamH264Vbr.u32MaxIQp = attr.maxIqp;
				stRcParam.stParamH264Vbr.u32MaxQp = attr.maxQp;
				stRcParam.stParamH264Vbr.u32MinIQp = attr.minIqp;
				stRcParam.stParamH264Vbr.u32MinQp = attr.minQp;
				stRcParam.stParamH264Vbr.s32ChangePos = attr.s32ChangePos;
			} else {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			}
			break;
		}

		case PT_H265: {
			if (attr.rcMode == VENC_RC_CBR) {
				stRcParam.stParamH265Cbr.bQpMapEn = CVI_FALSE;
				stRcParam.stParamH265Cbr.s32MaxReEncodeTimes = attr.s32MaxReEncodeTimes;
				stRcParam.stParamH265Cbr.u32MaxIprop = attr.maxIprop;
				stRcParam.stParamH265Cbr.u32MinIprop = attr.minIprop;
				stRcParam.stParamH265Cbr.u32MaxIQp = attr.maxIqp;
				stRcParam.stParamH265Cbr.u32MaxQp = attr.maxQp;
				stRcParam.stParamH265Cbr.u32MinIQp =  attr.minIqp;
				stRcParam.stParamH265Cbr.u32MinQp = attr.minQp;
			} else if (attr.rcMode == VENC_RC_VBR) {
				stRcParam.stParamH265Vbr.bQpMapEn = CVI_FALSE;
				stRcParam.stParamH265Vbr.s32MaxReEncodeTimes = attr.s32MaxReEncodeTimes;
				stRcParam.stParamH265Vbr.u32MaxIprop = attr.maxIprop;
				stRcParam.stParamH265Vbr.u32MinIprop = attr.minIprop;
				stRcParam.stParamH265Vbr.u32MaxIQp = attr.maxIqp;
				stRcParam.stParamH265Vbr.u32MaxQp = attr.maxQp;
				stRcParam.stParamH265Vbr.u32MinIQp =  attr.minIqp;
				stRcParam.stParamH265Vbr.u32MinQp = attr.minQp;
				stRcParam.stParamH265Vbr.s32ChangePos = attr.s32ChangePos;
			} else {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			}
			break;
		}

		case PT_MJPEG: {
			if (attr.rcMode == VENC_RC_CBR) {
				stRcParam.stParamMjpegCbr.u32MaxQfactor = 99;
				stRcParam.stParamMjpegCbr.u32MinQfactor = 1;
			} else if (attr.rcMode == VENC_RC_VBR) {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			} else {
				printf("chn:%d rcMode:%d not support \n", attr.VeChn, attr.rcMode);
				return -1;
			}
			break;
		}

		case PT_JPEG: {
			// nothing
			break;
		}

		default : {
			printf("chn:%d enType:%d not support \n", attr.VeChn, attr.enType);
			return -1;
		}
	}

	APP_CHECK_RET2(CVI_VENC_SetRcParam(VeChn, &stRcParam), "CVI_VENC_SetRcParam");

	APP_CHECK_RET2(CVI_VENC_GetRefParam(VeChn, &stRefParam), "CVI_VENC_GetRefParam");
	stRefParam.bEnablePred = 0;
	stRefParam.u32Base = 0;
	stRefParam.u32Enhance = 0;
	APP_CHECK_RET2(CVI_VENC_SetRefParam(VeChn, &stRefParam), "CVI_VENC_SetRefParam");

	APP_CHECK_RET2(CVI_VENC_GetChnParam(VeChn, &stChnParam), "CVI_VENC_GetChnParam");
	stChnParam.stFrameRate.s32SrcFrmRate = attr.srcFramerate;
	stChnParam.stFrameRate.s32DstFrmRate = attr.framerate;

	APP_CHECK_RET2(CVI_VENC_SetChnParam(VeChn, &stChnParam), "CVI_VENC_SetChnParam");

	APP_CHECK_RET2(CVI_VENC_GetRoiAttr(VeChn, 0, &stRoiAttr), "CVI_VENC_GetRoiAttr");
	stRoiAttr.bEnable = CVI_FALSE;
	stRoiAttr.bAbsQp = CVI_FALSE;
	stRoiAttr.s32Qp = -2;
	stRoiAttr.u32Index = 0;
	APP_CHECK_RET2(CVI_VENC_SetRoiAttr(VeChn, &stRoiAttr), "CVI_VENC_SetRoiAttr");

	APP_CHECK_RET2(CVI_VENC_GetFrameLostStrategy(VeChn, &stFrmLostParam), "CVI_VENC_GetFrameLostStrategy");
	stFrmLostParam.bFrmLostOpen = attr.frameLost;
	stFrmLostParam.enFrmLostMode = FRMLOST_PSKIP;
	stFrmLostParam.u32EncFrmGaps = attr.frameLostGap;
	stFrmLostParam.u32FrmLostBpsThr = attr.frameLostBspThr;
	APP_CHECK_RET2(CVI_VENC_SetFrameLostStrategy(VeChn, &stFrmLostParam), "CVI_VENC_SetFrameLostStrategy");

	APP_CHECK_RET2(CVI_VENC_GetSuperFrameStrategy(VeChn, &stSuperFrmParam), "CVI_VENC_GetSuperFrameStrategy");
	stSuperFrmParam.enSuperFrmMode = attr.enSuperFrmMode;
	APP_CHECK_RET2(CVI_VENC_SetSuperFrameStrategy(VeChn, &stSuperFrmParam), "CVI_VENC_SetSuperFrameStrategy");

	APP_CHECK_RET2(CVI_VENC_GetCuPrediction(VeChn, &stCuPrediction), "CVI_VENC_GetCuPrediction");
	stCuPrediction.u32IntraCost = attr.u32IntraCost;
	APP_CHECK_RET2(CVI_VENC_SetCuPrediction(VeChn, &stCuPrediction), "CVI_VENC_SetCuPrediction");

	if (attr.enType == PT_H265) {
		APP_CHECK_RET2(CVI_VENC_GetH265Trans(VeChn, &stH265Trans), "CVI_VENC_GetH265Trans");
		stH265Trans.cb_qp_offset = attr.h265CbQpOffset;
		stH265Trans.cr_qp_offset = attr.h265CrQpOffset;
		APP_CHECK_RET2(CVI_VENC_SetH265Trans(VeChn, &stH265Trans), "CVI_VENC_SetH265Trans");

		APP_CHECK_RET2(CVI_VENC_GetH265Vui(VeChn, &stH265Vui), "CVI_VENC_GetH265Vui");
		stH265Vui.stVuiTimeInfo.timing_info_present_flag = attr.timingInfoPresentFlag;
		stH265Vui.stVuiAspectRatio.aspect_ratio_info_present_flag = attr.aspectRatioInfoPresentFlag;
		stH265Vui.stVuiBitstreamRestric.bitstream_restriction_flag = 0;
		stH265Vui.stVuiVideoSignal.video_signal_type_present_flag = attr.videoSignalTypePresentFlag;

		if (stH265Vui.stVuiTimeInfo.timing_info_present_flag) {
			// frame rate = time_scale / num_units_in_tick
			stH265Vui.stVuiTimeInfo.time_scale = attr.framerate;
			stH265Vui.stVuiTimeInfo.num_units_in_tick = 1;
		}

		APP_CHECK_RET2(CVI_VENC_SetH265Vui(VeChn, &stH265Vui), "CVI_VENC_SetH265Vui");
	} else if (attr.enType == PT_H264) {
		APP_CHECK_RET2(CVI_VENC_GetH264Trans(VeChn, &stH264Trans), "CVI_VENC_GetH264Trans");
		stH264Trans.chroma_qp_index_offset = attr.h264ChromaQpOffset;
		APP_CHECK_RET2(CVI_VENC_SetH264Trans(VeChn, &stH264Trans), "CVI_VENC_SetH264Trans");

		APP_CHECK_RET2(CVI_VENC_GetH264Entropy(VeChn, &stH264EntropyEnc), "CVI_VENC_GetH264Entropy");
		stH264EntropyEnc.u32EntropyEncModeI = stH264EntropyEnc.u32EntropyEncModeP = attr.h264EntropyMode;
		APP_CHECK_RET2(CVI_VENC_SetH264Entropy(VeChn, &stH264EntropyEnc), "CVI_VENC_SetH264Entropy");

		APP_CHECK_RET2(CVI_VENC_GetH264Vui(VeChn, &stH264Vui), "CVI_VENC_GetH264Vui");
		stH264Vui.stVuiTimeInfo.timing_info_present_flag = attr.timingInfoPresentFlag;
		stH264Vui.stVuiAspectRatio.aspect_ratio_info_present_flag = attr.aspectRatioInfoPresentFlag;
		stH264Vui.stVuiBitstreamRestric.bitstream_restriction_flag = 0;
		stH264Vui.stVuiVideoSignal.video_signal_type_present_flag = attr.videoSignalTypePresentFlag;

		if (stH264Vui.stVuiTimeInfo.timing_info_present_flag) {
			stH264Vui.stVuiTimeInfo.time_scale = attr.framerate * 2;
			stH264Vui.stVuiTimeInfo.num_units_in_tick = 1;
		}

		APP_CHECK_RET2(CVI_VENC_SetH264Vui(VeChn, &stH264Vui), "CVI_VENC_SetH264Vui");
	} else if (attr.enType == PT_JPEG) {
		APP_CHECK_RET2(CVI_VENC_GetJpegParam(VeChn, &stJpegParam), "CVI_VENC_GetJpegParam");
		stJpegParam.u32Qfactor = attr.quality;
		APP_CHECK_RET2(CVI_VENC_SetJpegParam(VeChn, &stJpegParam), "CVI_VENC_SetJpegParam");
	}

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VeChn;
	if (attr.bind_mode == VENC_BIND_VI) {
		stSrcChn.enModId = CVI_ID_VI;
		stSrcChn.s32DevId = attr.viDev;
		stSrcChn.s32ChnId = attr.viChn;

		APP_CHECK_RET2(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(vi--venc)");
	} else if (attr.bind_mode == VENC_BIND_VPSS) {
		stSrcChn.enModId = CVI_ID_VPSS;
		stSrcChn.s32DevId = attr.vpssGrp;
		stSrcChn.s32ChnId = attr.vpssChn;

		APP_CHECK_RET2(CVI_SYS_Bind(&stSrcChn, &stDestChn), "CVI_SYS_Bind(vpss--venc)");
	}

	stRecvParam.s32RecvPicNum = -1;
	APP_CHECK_RET2(CVI_VENC_StartRecvFrame(VeChn, &stRecvParam), "CVI_VENC_StartRecvFrame");

	return CVI_SUCCESS;
}

static CVI_S32 venc_deinit(VENC_CHN VeChn)
{
	MMF_CHN_S stSrcChn;
	MMF_CHN_S stDestChn;

	stDestChn.enModId = CVI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VeChn;
	if (attr[VeChn].bind_mode == VENC_BIND_VI) {
		stSrcChn.enModId = CVI_ID_VI;
		stSrcChn.s32DevId = attr[VeChn].viDev;
		stSrcChn.s32ChnId = attr[VeChn].viChn;

		APP_CHECK_RET2(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(vi--venc)");
	} else if (attr[VeChn].bind_mode == VENC_BIND_VPSS) {
		stSrcChn.enModId = CVI_ID_VPSS;
		stSrcChn.s32DevId = attr[VeChn].vpssGrp;
		stSrcChn.s32ChnId = attr[VeChn].vpssChn;

		APP_CHECK_RET2(CVI_SYS_UnBind(&stSrcChn, &stDestChn), "CVI_SYS_UnBind(vpss--venc)");
	}

	APP_CHECK_RET2(CVI_VENC_StopRecvFrame(VeChn), "CVI_VENC_StopRecvFrame");

	APP_CHECK_RET2(CVI_VENC_ResetChn(VeChn), "CVI_VENC_ResetChn");

	APP_CHECK_RET2(CVI_VENC_DestroyChn(VeChn), "CVI_VENC_DestroyChn");

	return CVI_SUCCESS;
}

static CVI_VOID* venc_streamproc(CVI_VOID *pvData)
{
	CVI_S32 ret = CVI_SUCCESS;
	VIDEO_FRAME_INFO_S stFrame = {0};
	VENC_STREAM_S stStream = {0};
	VENC_CHN_STATUS_S stStatus = {0};
	venChnAttr *pstAttr = (venChnAttr *)pvData;
	CVI_CHAR cName[32] = {0};

#if DUMP_BS
	int  i = 0;
	int  fd = -1;
	VENC_PACK_S *ppack;

	if (pstAttr->enType == PT_H264)
		snprintf(cName, sizeof(cName), CONFIG_SD_FATFS_MOUNTPOINT"/video%d.h264", pstAttr->VeChn);
	else if (pstAttr->enType == PT_H265)
		snprintf(cName, sizeof(cName), CONFIG_SD_FATFS_MOUNTPOINT"/video%d.h265", pstAttr->VeChn);
	else
		snprintf(cName, sizeof(cName), CONFIG_SD_FATFS_MOUNTPOINT"/video%d.jpg", pstAttr->VeChn);

	fd = aos_open(cName, O_CREAT | O_TRUNC | O_RDWR);
	if (fd < 0)
		cvi_printf("open %s fail \n", cName);
#endif
	snprintf(cName, sizeof(cName), "venc_stmproc%d", pstAttr->VeChn);
	prctl(PR_SET_NAME, cName);

	while (!pstAttr->bStopVenc) {
		if (pstAttr->bind_mode == VENC_BIND_DISABLE) {
			APP_CHECK_RET3(CVI_VPSS_GetChnFrame(pstAttr->vpssGrp, pstAttr->vpssChn, &stFrame, 2000), "CVI_VPSS_GetChnFrame");

			APP_CHECK_RET3(CVI_VENC_SendFrame(pstAttr->VeChn, &stFrame, 2000), "CVI_VENC_SendFrame");
		}

		APP_CHECK_RET3(CVI_VENC_QueryStatus(pstAttr->VeChn, &stStatus), "CVI_VENC_QueryStatus");

		stStream.pstPack = malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
		if (stStream.pstPack == NULL) {
			cvi_printf("SAMPLE_VENC_GetStream, malloc memory failed\n");
			return (void *)CVI_SUCCESS;
		}

GET_STREAM:
		ret = CVI_VENC_GetStream(pstAttr->VeChn, &stStream, 2000);
		if (ret == CVI_ERR_VENC_BUSY) {
			goto GET_STREAM;
		} else if (ret != CVI_SUCCESS) {
			cvi_printf("SAMPLE_VENC_GetStream ret=0x%x\n", ret);
			return (void *)CVI_SUCCESS;
		}

		if (pstAttr->bind_mode == VENC_BIND_DISABLE) {
			APP_CHECK_RET3(CVI_VPSS_ReleaseChnFrame(pstAttr->vpssGrp, pstAttr->vpssChn, &stFrame), "CVI_VPSS_ReleaseChnFrame");
		}
#if DUMP_BS
		for (i = 0; i < stStream.u32PackCount; i++) {
			ppack = &stStream.pstPack[i];
			aos_write(fd, ppack->pu8Addr + ppack->u32Offset, ppack->u32Len - ppack->u32Offset);

		}
#endif
#if (CONFIG_APP_RTSP_SUPPORT == 1)
		SendToRtsp(pstAttr->VeChn, &stStream);
#endif
		APP_CHECK_RET3(CVI_VENC_ReleaseStream(pstAttr->VeChn, &stStream), "CVI_VENC_ReleaseStream");

		if (stStream.pstPack != NULL) {
			free(stStream.pstPack);
			stStream.pstPack = NULL;
		}
	}
#if DUMP_BS
	aos_sync(fd);
	aos_close(fd);
#endif

	return (void *)CVI_SUCCESS;
}

void _start_venc(void)
{
	int ret = 0;
	int VeChn = 0;
	struct sched_param param;
	pthread_attr_t pthread_attr;

	VPSS_MODE_S stVPSSMode = {0};
	CVI_SYS_GetVPSSModeEx(&stVPSSMode);

	for (VeChn = 0; VeChn < CONFIG_VENC_TEST_CHN; VeChn++) {

		attr[VeChn].VeChn = VeChn;
		attr[VeChn].bStopVenc = 0;

		if (VPSS_MODE_SINGLE == stVPSSMode.enMode) {
			attr[VeChn].vpssGrp = 0;
			attr[VeChn].vpssChn = attr[VeChn].VeChn;
		}

		APP_CHECK_RET(venc_init(VeChn, attr[VeChn]), "venc_init\n");
		param.sched_priority = 40;
		pthread_attr_init(&pthread_attr);
		pthread_attr_setschedpolicy(&pthread_attr, SCHED_RR);
		pthread_attr_setschedparam(&pthread_attr, &param);
		pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setstacksize(&pthread_attr, 8192);
		ret = pthread_create(&VencTask[VeChn], &pthread_attr, venc_streamproc, (CVI_VOID *)&attr[VeChn]);
		if (ret != 0) {
			cvi_printf("[Chn %d]pthread_create failed, ret %d\n", VeChn, ret);
			return ;
		}
	}
	return ;
}

void _stop_venc(void)
{
	int VeChn = 0;

	for (VeChn = 0; VeChn < CONFIG_VENC_TEST_CHN; VeChn++) {
		attr[VeChn].bStopVenc = 1;
		if (VencTask[VeChn])
			pthread_join(VencTask[VeChn], NULL);

		APP_CHECK_RET(venc_deinit(VeChn), "stop_venc ret=0x%x\n");
		cvi_printf("venc stop chn:%d\n", VeChn);
	}
	return ;
}

void start_venc(int32_t argc, char **argv)
{
	_start_venc();
}

void stop_venc(int32_t argc, char **argv)
{
	_stop_venc();
}

ALIOS_CLI_CMD_REGISTER(start_venc, start_venc, start venc);
ALIOS_CLI_CMD_REGISTER(stop_venc, stop_venc, stop venc);

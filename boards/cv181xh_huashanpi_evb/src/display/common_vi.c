#include "common_vi.h"

#define MAX_SENSOR_NUM	2
#define SNS0_TYPE	CONFIG_SNS0_TYPE
#define SNS1_TYPE	CONFIG_SNS1_TYPE

typedef struct _SNS_CONFIG_S {
	SIZE_S			stSize;
	WDR_MODE_E		enWdrMode;
	BAYER_FORMAT_E		enBayerFormat;
	PIXEL_FORMAT_E		enPixelFormat;
	SNS_TYPE_E		enSnsType;
} SNS_CONFIG_S;

ISP_CMOS_SENSOR_IMAGE_MODE_S snsr_image_mode = {
	.u16Width = 1280,
	.u16Height = 720,
	.f32Fps = 30,
	.u8SnsMode = WDR_MODE_NONE,
};

VI_DEV_ATTR_S vi_dev_attr_base = {
	.enIntfMode = VI_MODE_MIPI,
	.enWorkMode = VI_WORK_MODE_1Multiplex,
	.enScanMode = VI_SCAN_PROGRESSIVE,
	.as32AdChnId = {-1, -1, -1, -1},
	.enDataSeq = VI_DATA_SEQ_YUYV,
	.stSynCfg = {
	/*port_vsync	port_vsync_neg	  port_hsync			  port_hsync_neg*/
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	/*port_vsync_valid	   port_vsync_valid_neg*/
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb  hsync_act	hsync_hhb*/
	{0, 		  1280, 	  0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0, 		  720, 		  0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0, 		   0,		  0}
	},
	.enInputDataType = VI_DATA_TYPE_RGB,
	.stSize = {1280, 720},
	.stWDRAttr = {WDR_MODE_NONE, 720},
	.enBayerFormat = BAYER_FORMAT_BG,
};

VI_PIPE_ATTR_S vi_pipe_attr_base = {
	.enPipeBypassMode = VI_PIPE_BYPASS_NONE,
	.bYuvSkip = CVI_FALSE,
	.bIspBypass = CVI_FALSE,
	.u32MaxW = 1280,
	.u32MaxH = 720,
	.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP,
	.enCompressMode = COMPRESS_MODE_TILE,
	.enBitWidth = DATA_BITWIDTH_12,
	.bNrEn = CVI_FALSE,
	.bSharpenEn = CVI_FALSE,
	.stFrameRate = {-1, -1},
	.bDiscardProPic = CVI_FALSE,
	.bYuvBypassPath = CVI_FALSE,
};


VI_CHN_ATTR_S vi_chn_attr_base = {
	.stSize = {1280, 720},
	.enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420,
	.enDynamicRange = DYNAMIC_RANGE_SDR8,
	.enVideoFormat = VIDEO_FORMAT_LINEAR,
	.enCompressMode = COMPRESS_MODE_TILE,
	.bMirror = CVI_FALSE,
	.bFlip = CVI_FALSE,
	.u32Depth = 0,
	.stFrameRate = {-1, -1},
};

CVI_S32 get_sensor_type(CVI_S32 dev_id)
{
	if (dev_id == 0) {
		return SNS0_TYPE;
	} else {
		return SNS1_TYPE;
	}
}


CVI_S32 getSnsType(CVI_S32 *snsr_type, CVI_U8 *devNum)
{
	if ((SNS1_TYPE < SNS_TYPE_WDR_BUTT) && (SNS1_TYPE != SNS_TYPE_NONE)) {
		*devNum = 2;
	} else {
		*devNum = 1;
	}

	for (CVI_U8 i = 0; i < *devNum; i++) {
		snsr_type[i] = get_sensor_type(i);
	}

	return CVI_SUCCESS;
}

ISP_SNS_OBJ_S *getSnsObj(SNS_TYPE_E enSnsType)
{
	switch (enSnsType) {
#if CONFIG_SENSOR_GCORE_GC02M1
	case GCORE_GC02M1_MIPI_2M_30FPS_10BIT:
		return &stSnsGc02m1_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC02M1_SLAVE
	case GCORE_GC02M1_SLAVE_MIPI_2M_30FPS_10BIT:
		return &stSnsGc02m1_Slave_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC1054
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		return &stSnsGc1054_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC2053
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
		return &stSnsGc2053_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC2053_1L
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
		return &stSnsGc2053_1l_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC2093
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsGc2093_Obj;
#endif
#if CONFIG_SENSOR_GCORE_GC4653
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		return &stSnsGc4653_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX307
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX307_2L
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_2l_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX307_SLAVE
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_Slave_Obj;
#endif
#if CONFIG_SENSOR_SONY_IMX327
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx327_Obj;
#endif
#if CONFIG_SENSOR_BYD_BF314A
	case BYD_BF314A_MIPI_720P_30FPS_10BIT:
		return &stSnsBf314a_Obj;
#endif
	default:
		return CVI_NULL;
	}
}

CVI_S32 getSnsMode(CVI_S32 dev_id, ISP_CMOS_SENSOR_IMAGE_MODE_S *snsr_mode)
{
	SNS_SIZE_S sns_size;
	CVI_S32 sensor_type;

	if (dev_id >= MAX_SENSOR_NUM) {
		return CVI_FAILURE;
	}

	sensor_type = get_sensor_type(dev_id);

	memcpy(snsr_mode, &snsr_image_mode, sizeof(ISP_CMOS_SENSOR_IMAGE_MODE_S));

	getPicSize(dev_id, &sns_size);

	snsr_mode->u16Height = sns_size.u32Height;
	snsr_mode->u16Width = sns_size.u32Width;

	if (sensor_type >= SNS_TYPE_LINEAR_BUTT)
		snsr_mode->u8SnsMode = WDR_MODE_2To1_LINE;

	return CVI_SUCCESS;
}

CVI_S32 getPicSize(CVI_S32 dev_id, SNS_SIZE_S *pstSize)
{
	CVI_S32 sensor_type;

	if (dev_id >= MAX_SENSOR_NUM) {
		return CVI_FAILURE;
	}

	sensor_type = get_sensor_type(dev_id);

	switch (sensor_type) {
	case GCORE_GC02M1_MIPI_2M_30FPS_10BIT:
	case GCORE_GC02M1_SLAVE_MIPI_2M_30FPS_10BIT:
		pstSize->u32Width  = 1600;
		pstSize->u32Height = 1200;
		break;
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		pstSize->u32Width  = 1280;
		pstSize->u32Height = 720;
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		pstSize->u32Width  = 2560;
		pstSize->u32Height = 1440;
		break;

	default:
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1080;
		break;
	}

	return CVI_SUCCESS;
}

CVI_S32 getDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstViDevAttr)
{
	CVI_S32 sensor_type;
	SNS_SIZE_S sns_size;

	if (ViDev >= MAX_SENSOR_NUM) {
		return CVI_FAILURE;
	}

	sensor_type = get_sensor_type(ViDev);
	getPicSize(ViDev, &sns_size);

	memcpy(pstViDevAttr, &vi_dev_attr_base, sizeof(VI_DEV_ATTR_S));

	pstViDevAttr->stSize.u32Width = sns_size.u32Width;
	pstViDevAttr->stSize.u32Height = sns_size.u32Height;
	pstViDevAttr->stWDRAttr.u32CacheLine = sns_size.u32Height;

	switch (sensor_type) {
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	// GalaxyCore
	case GCORE_GC02M1_MIPI_2M_30FPS_10BIT:
	case GCORE_GC02M1_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pstViDevAttr->enBayerFormat = BAYER_FORMAT_RG;
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		pstViDevAttr->enBayerFormat = BAYER_FORMAT_GR;
		break;
	default:
		pstViDevAttr->enBayerFormat = BAYER_FORMAT_BG;
		break;
	};

	if (sensor_type >= SNS_TYPE_LINEAR_BUTT)
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;

	return CVI_SUCCESS;
}

CVI_S32 getPipeAttr(VI_DEV ViDev, VI_PIPE_ATTR_S *pstViPipeAttr)
{
	SNS_SIZE_S sns_size;

	if (ViDev >= MAX_SENSOR_NUM) {
		return CVI_FAILURE;
	}

	getPicSize(ViDev, &sns_size);

	memcpy(pstViPipeAttr, &vi_pipe_attr_base, sizeof(VI_PIPE_ATTR_S));

	pstViPipeAttr->u32MaxW = sns_size.u32Width;
	pstViPipeAttr->u32MaxH = sns_size.u32Height;

	return CVI_SUCCESS;
}

CVI_S32 getChnAttr(VI_DEV ViDev, VI_CHN_ATTR_S *pstViChnAttr)
{
	SNS_SIZE_S sns_size;

	if (ViDev >= MAX_SENSOR_NUM) {
		return CVI_FAILURE;
	}

	getPicSize(ViDev, &sns_size);

	memcpy(pstViChnAttr, &vi_chn_attr_base, sizeof(VI_CHN_ATTR_S));

	pstViChnAttr->stSize.u32Width = sns_size.u32Width;
	pstViChnAttr->stSize.u32Height = sns_size.u32Height;
	pstViChnAttr->enPixelFormat = PIXEL_FORMAT_NV21;

	return CVI_SUCCESS;
}

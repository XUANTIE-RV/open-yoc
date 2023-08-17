/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <tmedia_core/entity/format/format_factory.h>
#include <tmedia_backend_cvitek/cvi_vi/vi_cvi.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include <posix/timer.h>
#include "cvi_bin.h"
#include "cvi_sys.h"
#include "cvi_sns_ctrl.h"

__attribute__((weak)) vi_cvi_fastconverge_param g_vi_cvi_converge_params;

TMViCvi::TMViCvi()
{
    MMF_CHN_S Chn;

    TMSrcPadCvi *SrcPad = new TMSrcPadCvi(this, NULL);
    Chn.enModId = CVI_ID_VI;
    Chn.s32DevId = -1;
    Chn.s32ChnId = -1;

    SrcPad->SetConfig(Chn);
	mCviSrcPad.insert(std::make_pair(0, SrcPad));
	mCameraMode = TMCAMERA_MODE_NONE;
}

TMViCvi::~TMViCvi()
{
	std::map <int, TMSrcPadCvi*> ::iterator iter;
	for(iter = mCviSrcPad.begin(); iter != mCviSrcPad.end(); iter++) {
		delete iter->second;
	}
	mCviSrcPad.clear();
}

TMSrcPad  *TMViCvi::GetSrcPad(int padID)  
{
	std::map <int, TMSrcPadCvi*> ::iterator iter;
	if(padID == -1) {
		for(int i=0; ;i++) {
			iter = mCviSrcPad.find(i);
			if(iter == mCviSrcPad.end()) {
				padID = i;
				break;
			}
		}
		goto new_pad;
	} 
	iter = mCviSrcPad.find(padID);
	if(iter == mCviSrcPad.end()) {
		goto new_pad;
	}
	return iter->second;
new_pad:
	TMSrcPadCvi *SrcPad = new TMSrcPadCvi(this, NULL);
	iter = mCviSrcPad.begin();
	MMF_CHN_S Chn;
	Chn = iter->second->GetConfig();
	SrcPad->SetConfig(Chn);
	mCviSrcPad.insert(std::make_pair(padID, SrcPad));
	TMEDIA_PRINTF("VI: create new src pad:%d\n", padID);
	return SrcPad;
}

extern unsigned char isp_ir_default_param[];
extern unsigned int isp_ir_default_param_length;
extern unsigned char isp_rgb_color_mode_param[];
extern unsigned int isp_rgb_color_len;
extern unsigned char isp_rgb_mono_mode_param[];
extern unsigned int isp_rgb_mono_len;
void * g_ViDmaBuf = NULL;

extern unsigned char *cvi_isp_param;
extern unsigned int  cvi_isp_param_len;

__attribute__((weak)) int sensor_init_pre_hook(string sensor_name)
{
	return 0;
}

int TMViCvi::Open(string deviceName, TMPropertyList *propList) 
{
	TMEDIA_PRINTF("VI: %s Open\n", deviceName.c_str());
	unsigned int width, height;
	int rgbMode;

    if(propList->Get(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, &width) != 0) {
		mDefaultChannelPropertyList[0].Get(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, &width);
	} 
	if(propList->Get(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, &height) != 0) {
		mDefaultChannelPropertyList[0].Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, height);
	}
	if(propList->Get(TMCamera::PropID::CAMERA_WORK_MODE, &rgbMode) != 0) {
		rgbMode = TMCAMERA_COLOR_MODE;
	}
	mCameraMode = (TMCameraWorkMode_e)rgbMode;

	int devID;
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
	cameraName = deviceName;
	if(deviceName == "rgb0" || deviceName == "ir0") {
		devID = 0;
	} else if(deviceName == "ir1") {
		devID = 1;
	}
#else
	size_t pos;
	cameraName = deviceName;
	if ((pos = cameraName.find("ir")) != std::string::npos)  {
		devID = stoi(cameraName.substr(pos + 2));
	} else if ((pos = cameraName.find("rgb")) != std::string::npos) {
		devID = stoi(cameraName.substr(pos + 3));
	} else {
		TMEDIA_PRINTF("TMViCvi open param error devicenam=%s\n", cameraName.c_str());
		return -1;
	}
#endif

	TMEDIA_PRINTF("VI: open dev:%d\n", devID);
    this->mDeviceID = devID;

	VI_DEV_ATTR_S stViDevAttr = {
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
		.stSize = {width, height},
		.stWDRAttr = {WDR_MODE_NONE, height},
		.enBayerFormat = BAYER_FORMAT_RG,
	};
	sensor_init_pre_hook(cameraName);

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
	unsigned int g_ViDmaBufSize = 13 * 1024 * 1024;
	if(deviceName == "rgb0" || deviceName == "ir0") {
		if(!g_ViDmaBuf) {
			g_ViDmaBuf = malloc(g_ViDmaBufSize);
			if(!g_ViDmaBuf) {
				TMEDIA_PRINTF("VI: malloc fail!\n");
				return TMResult::TM_STATE_ERROR;
			}
		}
		stViDevAttr.phy_addr = (CVI_U64)g_ViDmaBuf;
		stViDevAttr.phy_size = g_ViDmaBufSize;
	}
#endif

	stViDevAttr.stSize.u32Width = width;
	stViDevAttr.stSize.u32Height = height;
	stViDevAttr.stWDRAttr.u32CacheLine = height;

	if(CVI_VI_SetDevAttr(this->mDeviceID, &stViDevAttr) != 0) {
		TMEDIA_PRINTF("CVI_VI_SetDevAttr fail\n");
		return TMResult::TM_STATE_ERROR; 
	}
	if(CVI_VI_EnableDev(this->mDeviceID) != 0) {
		TMEDIA_PRINTF("CVI_VI_EnableDev fail\n");
		return TMResult::TM_STATE_ERROR; 
	}

	VI_PIPE_ATTR_S stPipeAttr = {
		.enPipeBypassMode = VI_PIPE_BYPASS_NONE,
		.bYuvSkip = CVI_FALSE,
		.bIspBypass = CVI_FALSE,
		.u32MaxW = width,
		.u32MaxH = height,
		.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP,
		.enCompressMode = COMPRESS_MODE_TILE,
		.enBitWidth = DATA_BITWIDTH_12,
		.bNrEn = CVI_FALSE,
		.bSharpenEn = CVI_FALSE,
		.stFrameRate = {-1, -1},
		.bDiscardProPic = CVI_FALSE,
		.bYuvBypassPath = CVI_FALSE,
	};

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VI_CreatePipe(this->mDeviceID, &stPipeAttr));
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VI_StartPipe(this->mDeviceID));
	TMEDIA_PRINTF("CVI_VI_StartPipe:%d success\n", this->mDeviceID);

	if (cameraName == "ir0") {
		CVI_S16 firstFrLuma[5] = {60, 66, 77, 173, 724};
		CVI_S16 targetBv[5] = {30, 89, 194, 479, 721};
		ISP_AE_BOOT_FAST_CONVERGE_S stConvergeAttr;

		stConvergeAttr.bEnable = CVI_TRUE;
		stConvergeAttr.availableNode = 5;
		memcpy(stConvergeAttr.firstFrLuma, firstFrLuma,sizeof(firstFrLuma));
		memcpy(stConvergeAttr.targetBv, targetBv, sizeof(targetBv));
		CVI_ISP_SetFastConvergeAttr(0, &stConvergeAttr);
		CVI_ISP_SetFastConvergeAttr(1, &stConvergeAttr);
	}
#else
	if (CVI_VI_CreatePipe(this->mDeviceID, &stPipeAttr) != 0) {
		TMEDIA_PRINTF("CVI_VI_CreatePipe fail\n");
		return TMResult::TM_STATE_ERROR; 
	}

	if (CVI_VI_StartPipe(this->mDeviceID) != 0) {
		TMEDIA_PRINTF("CVI_VI_StartPipe fail\n");
		return TMResult::TM_STATE_ERROR; 
	}

	if (!g_vi_cvi_converge_params.params.empty() &&
		g_vi_cvi_converge_params.params.size() > (size_t)mDeviceID) {
		ISP_AE_BOOT_FAST_CONVERGE_S stConvergeAttr;
    	stConvergeAttr.bEnable = CVI_TRUE;
    	stConvergeAttr.availableNode = 5;
		auto &param = g_vi_cvi_converge_params.params[mDeviceID];

		memcpy(stConvergeAttr.firstFrLuma, 	param.luma,sizeof(param.luma));
		memcpy(stConvergeAttr.targetBv, 	param.bv, sizeof(param.bv));

		TMEDIA_PRINTF("vi cvi converge param luma: %d %d %d %d %d\n", 
				stConvergeAttr.firstFrLuma[0],
				stConvergeAttr.firstFrLuma[1],
				stConvergeAttr.firstFrLuma[2],
				stConvergeAttr.firstFrLuma[3],
				stConvergeAttr.firstFrLuma[4]);
		TMEDIA_PRINTF("vi cvi converge param bv: %d %d %d %d %d\n",
				stConvergeAttr.targetBv[0],
				stConvergeAttr.targetBv[1],
				stConvergeAttr.targetBv[2],
				stConvergeAttr.targetBv[3],
				stConvergeAttr.targetBv[4]);

		CVI_ISP_SetFastConvergeAttr(mDeviceID, &stConvergeAttr);
	}
#endif

	//ISP start
	CVI_S32 s32Ret;
	ISP_STATISTICS_CFG_S stsCfg = {0};

	CVI_ISP_GetStatisticsConfig(0, &stsCfg);
	stsCfg.stAECfg.stCrop[0].bEnable = 0;
	stsCfg.stAECfg.stCrop[0].u16X = stsCfg.stAECfg.stCrop[0].u16Y = 0;
	stsCfg.stAECfg.stCrop[0].u16W = width;
	stsCfg.stAECfg.stCrop[0].u16H = height;
	memset(stsCfg.stAECfg.au8Weight, 1, AE_WEIGHT_ZONE_ROW * AE_WEIGHT_ZONE_COLUMN * sizeof(CVI_U8));
	stsCfg.stWBCfg.u16ZoneRow = AWB_ZONE_ORIG_ROW;
	stsCfg.stWBCfg.u16ZoneCol = AWB_ZONE_ORIG_COLUMN;
	stsCfg.stWBCfg.stCrop.bEnable = 0;
	stsCfg.stWBCfg.stCrop.u16X = stsCfg.stWBCfg.stCrop.u16Y = 0;
	stsCfg.stWBCfg.stCrop.u16W = width;
	stsCfg.stWBCfg.stCrop.u16H = height;
	stsCfg.stWBCfg.u16BlackLevel = 0;
	stsCfg.stWBCfg.u16WhiteLevel = 4095;
	stsCfg.stFocusCfg.stConfig.bEnable = 1;
	stsCfg.stFocusCfg.stConfig.u8HFltShift = 1;
	stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[0] = 1;
	stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[1] = 2;
	stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[2] = 3;
	stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[3] = 5;
	stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[4] = 10;
	stsCfg.stFocusCfg.stConfig.stRawCfg.PreGammaEn = 0;
	stsCfg.stFocusCfg.stConfig.stPreFltCfg.PreFltEn = 1;
	stsCfg.stFocusCfg.stConfig.u16Hwnd = 17;
	stsCfg.stFocusCfg.stConfig.u16Vwnd = 15;
	stsCfg.stFocusCfg.stConfig.stCrop.bEnable = 0;
	// AF offset and size has some limitation.
	stsCfg.stFocusCfg.stConfig.stCrop.u16X = AF_XOFFSET_MIN;
	stsCfg.stFocusCfg.stConfig.stCrop.u16Y = AF_YOFFSET_MIN;
	stsCfg.stFocusCfg.stConfig.stCrop.u16W = width - AF_XOFFSET_MIN * 2;
	stsCfg.stFocusCfg.stConfig.stCrop.u16H = height - AF_YOFFSET_MIN * 2;
	//Horizontal HP0
	stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[0] = 0;
	stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[1] = 0;
	stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[2] = 13;
	stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[3] = 24;
	stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[4] = 0;
	//Horizontal HP1
	stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[0] = 1;
	stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[1] = 2;
	stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[2] = 4;
	stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[3] = 8;
	stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[4] = 0;
	//Vertical HP
	stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[0] = 13;
	stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[1] = 24;
	stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[2] = 0;
	stsCfg.unKey.bit1FEAeGloStat = stsCfg.unKey.bit1FEAeLocStat =
	stsCfg.unKey.bit1AwbStat1 = stsCfg.unKey.bit1AwbStat2 = stsCfg.unKey.bit1FEAfStat = 1;
	//LDG
	stsCfg.stFocusCfg.stConfig.u8ThLow = 0;
	stsCfg.stFocusCfg.stConfig.u8ThHigh = 255;
	stsCfg.stFocusCfg.stConfig.u8GainLow = 30;
	stsCfg.stFocusCfg.stConfig.u8GainHigh = 20;
	stsCfg.stFocusCfg.stConfig.u8SlopLow = 8;
	stsCfg.stFocusCfg.stConfig.u8SlopHigh = 15;
	//Register callback & call API
	ALG_LIB_S stAeLib, stAwbLib;
	stAeLib.s32Id = stAwbLib.s32Id = this->mDeviceID;
	ISP_BIND_ATTR_S stBindAttr;
	stBindAttr.stAeLib.s32Id = stBindAttr.stAwbLib.s32Id = this->mDeviceID;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, ALG_LIB_NAME_SIZE_MAX);
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, ALG_LIB_NAME_SIZE_MAX);
	strncpy(stBindAttr.stAeLib.acLibName, CVI_AE_LIB_NAME, ALG_LIB_NAME_SIZE_MAX);
	strncpy(stBindAttr.stAwbLib.acLibName, CVI_AWB_LIB_NAME, ALG_LIB_NAME_SIZE_MAX);
	s32Ret = CVI_AE_Register(this->mDeviceID, &stAeLib);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("AE Algo register failed!, error:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}
	s32Ret = CVI_AWB_Register(this->mDeviceID, &stAwbLib);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("AWB Algo register failed!, error:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}
	s32Ret = CVI_ISP_SetBindAttr(this->mDeviceID, &stBindAttr);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("Bind Algo failed with:%d\n", s32Ret);
	}
	s32Ret = CVI_ISP_MemInit(this->mDeviceID);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("Init Ext memory failed with:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}
	ISP_PUB_ATTR_S stPubAttr = { 0 };
	stPubAttr.stWndRect.u32Width = stPubAttr.stSnsSize.u32Width = width;
	stPubAttr.stWndRect.u32Height = stPubAttr.stSnsSize.u32Height = height;
	stPubAttr.f32FrameRate = 30;
	stPubAttr.enBayer = (ISP_BAYER_FORMAT_E)BAYER_FORMAT_RG;
	stPubAttr.enWDRMode = WDR_MODE_NONE;
	s32Ret = CVI_ISP_SetPubAttr(this->mDeviceID, &stPubAttr);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("SetPubAttr failed with:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}
	s32Ret = CVI_ISP_SetStatisticsConfig(this->mDeviceID, &stsCfg);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("ISP Set Statistic failed with:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}
	s32Ret = CVI_ISP_Init(this->mDeviceID);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("ISP Init failed with:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}
	if(cameraName.find("ir") != std::string::npos) {
		// set ir camera to mono mode
		ISP_MONO_ATTR_S stMonoAttr;
		stMonoAttr.Enable = 1;
		s32Ret = CVI_ISP_SetMonoAttr(this->mDeviceID, &stMonoAttr);
		if (s32Ret != CVI_SUCCESS) {
			TMEDIA_PRINTF("SetMonoAttr pipe failed with:%d\n", s32Ret);
			return s32Ret;
		}
	}

	//Run ISP
	s32Ret = CVI_ISP_Run(this->mDeviceID);
	if (s32Ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("ISP Run failed with:%d\n", s32Ret);
		return TMResult::TM_STATE_ERROR;
	}

	std::map <int, TMSrcPadCvi*> ::iterator iter = mCviSrcPad.begin();
    MMF_CHN_S Chn;
    Chn = iter->second->GetConfig();
    Chn.s32DevId = 0; //unuse
    Chn.s32ChnId = devID;

	for(iter = mCviSrcPad.begin(); iter != mCviSrcPad.end(); iter++) {
		iter->second->SetConfig(Chn);
	}
	TMEDIA_PRINTF("VI: %s Open success\n", deviceName.c_str());
    return TMResult::TM_OK;
}

int TMViCvi::Close()                                                  
{
	TMEDIA_PRINTF("VI: %s Close\n", cameraName.c_str());
    CVI_S32 s32Ret;
    ALG_LIB_S stAeLib, stAwbLib;
    stAeLib.s32Id = stAwbLib.s32Id = this->mDeviceID;
    strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, ALG_LIB_NAME_SIZE_MAX);
    strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, ALG_LIB_NAME_SIZE_MAX);
    //Stop ISP
	if(cameraName == "rgb0") {
		s32Ret = CVI_ISP_Exit(this->mDeviceID);
		if (s32Ret != CVI_SUCCESS) {
			TMEDIA_PRINTF("ISP Exit failed with:%d\n", s32Ret);
			return TMResult::TM_STATE_ERROR;
		}
		stSnsGc02m1_Obj.pfnUnRegisterCallback(this->mDeviceID, &stAeLib, &stAwbLib);

		s32Ret = CVI_AE_UnRegister(this->mDeviceID, &stAeLib);
		if (s32Ret) {
			TMEDIA_PRINTF("AE Algo unRegister failed!, error:%d\n", s32Ret);
			return TMResult::TM_STATE_ERROR;
		}
		s32Ret = CVI_AWB_UnRegister(this->mDeviceID, &stAwbLib);
		if (s32Ret) {
			TMEDIA_PRINTF("AWB Algo unRegister failed!, error:%d\n", s32Ret);
			return TMResult::TM_STATE_ERROR;
		}
		CVI_VI_DisableChn(this->mDeviceID, this->mDeviceID);
		CVI_VI_DestroyPipe(this->mDeviceID);
		CVI_VI_DisableDev(this->mDeviceID);
		CVI_VI_UnRegChnFlipMirrorCallBack(0, this->mDeviceID);  //first param not use, <=4
		CVI_SYS_VI_Close();
		TMEDIA_PRINTF("VI close device:%d\n", this->mDeviceID);
	} else if(cameraName == "ir0") {
		// do nothing
	} else if(cameraName == "ir1") {		
		for(int i=0; i<2; i++) {
			s32Ret = CVI_ISP_Exit(i);
			if (s32Ret != CVI_SUCCESS) {
				TMEDIA_PRINTF("ISP Exit failed with:%d\n", s32Ret);
				return TMResult::TM_STATE_ERROR;
			}
			if(i == 0) {
				stSnsGc02m1_Obj.pfnUnRegisterCallback(i, &stAeLib, &stAwbLib);
			} else if(i == 1) {
				stSnsGc02m1_Slave_Obj.pfnUnRegisterCallback(i, &stAeLib, &stAwbLib);
			}
			s32Ret = CVI_AE_UnRegister(i, &stAeLib);
			if (s32Ret) {
				TMEDIA_PRINTF("AE Algo unRegister failed!, error:%d\n", s32Ret);
				return TMResult::TM_STATE_ERROR;
			}
			s32Ret = CVI_AWB_UnRegister(i, &stAwbLib);
			if (s32Ret) {
				TMEDIA_PRINTF("AWB Algo unRegister failed!, error:%d\n", s32Ret);
				return TMResult::TM_STATE_ERROR;
			}
			CVI_VI_DisableChn(i, i);
			CVI_VI_DestroyPipe(i);
			CVI_VI_DisableDev(i);
			CVI_VI_UnRegChnFlipMirrorCallBack(0, i);  //first param not use, <=4
			TMEDIA_PRINTF("VI close device:%d success\n", i);
		}
		CVI_SYS_VI_Close();
		TMEDIA_PRINTF("VI: %s Close success\n", cameraName.c_str());
	}

    return TMResult::TM_OK;
}

int TMViCvi::SetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMViCvi::GetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMViCvi::Start()                                                  
{
    return TMResult::TM_OK;
}

int TMViCvi::Stop()                                                   
{

    return TMResult::TM_OK;
}

int TMViCvi::RecvFrame(TMVideoFrame &frame, int timeout)              
{
    return TMResult::TM_OK;
}

//TMTMSrcPad::camera interface
int TMViCvi::GetModes(TMCameraModes_s &modes)
{
    return TMResult::TM_OK;
}

int TMViCvi::SetMode(TMCameraModeCfg_s &mode)
{
    return TMResult::TM_OK;
}

int TMViCvi::StartChannel(int chnID)
{
    return TMResult::TM_OK;
}

int TMViCvi::StopChannel(int chnID)
{
    return TMResult::TM_OK;
}

int TMViCvi::OpenChannel(int chnID, TMPropertyList *propList)       
{
	CVI_S32 s32Ret;
	TMEDIA_PRINTF("VI: %s OpenChannel:%d\n", cameraName.c_str(), chnID);
	unsigned int width, height, pixelFormat;
    if(propList->Get(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, &width) != 0) {
		mDefaultChannelPropertyList[this->mDeviceID].Get(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, &width);
	} 
	if(propList->Get(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, &height) != 0) {
		mDefaultChannelPropertyList[this->mDeviceID].Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, height);
	}
	if(propList->Get(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, &pixelFormat) != 0) {
		mDefaultChannelPropertyList[this->mDeviceID].Get(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, &pixelFormat);
	}
	mCurrentChannelPropertyList[this->mDeviceID].Assign(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, width);
	mCurrentChannelPropertyList[this->mDeviceID].Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, height);
	mCurrentChannelPropertyList[this->mDeviceID].Assign(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, pixelFormat);

    VI_CHN_ATTR_S stChnAttr;
    stChnAttr.stSize.u32Width = width; 
    stChnAttr.stSize.u32Height = height; 
	stChnAttr.enPixelFormat = TMUtilCvi::MapPixelFormat((TMImageInfo::PixelFormat)pixelFormat); 
	TMEDIA_PRINTF("VI: Width:%d Height:%d Pixel Format:%d\n", stChnAttr.stSize.u32Width, stChnAttr.stSize.u32Height, stChnAttr.enPixelFormat);
	/* 以下采用默认值 */
	stChnAttr.enDynamicRange = DYNAMIC_RANGE_SDR8; 
	stChnAttr.enVideoFormat = VIDEO_FORMAT_LINEAR;
	stChnAttr.enCompressMode = COMPRESS_MODE_TILE;
	stChnAttr.bMirror = CVI_FALSE; 
	stChnAttr.bFlip = CVI_FALSE; 
	stChnAttr.u32Depth = 0; 
	stChnAttr.stFrameRate.s32SrcFrameRate = -1; 
    stChnAttr.stFrameRate.s32DstFrameRate = -1;

    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VI_SetChnAttr(this->mDeviceID, this->mDeviceID, &stChnAttr));
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
	if(cameraName == "ir1") {  //speial !!!
		CVI_VI_EnableChn(0, 0);
		CVI_VI_EnableChn(1, 1);
	} else if(cameraName == "ir0") {
		 // do nothing
	} else if(cameraName == "rgb0") {  //speial !!!
		CVI_VI_EnableChn(this->mDeviceID, this->mDeviceID);
	} else {
		TMEDIA_PRINTF("unsupport sensor:%s\n", cameraName.c_str());
	}
#else
	if (mDeviceID == 1) {
		CVI_VI_EnableChn(0, 0);
		CVI_VI_EnableChn(1, 1);
	}
#endif

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
	if (cameraName == "ir1") {
		TMEDIA_PRINTF("sizeof isp_ir_default_param is:%d\n", isp_ir_default_param_length);
		CVI_BIN_EnSingleMode();
		s32Ret = CVI_BIN_ImportBinData(isp_ir_default_param, isp_ir_default_param_length);
		if (s32Ret != CVI_SUCCESS) {
			TMEDIA_PRINTF("CVI_BIN_ImportBinData error! value:%d\n", s32Ret);
		}
		TMEDIA_PRINTF("CVI ISP:%d  Run success\n", this->mDeviceID);
	} else if (cameraName == "ir0") {
		TMEDIA_PRINTF("sizeof isp_ir_default_param is:%d\n", isp_ir_default_param_length);
		TMEDIA_PRINTF("CVI ISP:%d  Run success\n", this->mDeviceID);
	}
	else if(cameraName == "rgb0") {
		if (mCameraMode == TMCAMERA_COLOR_MODE) {
			CVI_BIN_ImportBinData(isp_rgb_color_mode_param, isp_rgb_color_len);
			TMEDIA_PRINTF("sizeof isp_rgb_color_mode_param is:%d\n", isp_rgb_color_len);
		} else {
			CVI_BIN_ImportBinData(isp_rgb_mono_mode_param, isp_rgb_mono_len);
			TMEDIA_PRINTF("sizeof isp_rgb_mono_mode_param is:%d\n", isp_rgb_mono_len);
		}
		TMEDIA_PRINTF("CVI ISP:%d  Run success\n", this->mDeviceID);
	}
#else
	if (mDeviceID == 1) {
		s32Ret = CVI_BIN_ImportBinData(cvi_isp_param, cvi_isp_param_len);
		if (s32Ret != CVI_SUCCESS) {
			TMEDIA_PRINTF("CVI_BIN_ImportBinData error! value:%d\n", s32Ret);
		}
	}
#endif

	TMEDIA_PRINTF("VI: %s OpenChannel:%d success\n", cameraName.c_str(), chnID);

    return TMResult::TM_OK;
}

int TMViCvi::CloseChannel(int chnID)                                       
{
    // CVI_S32 s32Ret;
    // s32Ret = CVI_VI_DisableChn(this->mDeviceID, this->mDeviceID);
    // if (s32Ret != CVI_SUCCESS) {
	// 	TMEDIA_PRINTF("CVI_VI_DisableChn TEST-FAIL:%d\n", s32Ret);
    //     return TMResult::TM_STATE_ERROR;
    // }
	// TMEDIA_PRINTF("VI close channel:%d\n", this->mDeviceID);
    return TMResult::TM_OK;
}

int TMViCvi::SetChannelConfig(int chnID, TMPropertyList &propList)         
{
	int rgbMode;
	if(propList.Get(TMCamera::PropID::CAMERA_WORK_MODE, &rgbMode) == 0) {
		if(rgbMode == mCameraMode) {
			TMEDIA_PRINTF("VI: already in:%d mode\n", rgbMode);
			return TMResult::TM_OK;
		}
		if(cameraName == "rgb0") {
			if (rgbMode == TMCAMERA_COLOR_MODE) {
				CVI_BIN_ImportBinData(isp_rgb_color_mode_param, isp_rgb_color_len);
				TMEDIA_PRINTF("sizeof isp_rgb_color_mode_param is:%d\n", isp_rgb_color_len);
			} else {
				CVI_BIN_ImportBinData(isp_rgb_mono_mode_param, isp_rgb_mono_len);
				TMEDIA_PRINTF("sizeof isp_rgb_mono_mode_param is:%d\n", isp_rgb_mono_len);
			}
			mCameraMode = (TMCameraWorkMode_e)rgbMode;
		}
	}
    return TMResult::TM_OK;
}

int TMViCvi::GetChannelConfig(int chnID, TMPropertyList &propList)         
{
    return TMResult::TM_OK;
}

static int cvi_release_frame_func(struct TMNativeFrameCtx *nativeFrameCtx)
{
    if (nativeFrameCtx)
    {
        CviNativeFrameCtx *nativeCtx = (CviNativeFrameCtx *)nativeFrameCtx->priv;
        CVI_VI_ReleaseChnFrame(nativeCtx->deviceID, nativeCtx->channelID, &nativeCtx->pstFrameInfo);
		free(nativeCtx);
    }

    return TMResult::TM_OK;
}

int TMViCvi::RecvChannelFrame(int chnID, TMVideoFrame &frame, int timeout) 
{
	CviNativeFrameCtx *framectx = (CviNativeFrameCtx*)malloc(sizeof(CviNativeFrameCtx));
    if(CVI_VI_GetChnFrame(this->mDeviceID, chnID, &framectx->pstFrameInfo, timeout) == 0) {
        frame.Init();
        framectx->deviceID = this->mDeviceID;
        framectx->channelID = this->mDeviceID;

        TMBuffer *ctxBuf = TMNativeFrameCtx::CreateCtxBuffer();
        TMNativeFrameCtx *ctx  = (TMNativeFrameCtx *)TMBuffer_Data(ctxBuf);
        ctx->internalBuffer = true; //indicate native frame ctx use internal buffer
        ctx->priv = framectx;
        ctx->freeFunc = cvi_release_frame_func; //set free function
        frame.SetNativeFrameCtx(ctxBuf);
        TMBuffer_UnRef(ctxBuf);
        ctxBuf = NULL;
    } else {
        return TMResult::TM_EAGAIN;
    }
    return TMResult::TM_OK;
}

int TMViCvi::ReleaseChannelFrame(int chnID, TMVideoFrame &frame)           
{
    frame.UnRef();
    return TMResult::TM_OK;
}
REGISTER_VIDEO_INPUT_CLASS(TMMediaInfo::DeviceID::CAMERA, TMViCvi)

/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_cvitek/cvi_vo/vo_cvi.h>
#include <tmedia_core/entity/format/format_factory.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include "cvi_vo.h"
#include "cvi_math.h"

TMVideoOutputCvi::TMVideoOutputCvi()
{
    MMF_CHN_S Chn;
    TMSinkPadCvi *SinkPad = new TMSinkPadCvi(this, NULL);
    Chn.enModId = CVI_ID_VO;
    Chn.s32DevId = -1;
    Chn.s32ChnId = -1;

    SinkPad->SetConfig(Chn);
	mCviSinkPad.insert(std::make_pair(0, SinkPad));
	mDeviceID = 0;
	mLayerID = 0;
	mChannelID = 0;

	InitDefaultPropertyList();
	int outX, outY;
	unsigned int outWidth, outHeight, outPixelFormat, outFrameRate;
	mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, &outX);
	mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, &outY);
	mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, &outWidth);
	mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, &outHeight);
	mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, &outPixelFormat);
	mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, &outFrameRate);

    mCurrentPropertyList.Reset();
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, outX);
    mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, outY);
    mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, outWidth);
    mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, outHeight);
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, outPixelFormat);
    mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, outFrameRate);    
}

TMVideoOutputCvi::~TMVideoOutputCvi()
{
	std::map <int, TMSinkPadCvi*> ::iterator iter;
	for(iter = mCviSinkPad.begin(); iter != mCviSinkPad.end(); iter++) {
		delete iter->second;
	}
	mCviSinkPad.clear();
}

TMSinkPad *TMVideoOutputCvi::GetSinkPad(int padID)  
{
	std::map <int, TMSinkPadCvi*> ::iterator iter;
	if(padID == -1) {
		for(int i=0; ;i++) {
			iter = mCviSinkPad.find(i);
			if(iter == mCviSinkPad.end()) {
				padID = i;
				break;
			}
		}
		goto new_pad;
	} 
	iter = mCviSinkPad.find(padID);
	if(iter == mCviSinkPad.end()) {
		goto new_pad;
	}
	return iter->second;
new_pad:
	TMSinkPadCvi *SinkPad = new TMSinkPadCvi(this, NULL);
	iter = mCviSinkPad.begin();
	MMF_CHN_S Chn;
	Chn = iter->second->GetConfig();
	SinkPad->SetConfig(Chn);
	mCviSinkPad.insert(std::make_pair(padID, SinkPad));
	TMEDIA_PRINTF("VO: create new sink pad:%d\n", padID);
	return SinkPad;
}

int TMVideoOutputCvi::Open(string deviceName, TMPropertyList *propList) 
{
	VO_CONFIG_S stVoConfig;
	memset(&stVoConfig, 0, sizeof(stVoConfig));
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;
	memset(&stLayerAttr, 0, sizeof(stLayerAttr));
	
	int outX, outY;
	unsigned int outWidth, outHeight, outFrameRate, outPixelFormat;
	if (propList != NULL)
    {
		int ret = 0;
		ret |= propList->Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, &outX);
		ret |= propList->Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, &outY);
        ret |= propList->Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, &outWidth);
        ret |= propList->Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, &outHeight);
		ret |= propList->Get(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, &outFrameRate);
		if(ret != 0) {
			TMEDIA_PRINTF("VO open err, param err\n");
			return TMResult::TM_EINVAL;
		}
		ret = propList->Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, &outPixelFormat);
		if(ret != 0) {
			mDefaultPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, &outPixelFormat);
		}
		mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, outX);
		mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, outY);
		mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, outWidth);
		mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, outHeight);
		mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, outPixelFormat);
		mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, outFrameRate);  
    } else {
		mCurrentPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, &outX);
		mCurrentPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, &outY);
		mCurrentPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, &outWidth);
		mCurrentPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, &outHeight);
		mCurrentPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, &outPixelFormat);
		mCurrentPropertyList.Get(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, &outFrameRate);
		TMEDIA_PRINTF("VO open param null, use default param\n");
	}
	TMEDIA_PRINTF("VO: X:%d Y:%d  width:%d height:%d format:%d frame_rate:%d\n", outX, outY, outWidth, outHeight, outPixelFormat, outFrameRate);
	RECT_S stDefDispRect  = {outX, outY, outWidth, outHeight};
	SIZE_S stDefImageSize = {outWidth, outHeight};
	
	VO_CHN_ATTR_S stVOChnAttr;
	memset(&stVOChnAttr, 0, sizeof(stVOChnAttr));
	stVoConfig.VoDev	 = mDeviceID;
	stVoConfig.u32DisBufLen  = 3;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stVoPubAttr.u32BgColor = COLOR_10_RGB_BLACK;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = TMUtilCvi::MapPixelFormat((TMImageInfo::PixelFormat)outPixelFormat);
	stLayerAttr.stDispRect = stDefDispRect;
	stLayerAttr.u32DispFrmRt = outFrameRate;
	stLayerAttr.stImageSize = stDefImageSize;
	stLayerAttr.enPixFormat = stVoConfig.enPixFormat;
	stVOChnAttr.stRect = stLayerAttr.stDispRect;

	if(CVI_VO_SetPubAttr(mDeviceID, &stVoConfig.stVoPubAttr) != 0) {
		TMEDIA_PRINTF("CVI_VO_SetPubAttr failed!\n");
	}		
	if(CVI_VO_Enable(mDeviceID) != 0) {
		TMEDIA_PRINTF("CVI_VO_Enable failed!\n");
	}
	if(CVI_VO_SetDisplayBufLen(mLayerID, stVoConfig.u32DisBufLen) != 0) {
		TMEDIA_PRINTF("CVI_VO_SetDisplayBufLen failed!\n");
	}
	if(CVI_VO_SetVideoLayerAttr(mLayerID, &stLayerAttr) != 0) {
		TMEDIA_PRINTF("CVI_VO_SetVideoLayerAttr failed!\n");
	}
	if(CVI_VO_EnableVideoLayer(mLayerID) != 0) {
		TMEDIA_PRINTF("CVI_VO_EnableVideoLayer failed!\n");
	}
	if(CVI_VO_SetChnAttr(mLayerID, mChannelID, &stVOChnAttr) != 0) {
		TMEDIA_PRINTF("CVI_VO_SetChnAttr failed!\n");
	}

	std::map <int, TMSinkPadCvi*> ::iterator iter = mCviSinkPad.begin();
    MMF_CHN_S Chn;
    Chn = iter->second->GetConfig();
    Chn.s32DevId = 0;
    Chn.s32ChnId = 0;

	for(iter = mCviSinkPad.begin(); iter != mCviSinkPad.end(); iter++) {
		iter->second->SetConfig(Chn);
	}

    return TMResult::TM_OK;
}

int TMVideoOutputCvi::Close()                                                  
{
    TMEDIA_PRINTF("Video Output close\n");
	if(CVI_VO_DisableVideoLayer(mLayerID) != 0) {
		TMEDIA_PRINTF("CVI_VO_DisableVideoLayer failed!\n");
	}
	if(CVI_VO_Disable(mDeviceID) != 0) {
		TMEDIA_PRINTF("CVI_VO_Disable failed!\n");
	}
    return TMResult::TM_OK;
}

int TMVideoOutputCvi::SetConfig(TMPropertyList &propList)                      
{
	int ret = 0;
	int outX, outY;
	unsigned int outWidth, outHeight, outFrameRate, outPixelFormat;

	ret |= propList.Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, &outX);
	ret |= propList.Get(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, &outY);
	ret |= propList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, &outWidth);
	ret |= propList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, &outHeight);
	ret |= propList.Get(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, &outPixelFormat);
	ret |= propList.Get(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, &outFrameRate);
	if(ret != 0) {
		TMEDIA_PRINTF("VO SetConfig err, param err\n");
		return TMResult::TM_EINVAL;
	}

	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, outX);
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, outY);
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, outWidth);
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, outHeight);
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, outPixelFormat);
	mCurrentPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, outFrameRate);  

    return TMResult::TM_OK;
}

int TMVideoOutputCvi::GetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMVideoOutputCvi::Start()                                                  
{
	if(CVI_VO_EnableChn(mLayerID, mChannelID) != 0) {
		TMEDIA_PRINTF("CVI_VO_EnableChn failed!\n");
	}
    return TMResult::TM_OK;
}

int TMVideoOutputCvi::Stop()                                                   
{
    TMEDIA_PRINTF("Video Output stop\n");
	if(CVI_VO_DisableChn(mLayerID, mChannelID) != 0) {
		TMEDIA_PRINTF("CVI_VO_DisableChn failed!\n");
	}
    return TMResult::TM_OK;
}

int TMVideoOutputCvi::SendFrame(TMVideoFrame &frame, int timeout)              
{
    return TMResult::TM_OK;
}

REGISTER_VIDEO_OUTPUT_CLASS(TMMediaInfo::DeviceID::SCREEN, TMVideoOutputCvi)
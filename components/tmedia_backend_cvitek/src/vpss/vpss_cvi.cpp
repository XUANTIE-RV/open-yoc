/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/util/util_inc.h>
#include <tmedia_core/entity/vpss/vpss_factory.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include "tmedia_backend_cvitek/cvi_vpss/vpss_cvi.h"

using namespace std;

bool TMVpssCvi::mCviCreatedVpssGrp[VPSS_MAX_GRP_NUM] = {false};
bool TMVpssCvi::mCviStartedVpssGrp[VPSS_MAX_GRP_NUM] = {false};
bool TMVpssCvi::mCviOpenedVpssChn[VPSS_MAX_CHN_NUM] = {false};
bool TMVpssCvi::mCviUsedOnlineVpss[2] = {false};
vector<grp_chn_combo_table_s> TMVpssCvi::mGroupChannelTable;

TMVpssCvi::TMVpssCvi()
{
    mCviVpssGrp = -1;
    mCviVpssChn = -1;

    MMF_CHN_S Chn;

    TMSrcPadCvi *SrcPad = new TMSrcPadCvi(this, NULL);
    TMSinkPadCvi *SinkPad = new TMSinkPadCvi(this, NULL);
    Chn.enModId = CVI_ID_VPSS;
    Chn.s32DevId = -1;
    Chn.s32ChnId = -1;

    SrcPad->SetConfig(Chn);
    SinkPad->SetConfig(Chn);
    mCviSrcPad.insert(std::make_pair(0, SrcPad));
    mCviSinkPad.insert(std::make_pair(0, SinkPad));
    mOnlineMode = false;
    mFilterFrameFlag = false;
    mStartFrame      = 0;
}

TMVpssCvi::~TMVpssCvi()
{
	std::map <int, TMSrcPadCvi*> ::iterator iter_src;
	for(iter_src = mCviSrcPad.begin(); iter_src != mCviSrcPad.end(); iter_src++) {
		delete iter_src->second;
	}
	mCviSrcPad.clear();

    std::map <int, TMSinkPadCvi*> ::iterator iter_sink;
	for(iter_sink = mCviSinkPad.begin(); iter_sink != mCviSinkPad.end(); iter_sink++) {
		delete iter_sink->second;
	}
	mCviSinkPad.clear();
}

TMSrcPad  *TMVpssCvi::GetSrcPad(int padID)
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
	TMEDIA_PRINTF("VPSS: Group:%d Channel:%d create new src pad:%d\n", mCviVpssGrp, mCviVpssChn, padID);
	return SrcPad;
}

TMSinkPad *TMVpssCvi::GetSinkPad(int padID)
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
    TMEDIA_PRINTF("VPSS: Group:%d Channel:%d create new sink pad:%d\n", mCviVpssGrp, mCviVpssChn, padID);
	return SinkPad;
}

int TMVpssCvi::MapVpssGrpParam(VPSS_GRP_ATTR_S &outParam, TMVpssParams &inParam)
{   
    outParam.u32MaxW = inParam.mMaxWidth;
    outParam.u32MaxH = inParam.mMaxHeight;
    outParam.enPixelFormat = TMUtilCvi::MapPixelFormat(inParam.mPixelFormat);
    outParam.stFrameRate.s32SrcFrameRate = -1;
    outParam.stFrameRate.s32DstFrameRate = -1;
    outParam.u8VpssDev = 0;    // TODO: should confirm from vendor

    return TMResult::TM_OK;
}

int TMVpssCvi::MapVpssChnParam(VPSS_CHN_ATTR_S &outParam, TMPropertyList &inParam)
{
    int RotationDegree = 0;
    int whChange = 0;
    if(inParam.Get(TMVpss::PropID::ROTATE_ENABLE, &RotationDegree) == 0) {
        if(RotationDegree == 90 || RotationDegree == 270) {
            whChange = 1;
        }
    }
    int width, height;
    if(inParam.Get(TMVpss::PropID::OUTPUT_WIDTH, &width) == 0)
    {
        if(whChange == 0) {
            outParam.u32Width = width;
        } else {
            outParam.u32Height = width;
        }
        mCurrentPropertyList.Assign(TMVpss::PropID::OUTPUT_WIDTH, width);
    } else {
        TMEDIA_PRINTF("Vpss: cannot config pixel width\n");
        return TMResult::TM_EINVAL;
    }
    if(inParam.Get(TMVpss::PropID::OUTPUT_HEIGHT, &height) == 0)
    {
        if(whChange == 0) {
            outParam.u32Height = height;
        } else {
            outParam.u32Width = height;
        }
        mCurrentPropertyList.Assign(TMVpss::PropID::OUTPUT_HEIGHT, height);
    } else {
        TMEDIA_PRINTF("Vpss: cannot config pixel height\n");
        return TMResult::TM_EINVAL;
    }
    uint32_t PixelFormat;
    if(inParam.Get(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, &PixelFormat) == 0)
    {
        mCurrentPropertyList.Assign(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, PixelFormat);
        outParam.enPixelFormat = TMUtilCvi::MapPixelFormat((TMImageInfo::PixelFormat)PixelFormat);
    } else {
        TMEDIA_PRINTF("Vpss: cannot config pixel format\n");
        return TMResult::TM_EINVAL;
    }
    /* 以下采用默认值 */
    outParam.enVideoFormat 			         = VIDEO_FORMAT_LINEAR;
    outParam.stFrameRate.s32SrcFrameRate     = -1;
    outParam.stFrameRate.s32DstFrameRate     = -1;
    outParam.bMirror					     = CVI_FALSE;
    outParam.bFlip 					         = CVI_FALSE;
    outParam.u32Depth					     = 0;
    outParam.stAspectRatio.enMode		     = ASPECT_RATIO_NONE;
    outParam.stNormalize.bEnable		     = CVI_FALSE;

    return TMResult::TM_OK;
}

int TMVpssCvi::MapVpssCropParam(VPSS_CROP_INFO_S &outParam, TMPropertyList &inParam)
{
    int CropEnablel;
    int CropCoordinate;
    if(inParam.Get(TMVpss::PropID::CROP_ENABLE, &CropEnablel) == 0) {
        if(CropEnablel == true) {
            outParam.bEnable = CVI_TRUE;
            inParam.Get(TMVpss::PropID::CROP_COORDINATE, &CropCoordinate);
            if(CropCoordinate == vpss_prop::CROP_ABS_COOR) {
                outParam.enCropCoordinate = VPSS_CROP_ABS_COOR;
            } else {
                TMEDIA_PRINTF("VPSS: err crop coordinate type\n");
                return TMResult::TM_EINVAL;
            }
            inParam.Get(TMVpss::PropID::CROP_RECT_X, &outParam.stCropRect.s32X);
            inParam.Get(TMVpss::PropID::CROP_RECT_Y, &outParam.stCropRect.s32Y);
            inParam.Get(TMVpss::PropID::CROP_RECT_W, &outParam.stCropRect.u32Width);
            inParam.Get(TMVpss::PropID::CROP_RECT_H, &outParam.stCropRect.u32Height);
        } else {
            return TMResult::TM_EINVAL;
        }
    } else {
        return TMResult::TM_EINVAL;
    }

    return TMResult::TM_OK;
}
int TMVpssCvi::Open(TMVpssParams &vpssParam, TMPropertyList *propList)
{
    // TODO: Add mutex to protect re-entrant
    int ret;
    VPSS_GRP vpssGrpID = -1;   //group index
    VPSS_CHN vpssChnID = -1;
    VPSS_GRP_ATTR_S grpAttr = {0};
    int deviceID = 0;
    int isOnlineMode = 0;
    int startFrame = 0;

	if (MapVpssGrpParam(grpAttr, vpssParam) != TMResult::TM_OK)
	{
	    TMEDIA_PRINTF("MapVpssGrpParam failed\n");
	    return TMResult::TM_MAPVAL_FAIL;
	}

    if (propList != NULL)
    {
        if (propList->Get(VPSS_GROUP_ID, &vpssGrpID) == 0)
        {
            if(vpssGrpID < 0 || vpssGrpID >= VPSS_MAX_GRP_NUM) {
                TMEDIA_PRINTF("Wrong VPSS Group:%d !\n", vpssGrpID);
                return TMResult::TM_EINVAL;
            }
        }
        if (propList->Get(VPSS_CHANNEL_ID, &vpssChnID) == 0)
        {
            if(vpssChnID < 0 || vpssChnID >= VPSS_MAX_CHN_NUM) {
                TMEDIA_PRINTF("Wrong VPSS Channel:%d !\n", vpssChnID);
                return TMResult::TM_EINVAL;
            }
        }
        propList->Get(VPSS_DEVICE_ID, &deviceID);
        if(propList->Get(VPSS_ONLINE_MODE_ID, &isOnlineMode) == 0) {
            if(isOnlineMode) {
                /* find an unused online vpss */
                int i = 0;
                for(; (size_t)i < sizeof(mCviUsedOnlineVpss); i++) {
                    if(mCviUsedOnlineVpss[i] == false) {
                        break;
                    }
                }
                if(i == sizeof(mCviUsedOnlineVpss)) {
                    TMEDIA_PRINTF("cannot find online vpss not used\n");
                    return TMResult::TM_STATE_ERROR;
                }
                vpssGrpID = i;
                mOnlineMode = true;
                mCviUsedOnlineVpss[vpssGrpID]  = true;
                TMEDIA_PRINTF("Vpss: Group:%d online mode\n", vpssGrpID);
            }
        }
        if (propList->Get(TMVpss::PropID::START_FRAME_NUM, &startFrame) == 0)
        {
            if(startFrame < 0) {
                TMEDIA_PRINTF("Wrong VPSS start frame:%d !\n", startFrame);
                return TMResult::TM_EINVAL;
            }
        }
    } else {
        TMEDIA_PRINTF("VPSS: unvalid propList\n");
        return TMResult::TM_EINVAL;
    }

    /* 未传入channel id 自动选择一个 */
    if(vpssGrpID < 0) {
        if(GetAvaliableVpssGrp(vpssGrpID) != TMResult::TM_OK) {
            TMEDIA_PRINTF("No avaliable Vpss GrpID\n");
            return TMResult::TM_EBUSY;
        } else {
            TMEDIA_PRINTF("Auto Config Vpss Group:%d\n", vpssGrpID);
        }
    }
    if(vpssChnID < 0) {
        if(GetAvaliableVpssChn(vpssChnID) != TMResult::TM_OK) {
            TMEDIA_PRINTF("No avaliable Vpss Channel\n");
            return TMResult::TM_EBUSY;
        } else {
            TMEDIA_PRINTF("Auto Config Vpss Channel:%d\n", vpssChnID);
        }
    }
    mCviVpssGrp = vpssGrpID;
    mCviVpssChn = vpssChnID;
    TMEDIA_PRINTF("VPSS: Open Group:%d Channel:%d\n", mCviVpssGrp, mCviVpssChn);
    /*检查 vpss/chnn 组合是否被创建过*/
    for(int i=0; i < (int)mGroupChannelTable.size(); i++) {
        if((mGroupChannelTable[i].VpssGrpID == vpssGrpID) && (mGroupChannelTable[i].VpssChnID == vpssChnID)) {
            TMEDIA_PRINTF("VPSS: Group:%d Channel:%d been used\n", vpssGrpID, vpssChnID);
            return TMResult::TM_EINVAL;
        }
    }
    grpAttr.u8VpssDev = deviceID;
    std::map <int, TMSrcPadCvi*> ::iterator iter_src = mCviSrcPad.begin();
    std::map <int, TMSinkPadCvi*> ::iterator iter_sink = mCviSinkPad.begin();
    MMF_CHN_S Chn;
    Chn = iter_src->second->GetConfig();
    Chn.s32DevId = vpssGrpID;
    Chn.s32ChnId = vpssChnID;
    for(; iter_src != mCviSrcPad.end(); iter_src++) {
		iter_src->second->SetConfig(Chn);
	}
    for(; iter_sink != mCviSinkPad.end(); iter_sink++) {
		iter_sink->second->SetConfig(Chn);
	}

    if(mCviCreatedVpssGrp[vpssGrpID] == false) {
        mCviCreatedVpssGrp[vpssGrpID] = true;
        ret = CVI_VPSS_CreateGrp(vpssGrpID, &grpAttr);
        if (ret != CVI_SUCCESS)
        {
            TMEDIA_PRINTF("CVI_VPSS_CreateGrp failed\n");
            return TMResult::TM_BACKEND_ERROR;
        } else {
            TMEDIA_PRINTF("CVI_VPSS_CreateGrp:%d success\n", vpssGrpID);
        }
    } else {
        /*检查 vpss group 分辨率参数是否冲突 */
        for(int i=0; i < (int)mGroupChannelTable.size(); i++) {
            if(mGroupChannelTable[i].VpssGrpID == vpssGrpID) {
                if((mGroupChannelTable[i].WidthIn != (int)grpAttr.u32MaxW) || (mGroupChannelTable[i].HeightIn != (int)grpAttr.u32MaxH)) {
                    TMEDIA_PRINTF("VPSS: Group:%d param err\n", vpssGrpID);
                    return TMResult::TM_EINVAL;
                }
            }
        }
    }
    VPSS_CHN_ATTR_S chnAttr = {0};

    /* channel attr转换*/
    if (MapVpssChnParam(chnAttr, *propList) != TMResult::TM_OK)
    {
        TMEDIA_PRINTF("MapVpssChnParam failed\n");
        return TMResult::TM_MAPVAL_FAIL;
    }

    ret = CVI_VPSS_SetChnAttr(mCviVpssGrp, mCviVpssChn, &chnAttr);
    if (ret != CVI_SUCCESS)
    {
        TMEDIA_PRINTF("CVI_VPSS_SetChnAttr failed\n");
        return TMResult::TM_BACKEND_ERROR;
    }

    int RotationDegree;
    if(propList->Get(TMVpss::PropID::ROTATE_ENABLE, &RotationDegree) == 0) {
        if(RotationDegree == 90) {
            CVI_VPSS_SetChnRotation(mCviVpssGrp, mCviVpssChn, ROTATION_90);
        } else if(RotationDegree == 180) {
            CVI_VPSS_SetChnRotation(mCviVpssGrp, mCviVpssChn, ROTATION_180);
        } else if(RotationDegree == 270) {
            CVI_VPSS_SetChnRotation(mCviVpssGrp, mCviVpssChn, ROTATION_270);
        } else {
            TMEDIA_PRINTF("VPSS: err rotation degree\n");
        }
        TMEDIA_PRINTF("VPSS: Group:%d rotation:%d\n", mCviVpssGrp, RotationDegree);
    }

    VPSS_CROP_INFO_S stCropInfo;
    if(MapVpssCropParam(stCropInfo, *propList) == TMResult::TM_OK) {
        CVI_VPSS_SetGrpCrop(mCviVpssGrp, &stCropInfo);
        TMEDIA_PRINTF("VPSS: Crop Coordinate:%d X:%d Y:%d Width:%d Height:%d\n", stCropInfo.enCropCoordinate, stCropInfo.stCropRect.s32X, stCropInfo.stCropRect.s32Y, stCropInfo.stCropRect.u32Width, stCropInfo.stCropRect.u32Height);
    }

    ret = CVI_VPSS_EnableChn(mCviVpssGrp, mCviVpssChn);
    mCviOpenedVpssChn[vpssChnID] = true;

    int out_width, out_height;
    mCurrentPropertyList.Get(TMVpss::PropID::OUTPUT_WIDTH, &out_width);
    mCurrentPropertyList.Get(TMVpss::PropID::OUTPUT_HEIGHT, &out_height);

    grp_chn_combo_table_s param;
    param.VpssGrpID = mCviVpssGrp;
    param.VpssChnID = mCviVpssChn;
    param.WidthIn = grpAttr.u32MaxW;
    param.HeightIn = grpAttr.u32MaxH;
    param.WidthOut = out_width;
    param.HeightOut = out_height;
    mGroupChannelTable.push_back(param);
    if (ret != CVI_SUCCESS)
    {
        TMEDIA_PRINTF("CVI_VPSS_EnableChn failed\n");
        return TMResult::TM_BACKEND_ERROR;
    } else {
        TMEDIA_PRINTF("CVI_VPSS_EnableChn:%d success\n", mCviVpssChn);
    }
    TMEDIA_PRINTF("VPSS: In: Width:%d Height::%d Pixel Format:%d\n",  grpAttr.u32MaxW, grpAttr.u32MaxH, grpAttr.enPixelFormat);
    TMEDIA_PRINTF("VPSS: Out: Width:%d Height:%d Pixel Format:%d\n", out_width, out_height, chnAttr.enPixelFormat);
    TMEDIA_PRINTF("VPSS: deviceID:%d\n", deviceID);


#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    /* 因为无法判断vpss前面绑定的vi是RGB还是IR， 以deviceID=1， mCviVpssGrp = 0 作为ir0取帧的判断条件 */
    if((mCviVpssGrp == 0 || mCviVpssGrp == 1) && deviceID == 1) {
        // mFilterFrameFlag = true;
    } else {
        mFilterFrameFlag = false;
    }
#else
    if (startFrame) {
        mFilterFrameFlag = true;
        mStartFrame      = startFrame;
    }
#endif

    // VpssTableInfoPrint();
    return TMResult::TM_OK;
}

int TMVpssCvi::SetConfig(TMPropertyList &propList)
{
    int ret;
    VPSS_CHN_ATTR_S chnAttr = {0};
    /* channel attr转换*/
    if (MapVpssChnParam(chnAttr, propList) != TMResult::TM_OK)
    {
        TMEDIA_PRINTF("MapVpssChnParam failed\n");
        return TMResult::TM_MAPVAL_FAIL;
    }

    ret = CVI_VPSS_SetChnAttr(mCviVpssGrp, mCviVpssChn, &chnAttr);
    if (ret != CVI_SUCCESS)
    {
        TMEDIA_PRINTF("CVI_VPSS_SetChnAttr failed\n");
        return TMResult::TM_BACKEND_ERROR;
    }
    TMEDIA_PRINTF("VPSS: Config Out: Width:%d Height:%d Pixel Format:%d\n",  chnAttr.u32Width, chnAttr.u32Height, chnAttr.enPixelFormat);

    return TMResult::TM_OK;
}

int TMVpssCvi::GetConfig(TMPropertyList &propList)
{
    propList.Reset();
    propList = mCurrentPropertyList;
    return TMResult::TM_OK;
}

int TMVpssCvi::Start()
{
    int ret;
    if(mCviStartedVpssGrp[mCviVpssGrp] == false) {
        ret = CVI_VPSS_StartGrp(mCviVpssGrp);
        if (ret != CVI_SUCCESS)
        {
            TMEDIA_PRINTF("CVI_VPSS_StartGrp failed\n");
            return TMResult::TM_BACKEND_ERROR;
        }
        mCviStartedVpssGrp[mCviVpssGrp] = true;
        TMEDIA_PRINTF("VPSS: Start Group:%d Channel:%d\n", mCviVpssGrp, mCviVpssChn);
    }
    return TMResult::TM_OK;
}

int TMVpssCvi::Flush()
{
    return TMResult::TM_OK;
}

int TMVpssCvi::Stop()
{
    // int ret;
    // TMEDIA_PRINTF("VPSS: Stop Group:%d Channel:%d\n", mCviVpssGrp, mCviVpssChn);
    // ret = CVI_VPSS_DisableChn(mCviVpssGrp, mCviVpssChn);
    // if (ret != CVI_SUCCESS)
    // {
    //     TMEDIA_PRINTF("CVI_VPSS_DisableChn failed\n");
    //     return TMResult::TM_BACKEND_ERROR;
    // }
    // mCviOpenedVpssChn[mCviVpssChn] = false;
    // ret = CVI_VPSS_StopGrp(mCviVpssGrp);
    // if (ret != CVI_SUCCESS)
    // {
    //     TMEDIA_PRINTF("CVI_VPSS_StopGrp failed\n");
    //     return TMResult::TM_BACKEND_ERROR;
    // }
    // mCviStartedVpssGrp[mCviVpssGrp] = false;
    return TMResult::TM_OK;
}

int TMVpssCvi::Close()
{
    int ret = 0;
    /* 修改说明：
          晶视 online模式 时序要求很高 online的vpss deinit不能有间隔 必须一次性全部销毁 
    */
    if(mCviVpssGrp == 0) {
        TMEDIA_PRINTF("VPSS: online status:%d %d\n", mCviStartedVpssGrp[0], mCviStartedVpssGrp[1]);
        if(mCviStartedVpssGrp[0] == true) {
            CVI_VPSS_DisableChn(0, 0);
            CVI_VPSS_StopGrp(0);
            ret = CVI_VPSS_DestroyGrp(0);
            mCviStartedVpssGrp[0] = false;
        }
        
        if(mCviStartedVpssGrp[1] == true) {
            CVI_VPSS_DisableChn(1, 0);
            CVI_VPSS_StopGrp(1);
            ret = CVI_VPSS_DestroyGrp(1);
            mCviStartedVpssGrp[1] = false;
        }
    } else {
        CVI_VPSS_DisableChn(mCviVpssGrp, mCviVpssChn);
        CVI_VPSS_StopGrp(mCviVpssGrp);
        ret = CVI_VPSS_DestroyGrp(mCviVpssGrp);
        TMEDIA_PRINTF("VPSS: Stop Group:%d Channel:%d\n", mCviVpssGrp, mCviVpssChn);
    }
    mCviStartedVpssGrp[mCviVpssGrp] = false;
    mCviOpenedVpssChn[mCviVpssChn] = false;
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VPSS_DestroyGrp() failed, ret =%d\n", ret);
        return TMResult::TM_BACKEND_ERROR;
    }
    vector<grp_chn_combo_table_s>::iterator itVec;
    for(int i=0; i < (int)mGroupChannelTable.size(); i++) {
        if((mGroupChannelTable[i].VpssGrpID == mCviVpssGrp) && (mGroupChannelTable[i].VpssChnID == mCviVpssChn)) {
            mGroupChannelTable.erase(mGroupChannelTable.begin() + i);
        }
    }
    mCviCreatedVpssGrp[mCviVpssGrp] = false;
    if(mOnlineMode) {
        mCviUsedOnlineVpss[mCviVpssGrp] = false;
    }
    mCviVpssGrp = -1;
    return TMResult::TM_OK;
}

int TMVpssCvi::SendFrame(TMVideoFrame &frame, int timeout)
{
    int ret;
    TMBuffer * frame_buf = frame.GetNativeFrameCtx();
    TMNativeFrameCtx *ctx  = (TMNativeFrameCtx *)TMBuffer_Data(frame_buf);
    ret = CVI_VPSS_SendChnFrame(this->mCviVpssGrp, this->mCviVpssChn, (VIDEO_FRAME_INFO_S *)ctx->priv, timeout);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VPSS_SendChnFrame err ret:%d\n", ret);
        return TMResult::TM_STATE_ERROR;
    }
    return TMResult::TM_OK;
}

static int cvi_release_frame_func(struct TMNativeFrameCtx *nativeFrameCtx)
{		
    if (nativeFrameCtx) {		
        CviNativeFrameCtx  *frame_ctx = (CviNativeFrameCtx  *)nativeFrameCtx->priv;	
        CVI_VPSS_ReleaseChnFrame(frame_ctx->deviceID, frame_ctx->channelID, &frame_ctx->pstFrameInfo);
        free(frame_ctx);
    }		
    return TMResult::TM_OK;
}
extern "C" CVI_U32 AE_GetFrameID(CVI_U8 sID, CVI_U32 *frameID);
extern "C" CVI_U8 AE_ViPipe2sID(VI_PIPE ViPipe);
int TMVpssCvi::RecvFrame(TMVideoFrame &frame, int timeout)
{
    int ret;
    CviNativeFrameCtx *frame_ctx = (CviNativeFrameCtx*)malloc(sizeof(CviNativeFrameCtx));;
    frame_ctx->deviceID = mCviVpssGrp;
    frame_ctx->channelID = mCviVpssChn;

GETARAIN:
    ret = CVI_VPSS_GetChnFrame(mCviVpssGrp, mCviVpssChn, &frame_ctx->pstFrameInfo, timeout);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VPSS_GetChnFrame err ret:%d\n", ret);
        goto GETARAIN;
    }

    /* 创建vi后 前几帧无法快速收敛，过滤掉前5帧 */
    if(mFilterFrameFlag) {
        while(1) {
            CVI_U8 sID = AE_ViPipe2sID(mCviVpssGrp);    //vpss group0->pipe0    vpss group1->pipe1
            CVI_U32 frameID;
            AE_GetFrameID(sID, &frameID);
            if(frameID < (uint32_t)mStartFrame) {
                CVI_VPSS_ReleaseChnFrame(mCviVpssGrp, mCviVpssChn, &frame_ctx->pstFrameInfo);
                goto GETARAIN;
            } else {
                break;
            }
        }
        mFilterFrameFlag = false;
    } 
    TMBuffer *ctxBuf = TMNativeFrameCtx::CreateCtxBuffer();		
    TMNativeFrameCtx *ctx  = (TMNativeFrameCtx *)TMBuffer_Data(ctxBuf);		
    ctx->internalBuffer = true;		
    ctx->priv = frame_ctx;		
    ctx->freeFunc = cvi_release_frame_func; //set free function		
    frame.SetNativeFrameCtx(ctxBuf);		
    TMBuffer_UnRef(ctxBuf);		
    ctxBuf = NULL;

    frame.mData[0] = (uint8_t *)frame_ctx->pstFrameInfo.stVFrame.u64PhyAddr[0];
    frame.mData[1] = (uint8_t *)frame_ctx->pstFrameInfo.stVFrame.u64PhyAddr[1];
    frame.mData[2] = (uint8_t *)frame_ctx->pstFrameInfo.stVFrame.u64PhyAddr[2];

    frame.mStride[0] = frame_ctx->pstFrameInfo.stVFrame.u32Stride[0];
    frame.mStride[1] = frame_ctx->pstFrameInfo.stVFrame.u32Stride[1];
    frame.mStride[2] = frame_ctx->pstFrameInfo.stVFrame.u32Stride[2];

    int pixelFormat;
    mCurrentPropertyList.Get(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, &pixelFormat);
    frame.mPixelFormat = (TMImageInfo::PixelFormat)pixelFormat;
    mCurrentPropertyList.Get(TMVpss::PropID::OUTPUT_WIDTH, &frame.mWidth);
    mCurrentPropertyList.Get(TMVpss::PropID::OUTPUT_HEIGHT, &frame.mHeight);

    return TMResult::TM_OK;
}

int TMVpssCvi::GetAvaliableVpssGrp(VPSS_GRP &vpssGrp)
{
    // TODO: Add mutex to protect re-entrant
    for (uint32_t i = 0; i < ARRAY_SIZE(mCviCreatedVpssGrp); i++)
    {
        if (mCviCreatedVpssGrp[i] == false)
        {
            vpssGrp = i;
            return TMResult::TM_OK;
        }
    }
    return TMResult::TM_EBUSY;
}

int TMVpssCvi::GetAvaliableVpssChn(VPSS_CHN &vpssChn)
{
    // TODO: Add mutex to protect re-entrant
    for (uint32_t i = 0; i < ARRAY_SIZE(mCviOpenedVpssChn); i++)
    {
        if (mCviOpenedVpssChn[i] == false)
        {
            vpssChn = i;
            return TMResult::TM_OK;
        }
    }
    return TMResult::TM_EBUSY;
}

REGISTER_VPSS_CLASS(TMVpssCvi)


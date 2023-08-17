/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_cvitek/cvi_encode/h264_encode_cvi.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"

TMH264EncoderCVI::TMH264EncoderCVI()
{
    mEncodeRunningFlag = 0;
    mFirstCreateFlag = 1;
    mChannelID = -1;

    MMF_CHN_S Chn;

    TMSrcPadCvi *SrcPad = new TMSrcPadCvi(this, NULL);
    TMSinkPadCvi *SinkPad = new TMSinkPadCvi(this, NULL);
    Chn.enModId = CVI_ID_VENC;
    Chn.s32DevId = -1;
    Chn.s32ChnId = -1;

    SrcPad->SetConfig(Chn);
    SinkPad->SetConfig(Chn);
    mCviSrcPad.insert(std::make_pair(0, SrcPad));
    mCviSinkPad.insert(std::make_pair(0, SinkPad));

    pthread_mutex_init(&mEncodeMutex, NULL);
    mCodecID = TMMediaInfo::CodecID::H264;

    InitDefaultPropertyList();
}

TMH264EncoderCVI::~TMH264EncoderCVI()
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
    pthread_mutex_destroy(&mEncodeMutex);
}

TMSrcPad *TMH264EncoderCVI::GetSrcPad(int padID) 
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
	TMEDIA_PRINTF("VPSS: create new src pad:%d\n", padID);
	return SrcPad;
}

TMSinkPad *TMH264EncoderCVI::GetSinkPad(int padID)
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
	TMEDIA_PRINTF("VPSS: create new sink pad:%d\n", padID);
	return SinkPad;
}


int TMH264EncoderCVI::Open(TMCodecParams &codecParam, TMPropertyList *propList) 
{
    VENC_CHN VeChn = -1;
    int GopNum, TargetBitrate, FPS;
    TMVideoEncoderParams *codec_param = dynamic_cast<TMVideoEncoderParams *>(&codecParam);
    if (codec_param == NULL)
    {
        TMEDIA_PRINTF("Input codecParam can't convert to (TMVideoEncoderParams *)\n");
        return TMResult::TM_EINVAL;
    }

    for (uint32_t i = 0; i < ARRAY_SIZE(gCviOpenedEncodeChn); i++) {
        if (gCviOpenedEncodeChn[i] == false) {
            VeChn = i;
            break;
        }
    }
    if(VeChn == -1) {
        TMEDIA_PRINTF("Encode: cannot find channel\n");
    }
    if(mFirstCreateFlag == 1) {
        mDefaultPropertyList.Get(TMH264Encoder::PropID::OUTPUT_GOP_NUMBER, &GopNum);
        mDefaultPropertyList.Get(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, &TargetBitrate);
        mDefaultPropertyList.Get(TMH264Encoder::PropID::OUTPUT_FPS, &FPS);
    } else {
        mCurrentPropertyList.Get(TMH264Encoder::PropID::OUTPUT_GOP_NUMBER, &GopNum);
        mCurrentPropertyList.Get(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, &TargetBitrate);
        mCurrentPropertyList.Get(TMH264Encoder::PropID::OUTPUT_FPS, &FPS);
    }
    
    if (propList != NULL)
    {
        propList->Get(TMH264Encoder::PropID::OUTPUT_GOP_NUMBER, &GopNum);
        propList->Get(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, &TargetBitrate);
        propList->Get(TMH264Encoder::PropID::OUTPUT_FPS, &FPS);
    }
    mCurrentPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_GOP_NUMBER, GopNum);
    mCurrentPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, TargetBitrate);
    mCurrentPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_FPS, FPS);
    TMEDIA_PRINTF("Encode Open, codec id:%d\n", (int)mCodecID);
    TMEDIA_PRINTF("Encode: Width:%d  Height:%d  GOP:%d Target Bitrate:%dkb FPS:%d/s\n", codec_param->mWidth, codec_param->mHeight, GopNum, TargetBitrate, FPS);
    this->mChannelID = VeChn;

    std::map <int, TMSrcPadCvi*> ::iterator iter_src = mCviSrcPad.begin();
    std::map <int, TMSinkPadCvi*> ::iterator iter_sink = mCviSinkPad.begin();
    MMF_CHN_S Chn;
    Chn = iter_src->second->GetConfig();
    Chn.s32DevId = 0;
    Chn.s32ChnId = VeChn;
    for(; iter_src != mCviSrcPad.end(); iter_src++) {
		iter_src->second->SetConfig(Chn);
	}
    for(; iter_sink != mCviSinkPad.end(); iter_sink++) {
		iter_sink->second->SetConfig(Chn);
	}

    VENC_CHN_ATTR_S VencChnAttr;
    memset(&VencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
    VencChnAttr.stVencAttr.enType = PT_H264;
    VencChnAttr.stVencAttr.stAttrH264e.bSingleLumaBuf = 0;
    VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    VencChnAttr.stRcAttr.stH264Cbr.bVariFpsEn = 0;
    VencChnAttr.stRcAttr.stH264Cbr.u32StatTime = 2;              //the rate statistic time, the unit is senconds(s)
    VencChnAttr.stRcAttr.stH264Cbr.u32SrcFrameRate = 30;
    VencChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = FPS;
    VencChnAttr.stRcAttr.stH264Cbr.u32BitRate = TargetBitrate;
    VencChnAttr.stRcAttr.stH264Cbr.u32Gop = GopNum;
	VencChnAttr.stVencAttr.u32MaxPicWidth = codec_param->mWidth;
	VencChnAttr.stVencAttr.u32MaxPicHeight = codec_param->mHeight;
    VencChnAttr.stVencAttr.u32PicWidth = codec_param->mWidth;
	VencChnAttr.stVencAttr.u32PicHeight = codec_param->mHeight;

	VencChnAttr.stVencAttr.u32BufSize = 1024*1024;
	VencChnAttr.stVencAttr.u32Profile = 0;
	VencChnAttr.stVencAttr.bByFrame = CVI_TRUE;
	VencChnAttr.stVencAttr.bSingleCore = 0;
	VencChnAttr.stVencAttr.bEsBufQueueEn = 0;
	VencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
	VencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_CreateChn(VeChn, &VencChnAttr));
    TMEDIA_PRINTF("CVI_VENC_CreateChn chn:%d success\n", VeChn);

    VENC_PARAM_MOD_S stModParam;
    memset(&stModParam, 0, sizeof(VENC_PARAM_MOD_S));
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetModParam(&stModParam));
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetModParam(&stModParam));

	VENC_RC_PARAM_S stRcParam = {0};
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetRcParam(VeChn, &stRcParam));
	stRcParam.s32FirstFrameStartQp = 30;
	stRcParam.s32InitialDelay = CVI_INITIAL_DELAY_DEFAULT;
	stRcParam.u32ThrdLv = 2;
	stRcParam.s32BgDeltaQp = 0;
    stRcParam.stParamH264Cbr.bQpMapEn = CVI_FALSE;
    stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
    stRcParam.stParamH264Cbr.u32MaxIprop = 100;
    stRcParam.stParamH264Cbr.u32MinIprop = 1;
    stRcParam.stParamH264Cbr.u32MaxIQp = CVI_H26X_MAXQP_DEFAULT;
    stRcParam.stParamH264Cbr.u32MaxQp = CVI_H26X_MAXQP_DEFAULT;
    stRcParam.stParamH264Cbr.u32MinIQp = CVI_H26X_MINQP_DEFAULT;
    stRcParam.stParamH264Cbr.u32MinQp = CVI_H26X_MINQP_DEFAULT;

	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetRcParam(VeChn, &stRcParam));

	VENC_REF_PARAM_S stRefParam = {0};
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetRefParam(VeChn, &stRefParam));
	stRefParam.bEnablePred = 0;
	stRefParam.u32Base = 0;
	stRefParam.u32Enhance = 0;
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetRefParam(VeChn, &stRefParam));

	VENC_CHN_PARAM_S stChnParam = {0};
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetChnParam(VeChn, &stChnParam));
	stChnParam.stFrameRate.s32SrcFrmRate = FPS;                             //Input frame rate of a channel
	stChnParam.stFrameRate.s32DstFrmRate = FPS;                             //Output frame rate of a channel
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetChnParam(VeChn, &stChnParam));

	VENC_ROI_ATTR_S stRoiAttr = {0};
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetRoiAttr(VeChn, 0, &stRoiAttr));
	stRoiAttr.bEnable = CVI_FALSE;
	stRoiAttr.bAbsQp = CVI_FALSE;
	stRoiAttr.s32Qp = -2;
	stRoiAttr.u32Index = 0;
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetRoiAttr(VeChn, &stRoiAttr));

	VENC_FRAMELOST_S stFrmLostParam = {0};
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetFrameLostStrategy(VeChn, &stFrmLostParam));
	stFrmLostParam.bFrmLostOpen = 0;
	stFrmLostParam.enFrmLostMode = FRMLOST_PSKIP;
	stFrmLostParam.u32EncFrmGaps = 0;
	stFrmLostParam.u32FrmLostBpsThr = 0;
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetFrameLostStrategy(VeChn, &stFrmLostParam));

	VENC_SUPERFRAME_CFG_S stSuperFrmParam;
    memset(&stSuperFrmParam, 0, sizeof(VENC_SUPERFRAME_CFG_S));
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetSuperFrameStrategy(VeChn, &stSuperFrmParam));
	stSuperFrmParam.enSuperFrmMode = SUPERFRM_NONE;
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetSuperFrameStrategy(VeChn, &stSuperFrmParam));

	VENC_CU_PREDICTION_S stCuPrediction;
    memset(&stCuPrediction, 0, sizeof(VENC_CU_PREDICTION_S));
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetCuPrediction(VeChn, &stCuPrediction));
	stCuPrediction.u32IntraCost = 0;
	UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetCuPrediction(VeChn, &stCuPrediction));
    VENC_H264_TRANS_S stH264Trans = {0};
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetH264Trans(VeChn, &stH264Trans));
    stH264Trans.chroma_qp_index_offset = 0;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetH264Trans(VeChn, &stH264Trans));

    VENC_H264_ENTROPY_S stH264EntropyEnc = {0};
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetH264Entropy(VeChn, &stH264EntropyEnc));
    stH264EntropyEnc.u32EntropyEncModeI = stH264EntropyEnc.u32EntropyEncModeP = 0;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetH264Entropy(VeChn, &stH264EntropyEnc));

    VENC_H264_VUI_S stH264Vui = {0};
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetH264Vui(VeChn, &stH264Vui));
    stH264Vui.stVuiTimeInfo.timing_info_present_flag = 0;
    stH264Vui.stVuiAspectRatio.aspect_ratio_info_present_flag = 0;
    stH264Vui.stVuiBitstreamRestric.bitstream_restriction_flag = 0;
    stH264Vui.stVuiVideoSignal.video_signal_type_present_flag = 0;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetH264Vui(VeChn, &stH264Vui));          

    gCviOpenedEncodeChn[this->mChannelID] = true;
    mFirstCreateFlag = 0;
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::SetConfig(TMPropertyList &propertyList) 
{
    int ret;
    int mTargetBitrate = -1;
    VENC_CHN_ATTR_S VencChnAttr;
	ret = CVI_VENC_GetChnAttr(this->mChannelID, &VencChnAttr);
	if (ret != CVI_SUCCESS) {
		TMEDIA_PRINTF("CVI_VENC_GetChnAttr, VencChn = %d ret =%d\n", this->mChannelID, ret);
		return TMResult::TM_STATE_ERROR;
	}
    if (propertyList.Get(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, &mTargetBitrate) == 0)
    {
        VencChnAttr.stRcAttr.stH264Cbr.u32BitRate = mTargetBitrate;
        ret =  CVI_VENC_SetChnAttr(this->mChannelID, &VencChnAttr);
        if(ret != 0) {
            TMEDIA_PRINTF("CVI_VENC_SetChnAttr failed\n");
            return TMResult::TM_STATE_ERROR;
        }
        TMEDIA_PRINTF("Encode: Change target bitrate:%d\n", mTargetBitrate);
        mCurrentPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, mTargetBitrate);
    }
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::GetConfig(TMPropertyList &propertyList) 
{
    propertyList.Reset();
    propertyList = mCurrentPropertyList;
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::Start() 
{
    int ret;
    pthread_mutex_lock(&mEncodeMutex);
    VENC_RECV_PIC_PARAM_S stRecvParam = {0};

	stRecvParam.s32RecvPicNum = -1;
	ret = CVI_VENC_StartRecvFrame(this->mChannelID, &stRecvParam);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VENC_StartRecvFrame channel:%d err\n", this->mChannelID); 
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
    TMEDIA_PRINTF("CVI_VENC_StartRecvFrame chn:%d success\n", this->mChannelID);
    mEncodeRunningFlag = 1;
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::Flush() 
{
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::Stop() 
{
    int ret;
    pthread_mutex_lock(&mEncodeMutex);
	ret = CVI_VENC_StopRecvFrame(this->mChannelID);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VENC_StopRecvFrame channel:%d err\n", this->mChannelID); 
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
    mEncodeRunningFlag = 0;
    pthread_mutex_unlock(&mEncodeMutex);
    TMEDIA_PRINTF("Encode stop, channel id:%d\n", this->mChannelID);
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::Close()
{
    int ret;
    pthread_mutex_lock(&mEncodeMutex);
	ret = CVI_VENC_ResetChn(this->mChannelID);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VENC_ResetChn channel:%d err\n", this->mChannelID); 
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
	ret = CVI_VENC_DestroyChn(this->mChannelID);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VENC_DestroyChn channel:%d err\n", this->mChannelID); 
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
    gCviOpenedEncodeChn[this->mChannelID] = false;
    pthread_mutex_unlock(&mEncodeMutex);
    TMEDIA_PRINTF("Encode close, channel id:%d\n", this->mChannelID);
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::SendFrame(TMVideoFrame &frame, int timeout)
{
    int ret;
    TMBuffer * frame_buf = frame.GetNativeFrameCtx();
    TMNativeFrameCtx *ctx  = (TMNativeFrameCtx *)TMBuffer_Data(frame_buf);
    
    ret = CVI_VENC_SendFrame(this->mChannelID, (VIDEO_FRAME_INFO_S *)ctx->priv, timeout);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VENC_SendFrame err\n");
        return TMResult::TM_STATE_ERROR;
    }
    return TMResult::TM_OK;
}

int TMH264EncoderCVI::RecvPacket(TMVideoPacket &pkt, int timeout) 
{
    CVI_S32 ret = CVI_SUCCESS;
	VENC_STREAM_S stStream = {0};
	VENC_CHN_STATUS_S stStatus = {0};
    int pkt_len_total = 0;

	VENC_PACK_S *ppack = NULL;

    pthread_mutex_lock(&mEncodeMutex);
    if(mEncodeRunningFlag == 0) {
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_EACCES;
    }
	ret = CVI_VENC_QueryStatus(this->mChannelID, &stStatus); 
    if(ret != 0) {
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
	stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
	if (stStream.pstPack == NULL) {
		TMEDIA_PRINTF("%s %d malloc fail\n", __func__, __LINE__);
        pthread_mutex_unlock(&mEncodeMutex);
		return TMResult::TM_ENOMEM;
	}

	ret = CVI_VENC_GetStream(this->mChannelID, &stStream, timeout);
	if((ret != CVI_SUCCESS) || (stStream.u32PackCount == 0)) {
        free(stStream.pstPack);
        pthread_mutex_unlock(&mEncodeMutex);
		return TMResult::TM_EAGAIN;
	}
    for(int i = 0; i < (int)stStream.u32PackCount; i++) {
        ppack = &stStream.pstPack[i];
        pkt_len_total += ppack->u32Len - ppack->u32Offset;
    }

    if(pkt.mData == NULL) {
        pkt.PrepareBuffer(pkt_len_total);
        pkt.mDataLength = 0;
    }
    /* VENC_STREAM_S -> TMPacket*/
    for(int i = 0; i < (int)stStream.u32PackCount; i++) {
        ppack = &stStream.pstPack[i];
        ret = pkt.Append(ppack->pu8Addr + ppack->u32Offset, ppack->u32Len - ppack->u32Offset);
        if(ret != TMResult::TM_OK) {
            TMEDIA_PRINTF("packet append err ret:%d len:%d/%d\n", ret, ppack->u32Len - ppack->u32Offset, pkt.mDataMaxLength);
            TMEDIA_PRINTF("pkt count:%d cur:%d\n", stStream.u32PackCount, i);
            for(int j = 0; j<(int)stStream.u32PackCount; j++) {
                TMEDIA_PRINTF("pkt:%d len:%d\n", j,  stStream.pstPack[j].u32Len - stStream.pstPack[j].u32Offset);
            }
            free(stStream.pstPack);
            pthread_mutex_unlock(&mEncodeMutex);
            return ret;
        }
    }
	ret = CVI_VENC_ReleaseStream(this->mChannelID, &stStream);
    if(ret != 0) {
        TMEDIA_PRINTF("CVI_VENC_ReleaseStream channel:%d err\n", this->mChannelID); 
        free(stStream.pstPack);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }

    int is_iframe = CheckVideoIsIframe(&stStream, mCodecID);
    if(is_iframe) {
        pkt.mPictureType = TMMediaInfo::PictureType::I;
    } else {
        pkt.mPictureType = TMMediaInfo::PictureType::P;
    }

    TMClock_t pts;
    pts.timestamp = ppack->u64PTS;
    pts.time_base = 1000000;

    pkt.mPTS.Set(pts);
    pkt.mDTS.Set(pts);

	if (stStream.pstPack != NULL) {
		free(stStream.pstPack);
	}
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_OK;
}

REGISTER_VIDEO_ENCODER_CLASS(TMMediaInfo::CodecID::H264, TMH264EncoderCVI)
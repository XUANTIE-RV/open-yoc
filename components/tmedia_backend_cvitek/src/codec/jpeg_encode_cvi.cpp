/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_cvitek/cvi_encode/jpeg_encode_cvi.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"

TMJpegEncoderCVI::TMJpegEncoderCVI()
{
    mEncodeRunningFlag = 0;
    mChannelID = -1;

    MMF_CHN_S Chn;
    Chn.enModId = CVI_ID_VENC;
    Chn.s32DevId = -1;
    Chn.s32ChnId = -1;

    mCviSrcPad = new TMSrcPadCvi(this, NULL);
    mCviSrcPad->SetConfig(Chn);
    mCviSinkPad = new TMSinkPadCvi(this, NULL);
    mCviSinkPad->SetConfig(Chn);

    pthread_mutex_init(&mEncodeMutex, NULL);
    mCodecID = TMMediaInfo::CodecID::JPEG;

    int tmQfactor = -1;
    InitDefaultPropertyList();
    mDefaultPropertyList.Get(PropID::ENCODE_QUALITY_FACTOR, &tmQfactor);
    mCurrentPropertyList.Reset();
    mCurrentPropertyList.Add(TMProperty((int)PropID::ENCODE_QUALITY_FACTOR, tmQfactor, "quality factor"));
}

TMJpegEncoderCVI::~TMJpegEncoderCVI()
{
    if (mCviSrcPad != NULL)
    {
        delete mCviSrcPad;
    }
    if (mCviSinkPad != NULL)
    {
        delete mCviSinkPad;
    }
    pthread_mutex_destroy(&mEncodeMutex);
}

TMSrcPad *TMJpegEncoderCVI::GetSrcPad(int padID)
{
    return mCviSrcPad;
}

TMSinkPad *TMJpegEncoderCVI::GetSinkPad(int padID)
{
    return mCviSinkPad;
}

int TMJpegEncoderCVI::Open(TMCodecParams &codecParam, TMPropertyList *propList)
{
    VENC_CHN VeChn = -1;
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
    int tmQfactor = -1;
    if (propList != NULL && (propList->Get(PropID::ENCODE_QUALITY_FACTOR, &tmQfactor) == 0))
    {
        if(tmQfactor > PIC_QUALITY_MAX) {
            tmQfactor = PIC_QUALITY_MAX;
        } else if(tmQfactor < PIC_QUALITY_MIN) {
            tmQfactor = PIC_QUALITY_MIN;
        }
        mCurrentPropertyList.Assign(PropID::ENCODE_QUALITY_FACTOR, tmQfactor);
    } else {
        mCurrentPropertyList.Get(PropID::ENCODE_QUALITY_FACTOR, &tmQfactor);
    }
    if(VeChn == -1) {
        TMEDIA_PRINTF("Encode: cannot find channel\n");
    }
    TMEDIA_PRINTF("Jpeg Encode Channel:%d Quality:%d\n", VeChn, tmQfactor);
    this->mChannelID = VeChn;
    MMF_CHN_S Chn;
    Chn = mCviSrcPad->GetConfig();
    Chn.s32DevId = 0;
    Chn.s32ChnId = VeChn;
    mCviSrcPad->SetConfig(Chn);
    mCviSinkPad->SetConfig(Chn);

    VENC_CHN_ATTR_S VencChnAttr;
    memset(&VencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
    VencChnAttr.stVencAttr.enType = PT_JPEG;
    VencChnAttr.stVencAttr.u32MaxPicWidth = codec_param->mWidth;
    VencChnAttr.stVencAttr.u32MaxPicHeight = codec_param->mHeight;
    VencChnAttr.stVencAttr.u32PicWidth = codec_param->mWidth;
    VencChnAttr.stVencAttr.u32PicHeight = codec_param->mHeight;

    VencChnAttr.stRcAttr.stMjpegCbr.fr32DstFrameRate = 25;
    VencChnAttr.stRcAttr.stMjpegCbr.u32BitRate = 1024;
    VencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;

    VencChnAttr.stVencAttr.u32BufSize = 1024 * 1024;
    VencChnAttr.stVencAttr.u32Profile = 0;
    VencChnAttr.stVencAttr.bByFrame = CVI_TRUE;
    VencChnAttr.stVencAttr.bSingleCore = 0;
    VencChnAttr.stVencAttr.bEsBufQueueEn = 0;
    VencChnAttr.stRcAttr.stMjpegCbr.bVariFpsEn = 0;
    VencChnAttr.stRcAttr.stMjpegCbr.u32StatTime = 2;
    VencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    VencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_CreateChn(VeChn, &VencChnAttr));

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
    stRcParam.stParamMjpegCbr.u32MaxQfactor = 99;
    stRcParam.stParamMjpegCbr.u32MinQfactor = 1;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetRcParam(VeChn, &stRcParam));

    VENC_REF_PARAM_S stRefParam = {0};
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetRefParam(VeChn, &stRefParam));
    stRefParam.bEnablePred = 0;
    stRefParam.u32Base = 0;
    stRefParam.u32Enhance = 0;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetRefParam(VeChn, &stRefParam));

    VENC_CHN_PARAM_S stChnParam = {0};
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetChnParam(VeChn, &stChnParam));
    stChnParam.stFrameRate.s32SrcFrmRate = 25; // Input frame rate of a channel
    stChnParam.stFrameRate.s32DstFrmRate = 25; // Output frame rate of a channel
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

    VENC_JPEG_PARAM_S stJpegParam = {0};
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_GetJpegParam(VeChn, &stJpegParam));

    // FIXME: workaround cvitek bug, quality 50 would lead to set param error
    stJpegParam.u32Qfactor = tmQfactor == 50 ? 49 : tmQfactor;
    UTIL_CVI_CHECK_RET_WITH_VALUE(CVI_VENC_SetJpegParam(VeChn, &stJpegParam));

    gCviOpenedEncodeChn[this->mChannelID] = true;
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::SetConfig(TMPropertyList &propertyList)
{
    int tmQfactor = -1;
    // propertyList.Dump();
    if (propertyList.Get(PropID::ENCODE_QUALITY_FACTOR, &tmQfactor) == 0)
    {
        if(tmQfactor > PIC_QUALITY_MAX) {
            tmQfactor = PIC_QUALITY_MAX;
        } else if(tmQfactor < PIC_QUALITY_MIN) {
            tmQfactor = PIC_QUALITY_MIN;
        }
        mCurrentPropertyList.Assign(PropID::ENCODE_QUALITY_FACTOR, tmQfactor);
        TMEDIA_PRINTF("Encode: change jpeg encode quality:%d\n", tmQfactor);
    }
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::GetConfig(TMPropertyList &propertyList)
{
    propertyList.Reset();
    propertyList = mCurrentPropertyList;
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::Start()
{
    int ret;
    pthread_mutex_lock(&mEncodeMutex);
    VENC_RECV_PIC_PARAM_S stRecvParam = {0};

    stRecvParam.s32RecvPicNum = -1;
    ret = CVI_VENC_StartRecvFrame(this->mChannelID, &stRecvParam);
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VENC_StartRecvFrame channel:%d err\n", this->mChannelID);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
    mEncodeRunningFlag = 1;
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::Flush()
{
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::Stop()
{
    int ret;
    pthread_mutex_lock(&mEncodeMutex);
    ret = CVI_VENC_StopRecvFrame(this->mChannelID);
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VENC_StopRecvFrame channel:%d err\n", this->mChannelID);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
    mEncodeRunningFlag = 0;
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::Close()
{
    int ret;
    pthread_mutex_lock(&mEncodeMutex);
    ret = CVI_VENC_ResetChn(this->mChannelID);
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VENC_ResetChn channel:%d err\n", this->mChannelID);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }
    ret = CVI_VENC_DestroyChn(this->mChannelID);
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VENC_DestroyChn channel:%d err\n",  this->mChannelID);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_STATE_ERROR;
    }

    gCviOpenedEncodeChn[this->mChannelID] = false;
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::SendFrame(TMVideoFrame &frame, int timeout)
{
    int ret;

    if (mEncodeRunningFlag == 0)
    {
        TMEDIA_PRINTF("encode not start\n");
        return TMResult::TM_EACCES;
    }

    TMBuffer *frame_buf = frame.GetNativeFrameCtx();
    TMNativeFrameCtx *ctx = (TMNativeFrameCtx *)TMBuffer_Data(frame_buf);
    CviNativeFrameCtx *frame_ctx = (CviNativeFrameCtx *)ctx->priv;

    ret = CVI_VENC_SendFrame(this->mChannelID, &(frame_ctx->pstFrameInfo), timeout);
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VENC_SendFrame err\n");
        return TMResult::TM_STATE_ERROR;
    }
    return TMResult::TM_OK;
}

int TMJpegEncoderCVI::RecvPacket(TMVideoPacket &pkt, int timeout)
{
    CVI_S32 ret = CVI_SUCCESS;
    VENC_STREAM_S stStream = {0};
    VENC_CHN_STATUS_S stStatus = {0};
    int pkt_len_total = 0;

    VENC_PACK_S *ppack = NULL;

    if (mEncodeRunningFlag == 0)
    {
        TMEDIA_PRINTF("encode not start\n");
        return TMResult::TM_EACCES;
    }
    pthread_mutex_lock(&mEncodeMutex);

    while (1)
    {
        ret = CVI_VENC_QueryStatus(this->mChannelID, &stStatus);
        if (ret == CVI_ERR_VENC_BUSY)
        {
            usleep(10000);
            continue;
        }
        else if (ret != CVI_SUCCESS)
        {
            pthread_mutex_unlock(&mEncodeMutex);
            return TMResult::TM_STATE_ERROR;
        }
        break;
    }
    stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
    if (stStream.pstPack == NULL)
    {
        TMEDIA_PRINTF("%s %d malloc fail\n", __func__, __LINE__);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_ENOMEM;
    }

    ret = CVI_VENC_GetStream(this->mChannelID, &stStream, timeout);
    if ((ret != CVI_SUCCESS) || (stStream.u32PackCount == 0))
    {
        free(stStream.pstPack);
        pthread_mutex_unlock(&mEncodeMutex);
        return TMResult::TM_EAGAIN;
    }
    ppack = &stStream.pstPack[0];
    pkt_len_total = ppack->u32Len - ppack->u32Offset;

    if (pkt.mData == NULL)
    {
        pkt.PrepareBuffer(pkt_len_total);
        pkt.mDataLength = 0;
    }
    /* VENC_STREAM_S -> TMPacket*/
    ret = pkt.Append(ppack->pu8Addr + ppack->u32Offset, ppack->u32Len - ppack->u32Offset);
    if (ret != TMResult::TM_OK)
    {
        TMEDIA_PRINTF("packet append err ret:%d len:%d/%d\n", ret, ppack->u32Len - ppack->u32Offset, pkt.mDataMaxLength);
        goto err;
    }
    ret = CVI_VENC_ReleaseStream(this->mChannelID, &stStream);
    if (ret != 0)
    {
        TMEDIA_PRINTF("CVI_VENC_ReleaseStream channel:%d err,ret:%d\n", this->mChannelID, ret);
        goto err;
    }
    TMClock_t pts;
    pts.timestamp = ppack->u64PTS;
    pts.time_base = 1000000;

    pkt.mPTS.Set(pts);

    free(stStream.pstPack);
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_OK;
err:
    free(stStream.pstPack);
    pthread_mutex_unlock(&mEncodeMutex);
    return TMResult::TM_STATE_ERROR;
}

REGISTER_VIDEO_ENCODER_CLASS(TMMediaInfo::CodecID::JPEG, TMJpegEncoderCVI)
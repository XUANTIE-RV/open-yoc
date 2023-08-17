/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_CVI_H264_ENCODER_H
#define TM_CVI_H264_ENCODER_H

#include <tmedia_core/entity/codec/codec_inc.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include "cvi_venc.h"

class TMH264EncoderCVI : public TMH264Encoder
{
public:
    TMH264EncoderCVI();
    virtual ~TMH264EncoderCVI();

    // TMFilterEntity interface
    TMSrcPad *GetSrcPad(int padID = 0) final override;
    TMSinkPad *GetSinkPad(int padID = 0) final override;
    // TMCodec interface
    int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) final override;
    int SetConfig(TMPropertyList &propertyList) final override;
    int GetConfig(TMPropertyList &propertyList) final override;
    int Start()                                 final override;
    int Flush()                                 final override;
    int Stop()                                  final override;
    int Close()                                 final override;

    // TMVideoEncoder interface
    int SendFrame(TMVideoFrame &frame, int timeout) final override;
    int RecvPacket(TMVideoPacket &pkt, int timeout) final override;

private:
    int mChannelID;
    int mEncodeRunningFlag;
    int mFirstCreateFlag;
    map<int, TMSrcPadCvi *>mCviSrcPad;
    map<int, TMSinkPadCvi *>mCviSinkPad;
    pthread_mutex_t mEncodeMutex;
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;
    virtual void InitDefaultPropertyList()
    {
        mDefaultPropertyList.Reset();
        mDefaultPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_GOP_NUMBER, 25);
        mDefaultPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_FPS, 25);
        mDefaultPropertyList.Assign(TMH264Encoder::PropID::OUTPUT_TARGET_BITRATE, 4*1024);   //目标码率  单位kb/s
    }
};

#endif // TM_CVI_H264_ENCODER_H
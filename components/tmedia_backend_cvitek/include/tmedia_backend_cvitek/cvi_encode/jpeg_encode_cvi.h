/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_CVI_JPEG_ENCODER_H
#define TM_CVI_JPEG_ENCODER_H

#include <tmedia_core/entity/codec/codec_inc.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include "cvi_venc.h"

class TMJpegEncoderCVI : public TMJpegEncoder
{
public:
    TMJpegEncoderCVI();
    virtual ~TMJpegEncoderCVI();

    // TMFilterEntity interface
    TMSrcPad *GetSrcPad(int padID = 0) final override;
    TMSinkPad *GetSinkPad(int padID = 0) final override;
    // TMCodec interface
    int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) final override;
    int SetConfig(TMPropertyList &propertyList) final override;
    int GetConfig(TMPropertyList &propertyList) final override;
    int Start() final override;
    int Flush() final override;
    int Stop() final override;
    int Close() final override;

    // TMVideoEncoder interface
    int SendFrame(TMVideoFrame &frame, int timeout) final override;
    int RecvPacket(TMVideoPacket &pkt, int timeout) final override;

private:
    const int PIC_QUALITY_MAX = 99;
    const int PIC_QUALITY_MIN = 1;
    int mChannelID;
    int mEncodeRunningFlag;
    TMSrcPadCvi *mCviSrcPad;
    TMSinkPadCvi *mCviSinkPad;
    pthread_mutex_t mEncodeMutex;
    virtual void InitDefaultPropertyList()
    {
        mDefaultPropertyList.Reset();
        mDefaultPropertyList.Assign(TMJpegEncoder::PropID::ENCODE_QUALITY_FACTOR, 60);
    }
};

#endif // TM_CVI_JPEG_ENCODER_H
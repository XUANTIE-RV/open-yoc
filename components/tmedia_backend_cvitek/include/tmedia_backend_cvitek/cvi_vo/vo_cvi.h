/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_CVI_VIDEO_OUTPUT_H
#define TM_CVI_VIDEO_OUTPUT_H

#include <tmedia_core/entity/format/format_io.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"

class  TMVideoOutputCvi: public TMVideoOutput 
{
public:

    TMVideoOutputCvi();
    virtual ~TMVideoOutputCvi();

    // TMSinkEntity interface
    TMSinkPad *GetSinkPad(int padID = 0) final override;

    // TMVideoOutput interface
    int Open(string deviceName, TMPropertyList *propList = NULL) final override;
    int Close()                                                  final override;
    int SetConfig(TMPropertyList &propList)                      final override;
    int GetConfig(TMPropertyList &propList)                      final override;
    int Start()                                                  final override;
    int Stop()                                                   final override;
    int SendFrame(TMVideoFrame &frame, int timeout)              final override;

private:
    map<int, TMSinkPadCvi *>mCviSinkPad;
    int mDeviceID;
    int mLayerID;
    int mChannelID;

    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;
    virtual void InitDefaultPropertyList()
    {
        mDefaultPropertyList.Reset();
        mDefaultPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_X, 0);
        mDefaultPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_COORDINATE_Y, 0);
        mDefaultPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_WIDTH, 320);
        mDefaultPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_HEIGHT, 480);
        mDefaultPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_PIXEL_FORMAT, (int32_t)(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
        mDefaultPropertyList.Assign(TMVideoOutput::PropID::VIDEO_OUT_FRAME_RATE, 60);
    }
};


#endif
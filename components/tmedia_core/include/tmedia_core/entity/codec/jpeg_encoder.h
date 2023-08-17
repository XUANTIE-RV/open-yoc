/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_JPEG_ENCODER_H
#define TM_JPEG_ENCODER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/media_info.h>
#include <tmedia_core/common/image_info.h>
#include <tmedia_core/entity/codec/codec.h>
#include <tmedia_core/common/packet.h>
#include <tmedia_core/common/frame.h>

using namespace std;

typedef struct {
    TMImageInfo::ImageRect_t rect;
    TMImageInfo::ImageRotation rotation;
} JpegEncodeConfig_t;

class TMJpegEncoder : public TMVideoEncoder
{
public:
    enum class PropID : uint32_t
    {
        // ID                      Data Type
        ENCODE_QUALITY_FACTOR,    // int, [1, 99], 99 is the best
        CROP_ENABLE,              // bool,    crop from origin frame, default is disabled
        CROP_RECT_X,              // int32_t, 0 ~ Origin picture width-1.  default 0.
        CROP_RECT_Y,              // int32_t, 0 ~ Origin picture height-1. default 0.
        CROP_RECT_W,              // int32_t, 0 ~ Origin picture width.    default -1 means input's width
        CROP_RECT_H,              // int32_t, 0 ~ Origin picture height.   default -1 menas input's height
        ROTATE_ANGLE,             // uint32_t, degree, 0/90/180/270
    };

    TMJpegEncoder() {InitDefaultPropertyList();}
    virtual ~TMJpegEncoder() {}

    // TMCodec interface
    virtual int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propertyList) = 0;
    virtual int GetConfig(TMPropertyList &propertyList) = 0;

    // TMVideoEncoder interface
    virtual int SendFrame(TMVideoFrame &frame, int timeout) = 0;
    virtual int RecvPacket(TMVideoPacket &pkt, int timeout) = 0;
    virtual int EncodeBatchProcess(TMVideoFrame &frame, JpegEncodeConfig_t *dst_config, int count, TMVideoPacket **pkt, int timeout) { return TMResult::TM_NOT_SUPPORT; }

protected:
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::ENCODE_QUALITY_FACTOR, 99, "quality_factor"));
            pList[i]->Add(TMProperty((int)PropID::CROP_ENABLE,      false, "crop enable"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_X,      0,     "crop_x"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_Y,      0,     "crop_y"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_W,      0,     "crop_w"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_H,      0,     "crop_h"));
            pList[i]->Add(TMProperty((int)PropID::ROTATE_ANGLE,     0,     "rotate_angle"));
        }
    }
};

#endif  /* TM_JPEG_ENCODER_H */

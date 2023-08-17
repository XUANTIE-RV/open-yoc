/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_JPEG_DECODER_H
#define TM_JPEG_DECODER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/media_info.h>

#include <tmedia_core/entity/codec/codec.h>
#include <tmedia_core/common/packet.h>
#include <tmedia_core/common/frame.h>

using namespace std;

class TMJpegDecoder : public TMVideoDecoder
{
public:
    enum class PropID : uint32_t
    {
        // ID                      Data Type
        FRAME_POOL_INIT_COUNT,  // int, <=0 means use platform default count

        CROP_ENABLE,              // bool,    crop from origin frame, default is disabled
        CROP_RECT_X,              // int32_t, 0 ~ Origin picture width-1.  default 0.
        CROP_RECT_Y,              // int32_t, 0 ~ Origin picture height-1. default 0.
        CROP_RECT_W,              // int32_t, 0 ~ Origin picture width.    default -1 means input's width
        CROP_RECT_H,              // int32_t, 0 ~ Origin picture height.   default -1 menas input's height

        // scale property
        SCALE_ENABLE,           // bool,     scale from origin frame, default is disabled
        SCALE_RECT_W,           // int32_t, 0 ~ Origin picture width    default -1 means input's width
        SCALE_RECT_H,           // int32_t, 0 ~ Origin picture height   default -1 menas input's height
    };

    TMJpegDecoder() 
    {
        mCodecID = TMMediaInfo::CodecID::JPEG;
        mMediaType = TMMediaInfo::Type::VIDEO;
        mWorkMode = TMMediaInfo::WorkMode::DECODE;
        InitDefaultPropertyList();
    }

    virtual ~TMJpegDecoder() {}

    // TMCodec interface
    virtual int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Flush() = 0;
    virtual int Stop() = 0;
    virtual int Close() = 0;

    // TMVideoDecoder interface
    virtual int SendPacket(TMPacket &pkt, int timeout) = 0;
    virtual int RecvFrame(TMVideoFrame &frame, int timeout) = 0;

protected:
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::FRAME_POOL_INIT_COUNT, -1, "frame pool init count"));
            pList[i]->Add(TMProperty((int)PropID::CROP_ENABLE, false, "crop enable"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_X, -1, "crop_x"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_Y, -1, "crop_y"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_W, -1, "crop _w"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_H, -1, "crop_h"));

            pList[i]->Add(TMProperty((int)PropID::SCALE_ENABLE, false, "scale enable"));
            pList[i]->Add(TMProperty((int)PropID::SCALE_RECT_W, -1, "scale_w"));
            pList[i]->Add(TMProperty((int)PropID::SCALE_RECT_H, -1, "scale__h"));
        }
    }
};

#endif  /* TM_JPEG_DECODER_H */

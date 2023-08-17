/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#ifndef TM_H264_ENCODER_H
#define TM_H264_ENCODER_H

#include <tmedia_core/entity/codec/encoder.h>

class TMH264Encoder : public TMVideoEncoder
{
public:
    enum class Profile : int32_t
    {
        UNKNOWN = -1,
        BASELINE,
        MAIN,
        EXTENDED,
        HIGH,
        HIGH_10,
        HIGH_422,
        HIGH_444,
    };

    enum class Level : int32_t
    {
        // Level                 ssuggest bitrete
        LEVEL_UNKNOWN = -99,
        LEVEL_1       = 10,   /* QCIF  (176x144)   64k bps */
        LEVEL_1_1     = 11,
        LEVEL_1_2     = 12,
        LEVEL_1_3     = 13,
        LEVEL_2       = 20,   /* CIF   (352x288)   576k bps" */
        LEVEL_2_1     = 21,
        LEVEL_2_2     = 22,
        LEVEL_3       = 30,   /* SD    (720x576)   2000k bps */
        LEVEL_3_1     = 31,
        LEVEL_3_2     = 32,
        LEVEL_4       = 40,   /* 1080p (1920x1080) 8500k bps */
        LEVEL_4_1     = 41,
        LEVEL_4_2     = 42,
        LEVEL_5       = 50,   /* 2k    (2560x1920) 168M bps */
        LEVEL_5_1     = 51,
        LEVEL_5_2     = 52,
    };

    enum class PropID : uint32_t
    {
        // ID                      Data Type
        PROFILE,                  // int, TMH264Encoder::Profile
        LEVEL,                    // int, TMH264Encoder::Level
        PICTURE_TYPE,             // uint32_t, bitmask of TMMediaInfo::PictureType
        OUTPUT_GOP_NUMBER,        // int, >=1
        OUTPUT_TARGET_BITRATE,    // int, <=0 means default, unit: kbps
        OUTPUT_RATE_CONTROL_MODE, // int, TMVideoEncoder::RateControlMode
        OUTPUT_FPS,               // int, <=0 means flow input frame rate
        CROP_ENABLE,              // bool,    crop from origin frame, default is disabled
        CROP_RECT_X,              // int32_t, 0 ~ Origin picture width-1.  default 0.
        CROP_RECT_Y,              // int32_t, 0 ~ Origin picture height-1. default 0.
        CROP_RECT_W,              // int32_t, 0 ~ Origin picture width.    default -1 means input's width
        CROP_RECT_H,              // int32_t, 0 ~ Origin picture height.   default -1 menas input's height
        ROTATE_ANGLE,             // uint32_t, degree, 0/90/180/270
    };

    TMH264Encoder()
    {
        mCodecID = TMMediaInfo::CodecID::H264;
        mMediaType = TMMediaInfo::Type::VIDEO;
        mWorkMode = TMMediaInfo::WorkMode::ENCODE;
        InitDefaultPropertyList();
    }
    virtual ~TMH264Encoder() {}

    // TMCodec interface
    virtual int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propertyList) = 0;
    virtual int GetConfig(TMPropertyList &propertyList) = 0;
    virtual int Start()                                 = 0;
    virtual int Flush()                                 = 0;
    virtual int Stop()                                  = 0;
    virtual int Close()                                 = 0;

    // TMVideoEncoder interface
    virtual int SendFrame(TMVideoFrame &frame, int timeout) = 0;
    virtual int RecvPacket(TMVideoPacket &pkt, int timeout) = 0;

protected:
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::PROFILE, (int)TMH264Encoder::Profile::HIGH, "profile"));
            pList[i]->Add(TMProperty((int)PropID::LEVEL, (int)TMH264Encoder::Level::LEVEL_5, "level"));
            pList[i]->Add(TMProperty((int)PropID::PICTURE_TYPE,
                          (uint32_t)TMMediaInfo::PictureType::I |
                          (uint32_t)TMMediaInfo::PictureType::P, "picture type"));
            pList[i]->Add(TMProperty((int)PropID::OUTPUT_GOP_NUMBER, 25, "group of picture"));
            pList[i]->Add(TMProperty((int)PropID::OUTPUT_TARGET_BITRATE, 8000, "bitrate"));
            pList[i]->Add(TMProperty((int)PropID::OUTPUT_RATE_CONTROL_MODE, (int)TMVideoEncoder::RateControlMode::CBR, "rate control mode"));
            pList[i]->Add(TMProperty((int)PropID::OUTPUT_FPS, 25, "frame per second"));
            pList[i]->Add(TMProperty((int)PropID::CROP_ENABLE,      false, "crop enable"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_X,      0,     "crop_x"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_Y,      0,     "crop_y"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_W,      0,     "crop_w"));
            pList[i]->Add(TMProperty((int)PropID::CROP_RECT_H,      0,     "crop_h"));
            pList[i]->Add(TMProperty((int)PropID::ROTATE_ANGLE,     0,     "rotate_angle"));
        }
    }
};

#endif /* TM_H264_ENCODER_H */

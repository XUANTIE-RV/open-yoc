/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#ifndef TM_H265_ENCODER_H
#define TM_H265_ENCODER_H

#include <tmedia_core/entity/codec/encoder.h>

class TMH265Encoder : public TMVideoEncoder
{
public:
    enum class Profile : int32_t
    {
        UNKNOWN = -1,
        MAIN,
        MAIN_10,
        MAIN_STILL,
    };

    enum class Tier : int32_t
    {
        UNKNOWN = -1,
        MAIN,
        HIGH,
    };

    enum class Level : int32_t
    {
        // Level           
        LEVEL_UNKNOWN = -99,
        LEVEL_1       = 10,
        LEVEL_2       = 20,
        LEVEL_2_1     = 21,
        LEVEL_3       = 30,
        LEVEL_3_1     = 31,
        LEVEL_4       = 40,
        LEVEL_4_1     = 41,
        LEVEL_5       = 50,
        LEVEL_5_1     = 51,
        LEVEL_5_2     = 52,
        LEVEL_6       = 60,
        LEVEL_6_1     = 61,
        LEVEL_6_2     = 62,
    };

    enum class PropID : uint32_t
    {
        // ID                      Data Type
        PROFILE,                  // int, TMH265Encoder::Profile
        TIER,                     // int, TMH265Encoder::Tier
        LEVEL,                    // int, TMH265Encoder::Level
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

    TMH265Encoder()
    {
        mCodecID = TMMediaInfo::CodecID::H265;
        mMediaType = TMMediaInfo::Type::VIDEO;
        mWorkMode = TMMediaInfo::WorkMode::ENCODE;
        InitDefaultPropertyList();
    }
    virtual ~TMH265Encoder() {}

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

    virtual int GetDefaultBitrate(int width, int height, Tier tier)
    {
        int pix_num = width*height;
        int target_bitrate;
        double tier_factor;

        if(width <=0 || height <= 0 || width*height > 7680*4320 || tier == Tier::UNKNOWN || tier > Tier::HIGH)
        {
            return -1;
        }

        if(tier < Tier::HIGH)
        {
            tier_factor = 1.0f;
        }
        else 
        {
            tier_factor = 3.0f;
        }

        if(pix_num <= 176*144)  //QCIF
        {
            target_bitrate = 64;
        }
        else if(pix_num <= 352*288) //CIF
        {
            target_bitrate = 576;
        }
        else if(pix_num <= 720*576) //SD
        {
            target_bitrate = 2000;
        }
        else if(pix_num <= 1280*720) //HD
        {
            target_bitrate = 2500;
        }
        else if(pix_num <= 1920*1080) //1080p
        {
            target_bitrate = 6000;
        }
        else if(pix_num <= 2560*1920) //2K
        {
            target_bitrate = 12000;
        }
        else if(pix_num <= 3840*2160) //4K
        {
            target_bitrate = 18000;
        }
        else if(pix_num <= 7680*4320) //8K
        {
            target_bitrate = 60000;
        }
        else 
        {
            return -1;
        }
        target_bitrate *= tier_factor;
        return target_bitrate;
    }

protected:
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::PROFILE, (int)TMH265Encoder::Profile::MAIN, "profile"));
            pList[i]->Add(TMProperty((int)PropID::TIER, (int)TMH265Encoder::Tier::MAIN, "tier"));
            pList[i]->Add(TMProperty((int)PropID::LEVEL, (int)TMH265Encoder::Level::LEVEL_5, "level"));
            pList[i]->Add(TMProperty((int)PropID::PICTURE_TYPE,
                          (uint32_t)TMMediaInfo::PictureType::I |
                          (uint32_t)TMMediaInfo::PictureType::P, "picture type"));
            pList[i]->Add(TMProperty((int)PropID::OUTPUT_GOP_NUMBER, 25, "group of picture"));
            pList[i]->Add(TMProperty((int)PropID::OUTPUT_TARGET_BITRATE, -1, "bitrate"));
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

#endif /* TM_H265_ENCODER_H */

/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef TM_CODEC_PARAMS_H
#define TM_CODEC_PARAMS_H

#include <string>
#include <tmedia_core/common/common_inc.h>

using namespace std;

class TMCodecParams
{
public:

    TMMediaInfo::Type     mCodecType;
    TMMediaInfo::WorkMode mWorkMode;
    TMMediaInfo::CodecID  mCodecID;
    void                 *mBackendPriv;

    TMCodecParams();
    virtual void Reset() = 0;
    virtual void Dump() = 0;
};

class TMVideoDecoderParams : public TMCodecParams
{
public:
    int32_t mProfile;
    int32_t mLevel;
    int32_t mWidth;
    int32_t mHeight;
    bool    mHasBFrame; // TODO: Needed?
    int64_t mBitRate;

    uint8_t *mExtData;
    int32_t  mExtDataSize;

    TMVideoDecoderParams();
    virtual void Reset();
    virtual void Dump();
};

class TMVideoEncoderParams : public TMCodecParams
{
public:
    int32_t mWidth;
    int32_t mHeight;
    TMImageInfo::PixelFormat mPixelFormat;

    TMVideoEncoderParams();
    virtual void Reset();
    virtual void Dump();
};

class TMJpegDecoderParams : public TMCodecParams
{
public:
    int32_t mOutputWidth;   // -1 means origin width
    int32_t mOutputHeight;  // -1 means origin height
    TMImageInfo::PixelFormat mOutputPixelFormat;

    TMJpegDecoderParams();
    virtual void Reset();
    virtual void Dump();
};

class TMJpegEncoderParams : public TMCodecParams
{
public:
    int32_t mWidth;               // input width
    int32_t mHeight;              // input height
    TMImageInfo::PixelFormat mPixelFormat; // input pixel format

    TMJpegEncoderParams();
    virtual void Reset();
    virtual void Dump();
};

class TMAudioDecoderParams : public TMCodecParams
{
public:
    int32_t mChannels;
    int32_t mBitsPerCodedSample;

    TMAudioDecoderParams();
    virtual void Reset();
    virtual void Dump();
};

class TMAudioEncoderParams : public TMCodecParams
{
public:
    int32_t mChannels;
    int32_t mBitPerRawSample;

    TMAudioEncoderParams();
    virtual void Reset();
    virtual void Dump();
};

#endif  /* TM_CODEC_PARAMS_H */

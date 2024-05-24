/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/codec/codec_params.h>

using namespace std;

////////////////////////////////
// class TMCodecParams        //
////////////////////////////////

TMCodecParams::TMCodecParams()
    : mCodecType(TMMediaInfo::Type::UNKNOWN),
      mCodecID(TMMediaInfo::CodecID::UNKNOWN)
{
}

void TMCodecParams::Reset()
{
    mCodecType = TMMediaInfo::Type::UNKNOWN;
    mWorkMode  = TMMediaInfo::WorkMode::UNKNOWN;
    mCodecID   = TMMediaInfo::CodecID::UNKNOWN;
}

void TMCodecParams::Dump()
{
    cout << "TMCodecParams info dump:" << endl;
    cout << "  mCodecType:" << TMMediaInfo::Name(mCodecType) << endl;
    cout << "  mWorkMode :" << TMMediaInfo::Name(mWorkMode) << endl;
    cout << "  mCodecID  :" << TMMediaInfo::Name(mCodecID) << endl;
}

////////////////////////////////
// class TMVideoDecoderParams //
////////////////////////////////

TMVideoDecoderParams::TMVideoDecoderParams()
    : mExtData(NULL)
{
    Reset();
}

void TMVideoDecoderParams::Reset()
{
    this->TMCodecParams::Reset();

    // TMCodecParams members
    mCodecType   = TMMediaInfo::Type::VIDEO;
    mWorkMode    = TMMediaInfo::WorkMode::DECODE;
    mCodecID     = TMMediaInfo::CodecID::UNKNOWN;
    mBackendPriv = NULL;

    mWidth = 0;
    mHeight = 0;
    mHasBFrame = 0;

    mBitRate = 0;
    mProfile = 0;
    mLevel = 0;

    if (mExtData != NULL && mExtDataSize > 0)
    {
        free(mExtData);
    }
    mExtData = NULL;
    mExtDataSize = 0;
}

void TMVideoDecoderParams::Dump()
{
    cout << "====Dump TMVideoDecoderParams begin====" << endl;
    this->TMCodecParams::Dump();
    cout << "TMVideoDecoderParams info dump:" << endl;
    cout << "  mBitRate    :" << mBitRate << endl;
    cout << "  mProfile    :" << mProfile << endl;
    cout << "  mLevel      :" << mLevel << endl;
    cout << "  mWidth      :" << mWidth << endl;
    cout << "  mHeight     :" << mHeight << endl;
    cout << "  mHasBFrame  :" << mHasBFrame << endl;
    cout << "  mExtData    :" << hex << static_cast<const void *>(mExtData) << dec << endl;
    cout << "  mExtDataSize:" << mExtDataSize << endl;
    cout << "  mBackendPriv:" << mBackendPriv << endl;
    cout << "====Dump TMVideoDecoderParams end  ====" << endl;
}

////////////////////////////////
// class TMJpegDecoderParams //
////////////////////////////////

TMJpegDecoderParams::TMJpegDecoderParams()
{
    Reset();
}

void TMJpegDecoderParams::Reset()
{
    this->TMCodecParams::Reset();

    // TMCodecParams members
    mCodecType = TMMediaInfo::Type::VIDEO;
    mWorkMode  = TMMediaInfo::WorkMode::DECODE;
    mCodecID = TMMediaInfo::CodecID::JPEG;
    mBackendPriv = NULL;

    // TMJpegDecoderParams members
    mOutputPixelFormat = TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12;
}

void TMJpegDecoderParams::Dump()
{
    this->TMCodecParams::Dump();
    cout << "TMJpegDecoderParams info dump:" << endl;
    cout << "  mOutputWidth:" << mOutputWidth << endl;
    cout << "  mOutputHeight:" << mOutputHeight << endl;
    cout << "  mOutputPixelFormat:" << TMImageInfo::Name(mOutputPixelFormat) << endl;
}

////////////////////////////////
// class TMVideoEncoderParams //
////////////////////////////////
TMVideoEncoderParams::TMVideoEncoderParams()
{
    Reset();
}

void TMVideoEncoderParams::Reset()
{
    this->TMCodecParams::Reset();

    // TMCodecParams members
    mCodecType   = TMMediaInfo::Type::VIDEO;
    mWorkMode    = TMMediaInfo::WorkMode::ENCODE;
    mCodecID     = TMMediaInfo::CodecID::UNKNOWN;
    mBackendPriv = NULL;

    mPixelFormat = TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW;
    mWidth = 0;
    mHeight = 0;
}

void TMVideoEncoderParams::Dump()
{
    this->TMCodecParams::Dump();
    cout << "TMVideoEncoderParams info dump:" << endl;
    cout << "        mWidth:" << mWidth << endl;
    cout << "       mHeight:" << mHeight << endl;
    cout << "  mPixelFormat:" << TMImageInfo::Name(mPixelFormat) << endl;
}

////////////////////////////////
// class TMJpegEncoderParams //
////////////////////////////////
TMJpegEncoderParams::TMJpegEncoderParams()
{
    Reset();
    mCodecID = TMMediaInfo::CodecID::JPEG;
}

void TMJpegEncoderParams::Reset()
{
    this->TMCodecParams::Reset();

    // TMCodecParams members
    mCodecType   = TMMediaInfo::Type::VIDEO;
    mWorkMode    = TMMediaInfo::WorkMode::ENCODE;
    mCodecID     = TMMediaInfo::CodecID::UNKNOWN;
    mBackendPriv = NULL;

    mPixelFormat = TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW;
    mWidth = 0;
    mHeight = 0;
}

void TMJpegEncoderParams::Dump()
{
    this->TMCodecParams::Dump();
    cout << "TMVideoDecoderParams info dump:" << endl;
    cout << "  mWidth:" << mWidth << endl;
    cout << "  mHeight:" << mHeight << endl;
}

////////////////////////////////
// class TMAudioDecoderParams //
////////////////////////////////

TMAudioDecoderParams::TMAudioDecoderParams()
{
    Reset();
}

void TMAudioDecoderParams::Reset()
{
    this->TMCodecParams::Reset();

    // TMCodecParams members
    mCodecType   = TMMediaInfo::Type::AUDIO;
    mWorkMode    = TMMediaInfo::WorkMode::DECODE;
    mCodecID     = TMMediaInfo::CodecID::UNKNOWN;
    mBackendPriv = NULL;

    mBitsPerCodedSample = 0;
    mChannels = 0;
}

void TMAudioDecoderParams::Dump()
{
    this->TMCodecParams::Dump();
    cout << "TMAudioDecoderParams dump:" << endl;
}

////////////////////////////////
// class TMAudioEncoderParams //
////////////////////////////////
TMAudioEncoderParams::TMAudioEncoderParams()
{
    Reset();
}

void TMAudioEncoderParams::Reset()
{
    this->TMCodecParams::Reset();

    // TMCodecParams members
    mCodecType   = TMMediaInfo::Type::AUDIO;
    mWorkMode    = TMMediaInfo::WorkMode::ENCODE;
    mCodecID     = TMMediaInfo::CodecID::UNKNOWN;
    mBackendPriv = NULL;

    mBitPerRawSample = 0;
    mChannels = 0;
}

void TMAudioEncoderParams::Dump()
{
    this->TMCodecParams::Dump();
    cout << "TMAudioEncoderParams dump:" << endl;
}


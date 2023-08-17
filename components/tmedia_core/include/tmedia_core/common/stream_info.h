/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_STREAM_INFO_H
#define TM_STREAM_INFO_H

#include <string>

#include <tmedia_core/common/common_inc.h>

using namespace std;

#ifndef MPEG_TIME_BASE
#define MPEG_TIME_BASE (90000)
#endif

typedef enum
{
    BSF_UNKNOWN = -1,
    BSF_ANNEXB,         // H264/H265 Elementary Stream format, match most hardware decoder (Default)
    BSF_AVCC,           // AVC (H264) avpacket, called Byte-Stream format
    BSF_HVCC,           // HEVC(h265) avpacket, called Byte-Stream format
} ByteStreamFormat_e;

typedef struct
{
    TMMediaInfo::CodecID mCodecID;
    int                  mProfile;
    int                  mLevel;
    int64_t              mBitRate;
    int                  mWidth;
    int                  mHeight;
} TMStreamVideoInfo_t;

typedef struct
{
    TMMediaInfo::CodecID mCodecID;
} TMStreamAduioInfo_t;

typedef struct
{
} TMStreamDataInfo_t;


class TMStreamInfo
{
public:
    int32_t mStreamID;
    TMMediaInfo::Type mMediaType;
    union
    {
        TMStreamVideoInfo_t mVideoInfo;
        TMStreamAduioInfo_t mAudioInfo;
        TMStreamDataInfo_t  mDataInfo;
    };
};

#endif  /* TM_STREAM_INFO_H */

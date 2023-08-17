/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef TM_MEDIA_INFO_H
#define TM_MEDIA_INFO_H

#include <string>
#include <tmedia_core/common/image_info.h>

using namespace std;

class TMMediaInfo
{
public:
    enum class Type
    {
        UNKNOWN = 0,
        VIDEO,
        AUDIO,
        DATA
    };

    enum class WorkMode
    {
        UNKNOWN = 0,
        DECODE,
        ENCODE,
        DEMUX,
        MUX,
    };

    enum class CodecID
    {
        UNKNOWN = 0,
        H264,
        H265,
        MPEG4,
        JPEG,
        VP9,

        AAC,
        MP3,
        G711,
    };

    enum class FormatID
    {
        UNKNOWN = -1,
        AVI,
        MP4,
        TS,
        JPEG,

        RTSP,

        CAMERA,

        PCM,
    };

    enum class DeviceID
    {
        UNKNOWN = -1,
        CAMERA,
        SCREEN,
        MIC,
        SPEAKER,
    };

    enum class MediaFileType
    {
        SIMPLE = 0,
        T_HEAD,
    };

    enum class PictureType : uint32_t
    {
        UNKNOWN = 0,
        I = 1 << 0,
        P = 1 << 1,
        B = 1 << 2,
    };

    static const string Name(TMMediaInfo::Type          type);
    static const string Name(TMMediaInfo::WorkMode      workMode);
    static const string Name(TMMediaInfo::CodecID       codecID);
    static const string Name(TMMediaInfo::FormatID      formatID);
    static const string Name(TMMediaInfo::DeviceID      deviceID);
    static const string Name(TMMediaInfo::MediaFileType FileType);
    static const string Name(TMMediaInfo::PictureType   pictureType);
};

#endif  /* TM_MEDIA_INFO_H */

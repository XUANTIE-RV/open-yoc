/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/common/common_inc.h>

using namespace std;

const string TMMediaInfo::Name(TMMediaInfo::Type type)
{
    switch (type)
    {
    case TMMediaInfo::Type::VIDEO:
        return "VIDEO";
    case TMMediaInfo::Type::AUDIO:
        return "AUDIO";
    case TMMediaInfo::Type::DATA:
        return "DATA";
    default:
        return "UNKNOWN";
    }
}

const string TMMediaInfo::Name(TMMediaInfo::WorkMode workMode)
{
    switch (workMode)
    {
    case TMMediaInfo::WorkMode::DECODE:
        return "DECODE";
    case TMMediaInfo::WorkMode::ENCODE:
        return "ENCODE";
    case TMMediaInfo::WorkMode::DEMUX:
        return "DEMUX";
    case TMMediaInfo::WorkMode::MUX:
        return "MUX";
    default:
        return "UNKNOWN";
    }
}

const string TMMediaInfo::Name(TMMediaInfo::CodecID codecID)
{
    switch (codecID)
    {
    case TMMediaInfo::CodecID::H264:
        return "H264";
    case TMMediaInfo::CodecID::H265:
        return "H265";
    case TMMediaInfo::CodecID::VP9:
        return "VP9";
    case TMMediaInfo::CodecID::MPEG4:
        return "MPEG4";
    case TMMediaInfo::CodecID::JPEG:
        return "JPEG";
    case TMMediaInfo::CodecID::AAC:
        return "AAC";
    case TMMediaInfo::CodecID::MP3:
        return "MP3";
    default:
        return "UNKNOWN";
    }
}

const string TMMediaInfo::Name(TMMediaInfo::FormatID formatID)
{
    switch (formatID)
    {
        case TMMediaInfo::FormatID::AVI:
            return "AVI";
        case TMMediaInfo::FormatID::MP4:
            return "MP4";
        case TMMediaInfo::FormatID::TS:
            return "TS";
        case TMMediaInfo::FormatID::RTSP:
            return "RTSP";
        case TMMediaInfo::FormatID::CAMERA:
            return "CAMERA";
        default:
            return "UNKNOWN";
    }
}

const string TMMediaInfo::Name(TMMediaInfo::DeviceID deviceID)
{
    switch (deviceID)
    {
        case TMMediaInfo::DeviceID::CAMERA:
            return "CAMERA";
        case TMMediaInfo::DeviceID::SCREEN:
            return "SCREEN";
        case TMMediaInfo::DeviceID::MIC:
            return "MIC";
        case TMMediaInfo::DeviceID::SPEAKER:
            return "SPEAKER";
        default:
            return "UNKNOWN";
    }
}

const string TMMediaInfo::Name(TMMediaInfo::MediaFileType FileType)
{
    switch (FileType) 
    {
        case TMMediaInfo::MediaFileType::SIMPLE:
            return "FILETYPE_SIMPLE";
        case TMMediaInfo::MediaFileType::T_HEAD:
            return "FILETYPE_T_HEAD";
        default:
            return "UNKNOWN";

    }
}

const string TMMediaInfo::Name(TMMediaInfo::PictureType pictureType)
{
    switch (pictureType)
    {
        case TMMediaInfo::PictureType::I:
            return "I";
        case TMMediaInfo::PictureType::P:
            return "P";
        case TMMediaInfo::PictureType::B:
            return "B";
        default:
            return "UNKNOWN";
    }
}

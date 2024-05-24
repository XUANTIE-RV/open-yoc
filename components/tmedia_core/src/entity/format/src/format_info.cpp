/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/entity/format/format_info.h>

using namespace std;

TMFormatInfo::~TMFormatInfo()
{
    Reset();
}

TMFormatInfo& TMFormatInfo::operator = (const TMFormatInfo & src)
{
    if (this == &src)
    {
        return *this;
    }
    this->Reset();

    this->mFileName = src.mFileName;
    this->mFormatID = src.mFormatID;
    this->mFileType = src.mFileType;
    this->mStartTime = src.mStartTime;
    this->mDuration = src.mDuration;
    this->mBitRate = src.mBitRate;

    this->mStreamCount = src.mStreamCount;
    for (auto iter = src.mStreamInfoMap.begin(); iter != src.mStreamInfoMap.end(); iter++)
    {
        TMStreamInfo *streamInfo = new TMStreamInfo;
        *streamInfo = *(iter->second);
        this->mStreamInfoMap.insert(TMStreamInfoMap_t::value_type(iter->first, streamInfo));
    }

    return *this;
}

void TMFormatInfo::Reset()
{
    mFileName = "";
    mFormatID = TMMediaInfo::FormatID::UNKNOWN;
    mFileType = "";
    mStartTime = 0;
    mDuration = 0;
    mBitRate = 0;
    mStreamCount = 0;

    auto iter = mStreamInfoMap.begin();
    while (iter != mStreamInfoMap.end())
    {
        if (iter->second != NULL)
        {
            delete(iter->second);
        }
        mStreamInfoMap.erase(iter++);
    }
}

void TMFormatInfo::Dump()
{
    cout << "====Dump FormatInfo begin====" << endl;
    cout << "  FileName   : " << mFileName << endl;
    cout << "  FileType   : " << mFileType << endl;
    cout << "  Format     : " << TMMediaInfo::Name(mFormatID) << endl;
    cout << "  StartTime  : " << mStartTime << endl;
    cout << "  Duration   : " << mDuration << endl;
    cout << "  BitRate    : " << mBitRate << endl;
    cout << "  StreamCount: " << mStreamCount << endl;

    for (uint32_t i = 0; i < mStreamCount; i++)
    {
        TMStreamInfo *streamInfo = mStreamInfoMap[i];
        cout << "  Stream[" << i << "] = {" << endl;
        cout << "    StreamID : " << streamInfo->mStreamID << endl;
        cout << "    MediaType: " << TMMediaInfo::Name(streamInfo->mMediaType) << endl;

        TMStreamVideoInfo_t *videoInfo;
        TMStreamAduioInfo_t *audioInfo;
        //TMStreamDataInfo_t  *dataInfo;

        switch (streamInfo->mMediaType)
        {
        case TMMediaInfo::Type::VIDEO:
            videoInfo = &(streamInfo->mVideoInfo);
            cout << "    mCodecID : " << TMMediaInfo::Name(videoInfo->mCodecID) << endl;
            break;
        case TMMediaInfo::Type::AUDIO:
            audioInfo = &(streamInfo->mAudioInfo);
            cout << "    mCodecID : " << TMMediaInfo::Name(audioInfo->mCodecID) << endl;
            break;
        case TMMediaInfo::Type::DATA:
            //dataInfo = &(streamInfo->mDataInfo);
            break;
        default:
            break;
        }
        cout << "  }" << endl;
    }
    cout << "====Dump FormatInfo end======" << endl;
}

int TMFormatInfo::GetStreamInfo(TMStreamInfo **vStreamInfo, TMMediaInfo::Type mediaType)
{
    if (vStreamInfo == NULL)
    {
        return TMResult::TM_EINVAL;
    }
    *vStreamInfo = NULL;

    for (uint32_t i = 0; i < mStreamCount; i++)
    {
        TMStreamInfo *streamInfo = mStreamInfoMap[i];
        if (mediaType == streamInfo->mMediaType)
        {
            *vStreamInfo = streamInfo;
            return 0;
        }
    }
    return TMResult::TM_ENOENT;
}

int TMFormatInfo::GetImageSize(TMImageInfo::ImageSize_t &imgSize)
{
    for (uint32_t i = 0; i < mStreamCount; i++)
    {
        TMStreamInfo *streamInfo = mStreamInfoMap[i];
        if (streamInfo->mMediaType == TMMediaInfo::Type::VIDEO)
        {
            imgSize.width = streamInfo->mVideoInfo.mWidth;
            imgSize.height = streamInfo->mVideoInfo.mHeight;
            return TMResult::TM_OK;
        }
    }

    imgSize.height = 0;
    imgSize.width = 0;
    return TMResult::TM_FORMAT_INVALID;
}


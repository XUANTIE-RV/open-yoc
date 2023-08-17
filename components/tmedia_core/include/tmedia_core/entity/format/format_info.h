/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_FORMAT_INFO_H
#define TM_FORMAT_INFO_H

#include <string>
#include <map>

#include <tmedia_core/common/common_inc.h>

using namespace std;

typedef map<int, TMStreamInfo*> TMStreamInfoMap_t;

class TMFormatInfo
{
public:

    string   mFileName;                 /* media file name */
    TMMediaInfo::FormatID mFormatID;    /* media file format */
    string   mFileType;                 /* media file type */
    int64_t  mStartTime;                /* unit: us */
    int64_t  mDuration;                 /* unit: us */
    uint32_t mBitRate;                  /* unit: bps */

    uint32_t mStreamCount;              /* total streams count */
    TMStreamInfoMap_t mStreamInfoMap;   /* Stores all streams in the map */

    ~TMFormatInfo();
    TMFormatInfo& operator=(const TMFormatInfo & src);
    void Reset();
    void Dump();
    int  GetStreamInfo(TMStreamInfo **vStreamInfo, TMMediaInfo::Type mediaType);
    int  GetImageSize(TMImageInfo::ImageSize_t &imgSize);
};

#endif  /* TM_FORMAT_INFO_H */

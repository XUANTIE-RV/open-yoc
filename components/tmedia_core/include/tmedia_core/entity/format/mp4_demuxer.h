/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_MP4_DEMUXER_H
#define TM_MP4_DEMUXER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_info.h>

using namespace std;

class TMMp4Demuxer : public TMFormatDemuxer
{
public:
    TMMp4Demuxer() {mFormatID = TMMediaInfo::FormatID::MP4; InitDefaultPropertyList();}
    virtual ~TMMp4Demuxer() {}

    // TMFormatDemuxer interface
    virtual int Open(string fileName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                = 0;
    virtual int GetFormatInfo(TMFormatInfo &fmtInfo)                   = 0;
    virtual int GetCodecParam(TMCodecParams &codecParams)              = 0;

    virtual int SetConfig(TMPropertyList &propertyList)                = 0;
    virtual int GetConfig(TMPropertyList &propertyList)                = 0;

    virtual int Start()                                                = 0;
    virtual int Stop()                                                 = 0;
    virtual int Seek(int64_t timestamp)                                = 0;
    virtual int ReadPacket(TMPacket &packet)                           = 0;

    enum class PropID : int
    {
        BYTESTREAM_FORMAT,   // Type: INT32, Value Range: ByteStreamFormat_e
    };

protected:
    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::BYTESTREAM_FORMAT, BSF_ANNEXB, "bytestream format"));
        }
    }
};

#endif  /* TM_MP4_DEMUXER_H */

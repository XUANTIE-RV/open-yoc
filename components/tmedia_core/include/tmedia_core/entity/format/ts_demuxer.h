/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#ifndef TM_TS_DEMUXER_H
#define TM_TS_DEMUXER_H

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_info.h>

using namespace std;

class TMTsDemuxer : public TMFormatDemuxer
{
public:
    TMTsDemuxer()
    {
        mFormatID = TMMediaInfo::FormatID::TS;
        InitDefaultPropertyList();
    }
    virtual ~TMTsDemuxer() {}

    // TMFormatDemuxer interface
    virtual int Open(string fileName = "", TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                = 0;
    virtual int GetFormatInfo(TMFormatInfo &fmtInfo)                   = 0;
    virtual int GetCodecParam(TMCodecParams &codecParams)              = 0;

    virtual int SetConfig(TMPropertyList &propertyList)                = 0;
    virtual int GetConfig(TMPropertyList &propertyList)                = 0;

    virtual int Start()                                                = 0;
    virtual int Stop()                                                 = 0;
    virtual int Seek(int64_t timestamp)                                = 0;
    virtual int ReadPacket(TMPacket &packet)                           = 0;
    virtual int SendPacket(TMPacket &pkt, int timeout)                 = 0;
    virtual int RecvPacket(TMPacket &pkt, int timeout)                 = 0;
    enum class PropID : int
    {
        TSDEMUXER_INBUF_SIZE,   // Type: UINT32, default=1M, to save the ts stream send in.
        TSDEMUXER_OUTBUF_SIZE,   // Type: UINT32, default=1M, to save the media stream which will be received.
    };

protected:
    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::TSDEMUXER_INBUF_SIZE, 1024 * 1024 * 1, "tsdemuxer inbuf size"));
            pList[i]->Add(TMProperty((int)PropID::TSDEMUXER_OUTBUF_SIZE, 1024 * 1024 * 1, "tsdemuxer outbuf size"));
        }
    }
};

#endif  /* TM_TS_DEMUXER_H */

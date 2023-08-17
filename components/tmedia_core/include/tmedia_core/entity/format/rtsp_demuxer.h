/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_RTSP_DEMUXER_H
#define TM_RTSP_DEMUXER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_info.h>

using namespace std;

class TMRtspDemuxer : public TMFormatDemuxer
{
public:
    TMRtspDemuxer() {mFormatID = TMMediaInfo::FormatID::RTSP; InitDefaultPropertyList();}
    virtual ~TMRtspDemuxer() {};

    // TMFormatDemuxer interface
    virtual int Open(string url, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                           = 0;

    virtual int GetFormatInfo(TMFormatInfo &fmtInfo)              = 0;
    virtual int GetCodecParam(TMCodecParams &codecParams)         = 0;

    virtual int SetConfig(TMPropertyList &propertyList)           = 0;
    virtual int GetConfig(TMPropertyList &propertyList)           = 0;

    virtual int Start()                                           = 0;
    virtual int Stop()                                            = 0;
    virtual int Seek(int64_t timestamp)                           = 0;
    virtual int ReadPacket(TMPacket &packet)                      = 0;

    enum class PropID : int {
        BUF_SIZE,
        INIT_PAUSE,
        TRANSPORT,
        TIMEOUT,
        MAXDELAY,
    };

protected:
    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::BUF_SIZE,   1*1024*1024, "bufsize"));
            pList[i]->Add(TMProperty((int)PropID::INIT_PAUSE, false,       "initial_pause"));
            pList[i]->Add(TMProperty((int)PropID::TRANSPORT,  "tcp",       "transport"));
            pList[i]->Add(TMProperty((int)PropID::TIMEOUT,    3000,        "timeout"));
            pList[i]->Add(TMProperty((int)PropID::MAXDELAY,   1000,        "maxdelay"));
        }
    }
};

#endif  /* TM_RTSP_DEMUXER_H */

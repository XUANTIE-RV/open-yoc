/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef TM_TS_MUXER_H
#define TM_TS_MUXER_H

#include <tmedia_core/common/media_info.h>
#include <tmedia_core/entity/parser/parser.h>
#include <tmedia_core/entity/format/format_muxer.h>

using namespace std;

class TMTsMuxer : public TMFormatMuxer
{
public:
    TMTsMuxer()
    {
        mFormatID = TMMediaInfo::FormatID::TS;
        InitDefaultPropertyList();
    }
    virtual ~TMTsMuxer() {}

    virtual int Open(TMPropertyList *propList = NULL)           = 0;
    virtual int Close()                                         = 0;

    virtual int SetConfig(TMPropertyList &propertyList)         = 0;
    virtual int GetConfig(TMPropertyList &propertyList)         = 0;

    virtual int Start()                                         = 0;
    virtual int Stop()                                          = 0;
    virtual int SendPacket(TMPacket &pkt, int timeout)          = 0;
    virtual int RecvPacket(TMPacket &pkt, int timeout)          = 0;
    enum class PropID : int
    {
        TSMUXER_BUF_SIZE,   // Type: UINT32, default=1M, to save the muxered ts stream.
    };


protected:
    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::TSMUXER_BUF_SIZE, 1024 * 1024 * 1, "tsmuxer buf size"));
        }
    }
};

#endif /* TM_TS_MUXER_H */

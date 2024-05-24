/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_RTSP_MUXER_H
#define TM_RTSP_MUXER_H

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_muxer.h>
#include <tmedia_core/entity/format/format_info.h>

using namespace std;

class TMRtspMuxer : public TMFormatMuxer
{
  public:
    TMRtspMuxer()
    {
        mFormatID = TMMediaInfo::FormatID::RTSP;
        InitDefaultPropertyList();
    };
    virtual ~TMRtspMuxer(){};

    virtual int Open(TMPropertyList *propList = NULL) = 0;
    virtual int Close() = 0;

    virtual int SetConfig(TMPropertyList &propertyList) = 0;
    virtual int GetConfig(TMPropertyList &propertyList) = 0;

    virtual int Start() = 0;
    virtual int Stop() = 0;
    virtual int SendPacket(TMPacket &pkt, int timeout) = 0;
    virtual int RecvPacket(TMPacket &pkt, int timeout) = 0;

    enum class PropID : int
    {
        TRANSPORT,
        PORT,
        PATH,
        URL_PATH,
        FPS,
        WIDTH,
        HEIGHT,
        CODEC_ID,
    };

  protected:
    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::TRANSPORT, "udp", "transport"));
            pList[i]->Add(TMProperty((int)PropID::PORT, 8554, "rtsp port"));
            pList[i]->Add(TMProperty((int)PropID::PATH, "stream0", "rtsp path"));
            pList[i]->Add(TMProperty((int)PropID::URL_PATH, "rtsp://", "rtsp url"));
            pList[i]->Add(TMProperty((int)PropID::FPS, 30, "fps"));
            pList[i]->Add(TMProperty((int)PropID::WIDTH, 1920, "width"));
            pList[i]->Add(TMProperty((int)PropID::HEIGHT, 1080, "height"));
            pList[i]->Add(TMProperty((int)PropID::CODEC_ID, "H264", "codec id"));
        }
    }
};

#endif /* TM_RTSP_MUXER_H */

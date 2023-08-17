/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef TM_RTSP_MUXER_H
#define TM_RTSP_MUXER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_info.h>

using namespace std;

typedef struct {
    string    url;
    AVCodecID audioCodecID;
    AVCodecID videoCodecID;
} TMRtspMuxerConfig_s;

class TMRtspMuxer : public TMFormatMuxer
{
public:
    TMRtspDemuxer();
    ~TMRtspDemuxer();

    virtual int  Open(TMRtspMuxerConfig_s& config);
    virtual void Close();

    virtual int  WriteHeader();
    virtual void WritePacket(TMPacket &packet);
};

#endif  /* TM_RTSP_MUXER_H */

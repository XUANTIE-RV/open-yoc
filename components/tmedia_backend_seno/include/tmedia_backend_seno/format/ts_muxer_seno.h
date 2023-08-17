/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#ifndef TM_TS_MUXER_SENO_H
#define TM_TS_MUXER_SENO_H

#define LOG_LEVEL 1
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/entity/format/format_factory.h>
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_muxer.h>
#include <tmedia_core/entity/format/format_info.h>
#include <tmedia_core/common/media_info.h>
#include <tmedia_core/entity/parser/parser.h>
#include <tmedia_core/util/util_ringbuffer.h>
#include <tmedia_core/entity/format/ts_muxer.h>
#include <mpegts/ts_muxer.hpp>
#include <mpegts/ts_frame.hpp>
#include <unistd.h>

using namespace std;

class TMTsMuxerSeno final: public TMTsMuxer
{
public:
    TMTsMuxerSeno();
    virtual ~TMTsMuxerSeno();

    int  Open(TMPropertyList *propList = NULL);
    int  Close();

    int  SetConfig(TMPropertyList &propertyList);
    int  GetConfig(TMPropertyList &propertyList);

    int  Start();
    int  Stop();
    int  SendPacket(TMPacket &pkt, int timeout);
    int  RecvPacket(TMPacket &pkt, int timeout);

private:
    int  MapType(TMMediaInfo::CodecID mtype);
    TsMuxer                  *mVideoMux;
    TsFrame                  *mVideoFrame;
    TsMuxer                  *mAudioMux;
    TsFrame                  *mAudioFrame;
    TMUtilRingBuffer         *mRingBufferPtr;
    bool                     mSendAble;
    bool                     mRecvAble;
    condition_variable       mMuxerCV;   //To wait the mSendAble signal and the mRecvAble signal to change.
    mutex                    mMuxerLock; //To protect the *mRingBufferPtr
    mutex                    mMuxerWaitSendV;
    mutex                    mMuxerWaitSendA;
    mutex                    mMuxerWaitRecv;
};

#endif  /* TM_TS_MUXER_SENO_H */

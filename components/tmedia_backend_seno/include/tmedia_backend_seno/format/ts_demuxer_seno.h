/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#ifndef TM_TS_DEMUXER_SENO_H
#define TM_TS_DEMUXER_SENO_H

#define LOG_LEVEL 1
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/common/clock.h>
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_factory.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_info.h>
#include <tmedia_core/entity/format/ts_demuxer.h>
#include <tmedia_core/util/util_ringbuffer.h>
#include <mpegts/ts_demuxer.hpp>
#include <mpegts/ts_typedef.h>
#include <vector>
#include <unistd.h>

using namespace std;


class TMTsDemuxerSeno final: public TMTsDemuxer
{
public:
    TMTsDemuxerSeno();
    ~TMTsDemuxerSeno();

    // TMFormatDemuxer interface
    int Open(string fileName = "", TMPropertyList *propList = NULL);
    int Close();
    int GetFormatInfo(TMFormatInfo &fmtInfo);
    int GetCodecParam(TMCodecParams &codecParams);

    int SetConfig(TMPropertyList &propertyList);
    int GetConfig(TMPropertyList &propertyList);

    int Start();
    int Stop();
    int Seek(int64_t timestamp);
    int ReadPacket(TMPacket &packet);
    int SendPacket(TMPacket &pkt, int timeout);
    int RecvPacket(TMPacket &pkt, int timeout);

private:
    TMMediaInfo::CodecID   MapType(ES_TYPE_ID mtype);
    Packet                 *mPacket;
    TsDemuxer              *mDemux;
    TMUtilRingBuffer       *mRingBufferInPtr;
    TMUtilRingBuffer       *mRingBufferOutPtr;
    char                    mTempPesData[188];
    int                     mPnum;
    bool                    mPacketFull;
    bool                    mSendAble;
    bool                    mRecvAble;
    mutex                   mDemuxerLock; //To protect the *mRingBufferInPtr
    condition_variable      mDemuxerCV;   //To Wait the mSendAble signal and the mRecvAble signal to change.
    mutex                   mDemuxerWaitSend;
    mutex                   mDemuxerWaitRecv;
};


#endif  /* TM_TS_DEMUXER_SENO_H */

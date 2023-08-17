/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_FORMAT_DEMUXER_H
#define TM_FORMAT_DEMUXER_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/entity/codec/codec_params.h>
#include <tmedia_core/entity/format/format_info.h>
#include <tmedia_core/entity/format/format.h>
#include <tmedia_core/bind/bind_inc.h>
#include <tmedia_core/entity/entity.h>

using namespace std;

typedef enum
{
    DEMUXER_EVENT_TYPE_INVALID = 0,
    DEMUXER_EVENT_TYPE_EOF     = 1 << 0,
    DEMUXER_EVENT_TYPE_EORROR  = 1 << 1,
} formatdemuxer_event_type_e;

typedef struct
{
    formatdemuxer_event_type_e type;
    unsigned int id; /* bitmasks */
} TMFormatDemuxerEventSubscription_s;


class TMFormatDemuxer : public TMFormat,
    public TMSrcEntity
{
public:
    TMFormatDemuxer();
    virtual ~TMFormatDemuxer();

    // TMFormat interface
    virtual int Open(string fileName = "", TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                   = 0;
    virtual int GetFormatInfo(TMFormatInfo &fmtInfo)      = 0;
    virtual int GetCodecParam(TMCodecParams &codecParams) = 0;

    virtual int SetConfig(TMPropertyList &propertyList)   = 0;
    virtual int GetConfig(TMPropertyList &propertyList)   = 0;

    virtual int Start()                                   = 0;
    virtual int Stop()                                    = 0;
    virtual int Seek(int64_t timestamp)                   = 0;
    virtual int ReadPacket(TMPacket &packet)              = 0;

    // TMSrcEntity interface
    TMSrcPad *GetSrcPad(TMData::Type dataType, int streamID = -1);

    // TMEntity interface
    virtual int PipeStart()  final override;
    virtual int PipePause()  final override;
    virtual int PipeStop()   final override;

protected:
    TMMediaInfo::FormatID mFormatID;

    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;
    virtual void InitDefaultPropertyList()                = 0;

    virtual int CreatePads() final override;
    virtual int DestoryPads() final override;

private:
    thread *mThread;
    bool mThreadRunFlag;
    std::mutex mPipeMutex;
    std::condition_variable mPipeCondition;

    void ThreadRoutine();

    int StartProcessData();
    int PauseProcessData();
    int StopProcessData();
};

#endif  /* TM_FORMAT_DEMUXER_H */

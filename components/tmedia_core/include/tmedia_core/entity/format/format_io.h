/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FORMAT_IO_H
#define TM_FORMAT_IO_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/entity/format/format_info.h>
#include <tmedia_core/entity/format/format.h>
#include <tmedia_core/entity/entity.h>

using namespace std;

class TMVideoInput : public TMFormat, public TMSrcEntity
{
public:
    TMVideoInput() {}
    virtual ~TMVideoInput() {}

    // TMSrcEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;

    virtual int Open(string deviceName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                  = 0;
    virtual int SetConfig(TMPropertyList &propList)                      = 0;
    virtual int GetConfig(TMPropertyList &propList)                      = 0;
    virtual int Start()                                                  = 0;
    virtual int Stop()                                                   = 0;
    virtual int RecvFrame(TMVideoFrame &frame, int timeout)              = 0;
};

class TMVideoOutput : public TMFormat, public TMSinkEntity
{
public:
    enum class PropID : uint32_t
    {
        // ID                           Data Type
        VIDEO_OUT_COORDINATE_X,
        VIDEO_OUT_COORDINATE_Y,
        VIDEO_OUT_PIXEL_WIDTH,
        VIDEO_OUT_PIXEL_HEIGHT,
        VIDEO_OUT_PIXEL_FORMAT,
        VIDEO_OUT_FRAME_RATE,
    };
    TMVideoOutput() {}
    virtual ~TMVideoOutput() {}

    // TMSinkEntity interface
    virtual TMSinkPad *GetSinkPad(int padID = 0) = 0;

    virtual int Open(string deviceName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                  = 0;
    virtual int SetConfig(TMPropertyList &propList)                      = 0;
    virtual int GetConfig(TMPropertyList &propList)                      = 0;
    virtual int Start()                                                  = 0;
    virtual int Stop()                                                   = 0;
    virtual int SendFrame(TMVideoFrame &frame, int timeout)              = 0;
};

class TMAudioInput : public TMFormat, public TMSrcEntity
{
public:
    enum class PropID : uint32_t
    {
        // ID                           Data Type
        AUDIO_IN_SAMPLE_RATE,           
        AUDIO_IN_SAMPLE_BITS,           //AUDIO_SAMPLE_BITS_8BIT or AUDIO_SAMPLE_BITS_16BIT
        AUDIO_IN_SAMPLE_CHANNELS,       //AUDIO_SAMPLE_CHANNEL_MONO or AUDIO_SAMPLE_CHANNEL_STEREO
        AUDIO_IN_DATA_TYPE,             //AUDIO_PCM_ACCESS_RW_INTERLEAVED or AUDIO_PCM_ACCESS_RW_NONINTERLEAVED
    };

    TMAudioInput() {}
    virtual ~TMAudioInput() {}

    // TMSrcEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;

    virtual int Open(string deviceName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                  = 0;
    virtual int SetConfig(TMPropertyList &propList)                      = 0;
    virtual int GetConfig(TMPropertyList &propList)                      = 0;
    virtual int Start()                                                  = 0;
    virtual int Stop()                                                   = 0;
    virtual int RecvFrame(TMAudioFrame &frame, int timeout)              = 0;
};

class TMAudioOutput : public TMFormat, public TMSinkEntity
{
public:
    enum class PropID : uint32_t
    {
        // ID                              Data Type
        AUDIO_OUT_SAMPLE_RATE,           
        AUDIO_OUT_SAMPLE_BITS,           //AUDIO_SAMPLE_BITS_8BIT or AUDIO_SAMPLE_BITS_16BIT
        AUDIO_OUT_SAMPLE_CHANNELS,       //AUDIO_SAMPLE_CHANNEL_MONO or AUDIO_SAMPLE_CHANNEL_STEREO
        AUDIO_OUT_DATA_TYPE,             //AUDIO_PCM_ACCESS_RW_INTERLEAVED or AUDIO_PCM_ACCESS_RW_NONINTERLEAVED
    };
    TMAudioOutput() {}
    virtual ~TMAudioOutput() {}

    // TMSinkEntity interface
    virtual TMSinkPad *GetSinkPad(int padID = 0) = 0;

    virtual int Open(string deviceName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                  = 0;
    virtual int SetConfig(TMPropertyList &propList)                      = 0;
    virtual int GetConfig(TMPropertyList &propList)                      = 0;
    virtual int Start()                                                  = 0;
    virtual int Stop()                                                   = 0;
    virtual int SendFrame(TMAudioFrame &frame, int timeout)              = 0;
};

#endif  /* TM_FORMAT_IO_H */

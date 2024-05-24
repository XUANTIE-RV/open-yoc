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
    enum class MediumFormat: uint32_t
    {
        MEDIUM_FORMAT_SHARE_MEMORY,
        MEDIUM_FORMAT_DMA_BUFFER_FD,
    };

    enum class PropID : uint32_t
    {
        // ID                           Data Type
        VIDEO_OUT_SURFACE_COORDINATE_X,     //surface x offset upper left corner of screen
        VIDEO_OUT_SURFACE_COORDINATE_Y,     //surface y offset upper left corner of screen
        VIDEO_OUT_SURFACE_PIXEL_WIDTH,
        VIDEO_OUT_SURFACE_PIXEL_HEIGHT,
        VIDEO_OUT_PICTURE_COORDINATE_X,     //picture x offset upper left corner of surface
        VIDEO_OUT_PICTURE_COORDINATE_Y,     //picture y offset upper left corner of surface
        VIDEO_OUT_PICTURE_PIXEL_FORMAT,
        VIDEO_OUT_FRAME_RATE,
        VIDEO_OUT_MEDIUM_FORMAT,   //share memory or dma buff fd
    };
    TMVideoOutput() 
    {
        InitDefaultPropertyList();
    }

    virtual ~TMVideoOutput() {}

    virtual int Open(string deviceName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                  = 0;
    virtual int SetConfig(TMPropertyList &propList)                      = 0;
    virtual int GetConfig(TMPropertyList &propList)                      = 0;
    virtual int Start()                                                  = 0;
    virtual int Stop()                                                   = 0;
    virtual int SendFrame(TMVideoFrame &frame, int timeout)              = 0;

protected:
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
        for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
        {
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_SURFACE_COORDINATE_X, 0, "surface_coordinate_x"));
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_SURFACE_COORDINATE_Y, 0, "surface_coordinate_y"));
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_SURFACE_PIXEL_WIDTH, 0, "surface width"));
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_SURFACE_PIXEL_HEIGHT, 0, "surface height"));

            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_PICTURE_COORDINATE_X, 0, "picture_coordinate_x"));
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_PICTURE_COORDINATE_Y, 0, "picture_coordinate_y"));

            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_PICTURE_PIXEL_FORMAT, (int)TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12, "pixel format"));
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_FRAME_RATE, 25, "frame rate"));
            pList[i]->Add(TMProperty((int)PropID::VIDEO_OUT_MEDIUM_FORMAT, (int)MediumFormat::MEDIUM_FORMAT_DMA_BUFFER_FD, "data format"));
        }
    }
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

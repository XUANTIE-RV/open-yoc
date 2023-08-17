/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_FORMAT_FACTORY_H
#define TM_FORMAT_FACTORY_H

#include <tmedia_core/common/media_info.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_muxer.h>
#include <tmedia_core/entity/format/format_io.h>
#include <tmedia_core/entity/format/camera.h>

using namespace std;

class TMFormatFactory
{
public:
    TMFormatFactory() {};
    ~TMFormatFactory() {};
};

class TMFormatDemuxerFactory : public TMFormatFactory
{
public:
    TMFormatDemuxerFactory() {};
    ~TMFormatDemuxerFactory() {};

    static TMFormatDemuxer *CreateEntity(TMMediaInfo::FormatID formatID, string class_name = "");
    static void RegisterClass(TMMediaInfo::FormatID formatID, string class_name, class_new_t func)
    {
        mDemuxerClasses()[formatID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::FormatID, pair<string, class_new_t>> &mDemuxerClasses()
    {
        static map<TMMediaInfo::FormatID, pair<string, class_new_t>> DemuxerClasses;
        return DemuxerClasses;
    }
};

class DemuxerRegister
{
public:
    DemuxerRegister(TMMediaInfo::FormatID formatID, string class_name, class_new_t func)
    {
        TMFormatDemuxerFactory::RegisterClass(formatID, class_name, func);
    }
};

#define REGISTER_DEMUXER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const DemuxerRegister reg; \
    };\
const DemuxerRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMFormatMuxerFactory : public TMFormatFactory
{
public:
    TMFormatMuxerFactory() {};
    ~TMFormatMuxerFactory() {};

    static TMFormatMuxer *CreateEntity(TMMediaInfo::FormatID formatID, string class_name = "");
    static void RegisterClass(TMMediaInfo::FormatID formatID, string class_name, class_new_t func)
    {
        mMuxerClasses()[formatID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::FormatID, pair<string, class_new_t>> &mMuxerClasses()
    {
        static map<TMMediaInfo::FormatID, pair<string, class_new_t>> MuxerClasses;
        return MuxerClasses;
    }
};

class MuxerRegister
{
public:
    MuxerRegister(TMMediaInfo::FormatID formatID, string class_name, class_new_t func)
    {
        TMFormatMuxerFactory::RegisterClass(formatID, class_name, func);
    }
};

#define REGISTER_MUXER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const MuxerRegister reg; \
    };\
const MuxerRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMFormatVideoInputFactory : public TMFormatFactory
{
public:
    TMFormatVideoInputFactory();
    ~TMFormatVideoInputFactory();

    static TMVideoInput *CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name = "");
    static void RegisterClass(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        mVideoInputClasses()[deviceID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> &mVideoInputClasses()
    {
        static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> VideoInputClasses;
        return VideoInputClasses;
    }
};
class VideoInputRegister
{
public:
    VideoInputRegister(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        TMFormatVideoInputFactory::RegisterClass(deviceID, class_name, func);
    }
};

#define REGISTER_VIDEO_INPUT_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const VideoInputRegister reg; \
    };\
const VideoInputRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMFormatVideoOutputFactory : public TMFormatFactory
{
public:
    TMFormatVideoOutputFactory();
    ~TMFormatVideoOutputFactory();

    static TMVideoOutput *CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name = "");
    static void RegisterClass(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        mVideoOutputClasses()[deviceID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> &mVideoOutputClasses()
    {
        static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> VideoOutputClasses;
        return VideoOutputClasses;
    }
};
class VideoOutputRegister
{
public:
    VideoOutputRegister(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        TMFormatVideoOutputFactory::RegisterClass(deviceID, class_name, func);
    }
};

#define REGISTER_VIDEO_OUTPUT_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const VideoOutputRegister reg; \
    };\
const VideoOutputRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMFormatAudioInputFactory : public TMFormatFactory
{
public:
    TMFormatAudioInputFactory();
    ~TMFormatAudioInputFactory();

    static TMAudioInput *CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name = "");
    static void RegisterClass(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        mAudioInputClasses()[deviceID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> &mAudioInputClasses()
    {
        static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> AudioInputClasses;
        return AudioInputClasses;
    }
};
class AudioInputRegister
{
public:
    AudioInputRegister(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        TMFormatAudioInputFactory::RegisterClass(deviceID, class_name, func);
    }
};

#define REGISTER_AUDIO_INPUT_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const AudioInputRegister reg; \
    };\
const AudioInputRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMFormatAudioOutputFactory : public TMFormatFactory
{
public:
    TMFormatAudioOutputFactory();
    ~TMFormatAudioOutputFactory();

    static TMAudioOutput *CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name = "");
    static void RegisterClass(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        mAudioOutputClasses()[deviceID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> &mAudioOutputClasses()
    {
        static map<TMMediaInfo::DeviceID, pair<string, class_new_t>> AudioOutputClasses;
        return AudioOutputClasses;
    }
};
class AudioOutputRegister
{
public:
    AudioOutputRegister(TMMediaInfo::DeviceID deviceID, string class_name, class_new_t func)
    {
        TMFormatAudioOutputFactory::RegisterClass(deviceID, class_name, func);
    }
};

#define REGISTER_AUDIO_OUTPUT_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const AudioOutputRegister reg; \
    };\
const AudioOutputRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

#endif  /* TM_FORMAT_FACTORY_H */

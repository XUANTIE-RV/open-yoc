/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_CODEC_FACTORY_H
#define TM_CODEC_FACTORY_H

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>
#include <tmedia_core/entity/codec/decoder.h>
#include <tmedia_core/entity/codec/encoder.h>

using namespace std;

class TMCodecFactory
{
public:
    TMCodecFactory()    {};
    ~TMCodecFactory()   {};
};

class TMVideoDecoderFactory : public TMCodecFactory
{
public:
    static TMVideoDecoder* CreateEntity(TMMediaInfo::CodecID codecID, string class_name = "");
    static void RegisterClass(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        mVideoDecoderClasses()[codecID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::CodecID, pair<string, class_new_t>> &mVideoDecoderClasses()
    {
        static map<TMMediaInfo::CodecID, pair<string, class_new_t>> VideoDecoderClasses;
        return VideoDecoderClasses;
    }
};

class VideoDecoderRegister
{
public:
    VideoDecoderRegister(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        TMVideoDecoderFactory::RegisterClass(codecID, class_name, func);
    }
};

#define REGISTER_VIDEO_DECODER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const VideoDecoderRegister reg; \
    };\
const VideoDecoderRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMVideoEncoderFactory : public TMCodecFactory
{
public:
    static TMVideoEncoder* CreateEntity(TMMediaInfo::CodecID codecID, string class_name = "");
    static void RegisterClass(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        mVideoEncoderClasses()[codecID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::CodecID, pair<string, class_new_t>> &mVideoEncoderClasses()
    {
        static map<TMMediaInfo::CodecID, pair<string, class_new_t>> VideoEncoderClasses;
        return VideoEncoderClasses;
    }
};

class VideoEncoderRegister
{
public:
    VideoEncoderRegister(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        TMVideoEncoderFactory::RegisterClass(codecID, class_name, func);
    }
};

#define REGISTER_VIDEO_ENCODER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const VideoEncoderRegister reg; \
    };\
const VideoEncoderRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMAudioDecoderFactory : public TMCodecFactory
{
public:
    static TMAudioDecoder* CreateEntity(TMMediaInfo::CodecID codecID, string class_name = "");
    static void RegisterClass(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        mAudioDecoderClasses()[codecID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::CodecID, pair<string, class_new_t>> &mAudioDecoderClasses()
    {
        static map<TMMediaInfo::CodecID, pair<string, class_new_t>> AudioDecoderClasses;
        return AudioDecoderClasses;
    }
};

class AudioDecoderRegister
{
public:
    AudioDecoderRegister(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        TMAudioDecoderFactory::RegisterClass(codecID, class_name, func);
    }
};

#define REGISTER_AUDIO_DECODER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const AudioDecoderRegister reg; \
    };\
const AudioDecoderRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

class TMAudioEncoderFactory : public TMCodecFactory
{
public:
    static TMAudioEncoder* CreateEntity(TMMediaInfo::CodecID codecID, string class_name = "");
    static void RegisterClass(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        mAudioEncoderClasses()[codecID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::CodecID, pair<string, class_new_t>> &mAudioEncoderClasses()
    {
        static map<TMMediaInfo::CodecID, pair<string, class_new_t>> AudioEncoderClasses;
        return AudioEncoderClasses;
    }
};

class AudioEncoderRegister
{
public:
    AudioEncoderRegister(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        TMAudioEncoderFactory::RegisterClass(codecID, class_name, func);
    }
};

#define REGISTER_AUDIO_ENCODER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const AudioEncoderRegister reg; \
    };\
const AudioEncoderRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

#endif  /* TM_CODEC_FACTORY_H */

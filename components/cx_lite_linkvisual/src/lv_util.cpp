#include "lv_util.h"
#include <iostream>

using namespace std;

lv_video_format_e lv::MapEncodeFormat_CX2LV(cx::VencoderType type)
{
    lv_video_format_e format_out = LV_VIDEO_FORMAT_H264;
	switch (type)
    {
    case cx::VENC_H264:
        format_out = LV_VIDEO_FORMAT_H264;
        break;
    case cx::VENC_H265:
        format_out = LV_VIDEO_FORMAT_H265;
        break;
    default:
        printf("LV err encode format:%d\n", (int)type);
    }
    return format_out;
}

int lv::MapVideoFrameType_CX2LV(TMMediaInfo::PictureType type)
{
    int type_out = -1;
	switch (type)
    {
    case TMMediaInfo::PictureType::I:
        type_out = 1;
        break;
    case TMMediaInfo::PictureType::P:
        type_out = 0;
        break;
    case TMMediaInfo::PictureType::B:
        type_out = 0;
        break;
    default:
        printf("LV err video frame type:%d\n", (int)type);
    }
    return type_out;
}
cx::AudioEncoderType lv::MapAudioEncodeType_LV2CX(lv_audio_format_e type)
{
	cx::AudioEncoderType format_out = cx::AENC_NONE;
	switch (type)
    {
    case LV_AUDIO_FORMAT_G711A:
        format_out = cx::AENC_G711A;
        break;
    case LV_AUDIO_FORMAT_G711U:
        format_out = cx::AENC_G711U;
        break;
    case LV_AUDIO_FORMAT_AAC:
        format_out = cx::AENC_AAC;
        break;
    case LV_AUDIO_FORMAT_PCM:
        format_out = cx::AENC_NONE;
        break;
    default:
        printf("LV err audio encode format:%d\n", (int)type);
    }
    return format_out;
}
cx::AudioSampleBits lv::MapAudioSampleBits_LV2CX(lv_audio_sample_bits_e inParam)
{
    cx::AudioSampleBits outParam = cx::AUD_SAMPLE_BITS_NONE;
    switch (inParam)
    {
    case LV_AUDIO_SAMPLE_BITS_8BIT:
        outParam = cx::AUD_SAMPLE_BITS_8BIT;
        break;
    case LV_AUDIO_SAMPLE_BITS_16BIT:
        outParam = cx::AUD_SAMPLE_BITS_16BIT;
        break;
    default:
        printf("LV err audio sample bits:%d\n", (int)inParam);
        break;
    }
    return outParam;
}

cx::AudioSampleChannels lv::MapAudioSampleChannels_LV2CX(lv_audio_channel_e inParam)
{
    cx::AudioSampleChannels outParam = cx::AUD_SAMPLE_CHANNEL_NONE;
    switch (inParam)
    {
    case LV_AUDIO_CHANNEL_MONO:
        outParam = cx::AUD_SAMPLE_CHANNEL_MONO;
        break;
    case LV_AUDIO_CHANNEL_STEREO:
        outParam = cx::AUD_SAMPLE_CHANNEL_STEREO;
        break;
    default:
        printf("LV err audio sample channels:%d\n", (int)inParam);
        break;
    }
    return outParam;
}

int lv::MapAudioSampleRate_LV2CX(lv_audio_sample_rate_e inParam) 
{
    int outParam = -1;
    switch (inParam)
    {
    case LV_AUDIO_SAMPLE_RATE_16000:
        outParam = 16000;
        break;
    case LV_AUDIO_SAMPLE_RATE_8000:
        outParam = 8000;
        break;
    default:
        printf("LV err audio sample rate:%d\n", (int)inParam);
        break;
    }
    return outParam;
}

lv_audio_format_e lv::MapAudioEncodeType_CX2LV(cx::AudioEncoderType type)
{
	lv_audio_format_e format_out = LV_AUDIO_FORMAT_G711A;
	switch (type)
    {
    case cx::AENC_G711A:
        format_out = LV_AUDIO_FORMAT_G711A;
        break;
    case cx::AENC_G711U:
        format_out = LV_AUDIO_FORMAT_G711U;
        break;
    case cx::AENC_AAC:
        format_out = LV_AUDIO_FORMAT_AAC;
        break;
    case cx::AENC_NONE:
        format_out = LV_AUDIO_FORMAT_PCM;
        break;
    default:
        printf("LV err audio encode format:%d\n", (int)type);
        break;
    }
    return format_out;
}
lv_audio_sample_bits_e lv::MapAudioSampleBits_CX2LV(cx::AudioSampleBits inParam)
{
    lv_audio_sample_bits_e outParam = LV_AUDIO_SAMPLE_BITS_16BIT;
    switch (inParam)
    {
    case cx::AUD_SAMPLE_BITS_8BIT:
        outParam = LV_AUDIO_SAMPLE_BITS_8BIT;
        break;
    case cx::AUD_SAMPLE_BITS_16BIT:
        outParam = LV_AUDIO_SAMPLE_BITS_16BIT;
        break;
    default:
        printf("LV err audio sample bits:%d\n", (int)inParam);
        break;
    }
    return outParam;
}

lv_audio_channel_e lv::MapAudioSampleChannels_CX2LV(cx::AudioSampleChannels inParam)
{
    lv_audio_channel_e outParam = LV_AUDIO_CHANNEL_STEREO;
    switch (inParam)
    {
    case cx::AUD_SAMPLE_CHANNEL_MONO:
        outParam = LV_AUDIO_CHANNEL_MONO;
        break;
    case cx::AUD_SAMPLE_CHANNEL_STEREO:
        outParam = LV_AUDIO_CHANNEL_STEREO;
        break;
    default:
        printf("LV err audio sample channels:%d\n", (int)inParam);
        break;
    }
    return outParam;
}

lv_audio_sample_rate_e lv::MapAudioSampleRate_CX2LV(int inParam) 
{
    lv_audio_sample_rate_e outParam = LV_AUDIO_SAMPLE_RATE_16000;
    switch (inParam)
    {
    case 16000:
        outParam = LV_AUDIO_SAMPLE_RATE_16000;
        break;
    case 8000:
        outParam = LV_AUDIO_SAMPLE_RATE_8000;
        break;
    default:
        printf("LV err audio sample rate:%d\n", inParam);
        break;
    }
    return outParam;
}
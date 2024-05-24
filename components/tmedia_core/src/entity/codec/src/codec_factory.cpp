/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#define LOG_LEVEL 3 // Should at the top of all codes

#include <iostream>
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/entity/codec/codec_inc.h>

using namespace std;

TMVideoDecoder* TMVideoDecoderFactory::CreateEntity(TMMediaInfo::CodecID codecID, string class_name)
{
    map<TMMediaInfo::CodecID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mVideoDecoderClasses().find(codecID);
        if (it != mVideoDecoderClasses().end())
        {
            TMVideoDecoder* codec = (TMVideoDecoder*)it->second.second();
            if (codec != NULL)
                codec->mName = "VideoDecoder_" + TMMediaInfo::Name(codecID);
            return codec;
        }
        else
        {
            LOG_E("Video Decoder '%s' not supported\n", TMMediaInfo::Name(codecID).c_str());
            return NULL;
        }
    }
    else
    {
        for (it = mVideoDecoderClasses().begin(); it != mVideoDecoderClasses().end(); it++)
        {
            if ((it->first == codecID) && (it->second.first == class_name))
            {
                TMVideoDecoder* codec = (TMVideoDecoder*)it->second.second();
                if (codec != NULL)
                    codec->mName = "VideoDecoder_" + TMMediaInfo::Name(codecID);
                return codec;
            }
        }

        LOG_E("Video Decoder '%s':'%s' not supported\n",
              TMMediaInfo::Name(codecID).c_str(), class_name.c_str());
        return NULL;
    }
}

TMVideoEncoder* TMVideoEncoderFactory::CreateEntity(TMMediaInfo::CodecID codecID, string class_name)
{
    map<TMMediaInfo::CodecID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mVideoEncoderClasses().find(codecID);
        if (it != mVideoEncoderClasses().end())
        {
            TMVideoEncoder* codec = (TMVideoEncoder*)it->second.second();
            if (codec != NULL)
                codec->mName = "VideoEncoder_" + TMMediaInfo::Name(codecID);
            return codec;
        }
        else
        {
            cout << "Video Encoder '" << TMMediaInfo::Name(codecID) << "' not supported" << endl;
            return NULL;
        }
    }
    else
    {
        for (it = mVideoEncoderClasses().begin(); it != mVideoEncoderClasses().end(); it++)
        {
            if ((it->first == codecID) && (it->second.first == class_name))
            {
                TMVideoEncoder* codec = (TMVideoEncoder*)it->second.second();
                if (codec != NULL)
                    codec->mName = "VideoEncoder_" + TMMediaInfo::Name(codecID);
                return codec;
            }
        }

        LOG_E("Video Encoder '%s':'%s' not supported\n",
              TMMediaInfo::Name(codecID).c_str(), class_name.c_str());
        return NULL;
    }
}

TMAudioEncoder* TMAudioEncoderFactory::CreateEntity(TMMediaInfo::CodecID codecID, string class_name)
{
    map<TMMediaInfo::CodecID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mAudioEncoderClasses().find(codecID);
        if (it != mAudioEncoderClasses().end())
        {
            TMAudioEncoder* codec = (TMAudioEncoder*)it->second.second();
            if (codec != NULL)
                codec->mName = "AudioEncoder_" + TMMediaInfo::Name(codecID);
            return codec;
        }
        else
        {
            cout << "Audio Encoder '" << TMMediaInfo::Name(codecID) << "' not supported" << endl;
            return NULL;
        }
    }
    else
    {
        for (it = mAudioEncoderClasses().begin(); it != mAudioEncoderClasses().end(); it++)
        {
            if ((it->first == codecID) && (it->second.first == class_name))
            {
                TMAudioEncoder* codec = (TMAudioEncoder*)it->second.second();
                if (codec != NULL)
                    codec->mName = "AudioEncoder_" + TMMediaInfo::Name(codecID);
                return codec;
            }
        }

        LOG_E("Audio Encoder '%s':'%s' not supported\n",
              TMMediaInfo::Name(codecID).c_str(), class_name.c_str());
        return NULL;
    }
}

TMAudioDecoder* TMAudioDecoderFactory::CreateEntity(TMMediaInfo::CodecID codecID, string class_name)
{
    map<TMMediaInfo::CodecID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mAudioDecoderClasses().find(codecID);
        if (it != mAudioDecoderClasses().end())
        {
            TMAudioDecoder* codec = (TMAudioDecoder*)it->second.second();
            if (codec != NULL)
                codec->mName = "AudioDecoder_" + TMMediaInfo::Name(codecID);
            return codec;
        }
        else
        {
            cout << "Audio Decoder '" << TMMediaInfo::Name(codecID) << "' not supported" << endl;
            return NULL;
        }
    }
    else
    {
        for (it = mAudioDecoderClasses().begin(); it != mAudioDecoderClasses().end(); it++)
        {
            if ((it->first == codecID) && (it->second.first == class_name))
            {
                TMAudioDecoder* codec = (TMAudioDecoder*)it->second.second();
                if (codec != NULL)
                    codec->mName = "AudioDecoder_" + TMMediaInfo::Name(codecID);
                return codec;
            }
        }

        LOG_E("Audio Decoder '%s':'%s' not supported\n",
              TMMediaInfo::Name(codecID).c_str(), class_name.c_str());
        return NULL;
    }
}

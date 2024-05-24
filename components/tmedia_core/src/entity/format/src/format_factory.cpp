/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <iomanip>
#include <tmedia_core/entity/format/format_inc.h>

using namespace std;

TMFormatDemuxer *TMFormatDemuxerFactory::CreateEntity(TMMediaInfo::FormatID formatID, string class_name)
{
    map<TMMediaInfo::FormatID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mDemuxerClasses().find(formatID);
        if (it != mDemuxerClasses().end())
        {
            TMFormatDemuxer *demuxer = (TMFormatDemuxer *)it->second.second();
            demuxer->mName = "Demuxer_" + TMMediaInfo::Name(formatID);
            return demuxer;
        } else
        {
            cout << "Format Demuxer '" << TMMediaInfo::Name(formatID) << "' not supported" << endl;
            return NULL;
        }
    } else
    {
        for (it = mDemuxerClasses().begin(); it != mDemuxerClasses().end(); it++)
        {
            if ((it->first == formatID) && (it->second.first == class_name))
            {
                TMFormatDemuxer *demuxer = (TMFormatDemuxer *)it->second.second();
                demuxer->mName = "Demuxer_" + TMMediaInfo::Name(formatID);
                return demuxer;
            }
        }
    }
    cout << "Format Demuxer '" << class_name << "' not supported" << endl;
    return NULL;
}

TMFormatMuxer *TMFormatMuxerFactory::CreateEntity(TMMediaInfo::FormatID formatID, string class_name)
{
    map<TMMediaInfo::FormatID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mMuxerClasses().find(formatID);
        if (it != mMuxerClasses().end())
        {
            return (TMFormatMuxer *)it->second.second();
        } else
        {
            cout << "Format Muxer '" << TMMediaInfo::Name(formatID) << "' not supported" << endl;
            return NULL;
        }
    } else
    {
        for (it = mMuxerClasses().begin(); it != mMuxerClasses().end(); it++)
        {
            if ((it->first == formatID) && (it->second.first == class_name))
                return (TMFormatMuxer *)it->second.second();
        }
    }
    cout << "Format Muxer '" << class_name << "' not supported" << endl;
    return NULL;
}

TMVideoInput *TMFormatVideoInputFactory::CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name)
{
    map<TMMediaInfo::DeviceID, vector<pair<string, class_new_t>>>::iterator it;
    if (class_name.empty())
    {
        it = mVideoInputClasses().find(deviceID);
        if (it != mVideoInputClasses().end())
        {
            vector<pair<string, class_new_t>> &vectorData = mVideoInputClasses()[deviceID];
            cout << "Here are entities matched ID named:" << endl;
            for (size_t i = 0; i < vectorData.size(); i++)
            {
                cout << left << setw(20) << vectorData[i].first << "";
            }
            cout << endl;
            cout << "No class name specified, create entity called: %s" << vectorData[0].first << endl;
            return (TMVideoInput *)vectorData[0].second();
        } else
        {
            cout << "Format VideoInput '" << TMMediaInfo::Name(deviceID) << "' not supported" << endl;
            return NULL;
        }
    } else
    {
        it = mVideoInputClasses().find(deviceID);
        if (it != mVideoInputClasses().end())
        {
            vector<pair<string, class_new_t>> &vectorData = mVideoInputClasses()[deviceID];
            for (size_t i = 0; i < vectorData.size(); i++)
            {
                if (vectorData[i].first == class_name)
                {
                    return (TMVideoInput *)vectorData[i].second();
                }
            }
        } else
        {
            cout << "Format VideoInput '" << TMMediaInfo::Name(deviceID) << "' not supported" << endl;
            return NULL;
        }
    }
    cout << "Format VideoInput '" << class_name << "' not supported" << endl;
    return NULL;
}

TMVideoOutput *TMFormatVideoOutputFactory::CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name)
{
    map<TMMediaInfo::DeviceID, std::vector<std::pair<string, class_new_t>>>::iterator it;
    if (class_name.empty())
    {
        it = mVideoOutputClasses().find(deviceID);
        if (it != mVideoOutputClasses().end())
        {
            std::vector<std::pair<string, class_new_t>> &vectorData = mVideoOutputClasses()[deviceID];
            cout << "Here are entities matched ID named:" << endl;
            for(size_t i=0; i < vectorData.size(); i++)
            {
                cout << left << setw(20) << vectorData[i].first << "";
            }
            cout << endl;
            cout << "No class name specified, create entity called: %s" << vectorData[0].first << endl;
            return (TMVideoOutput *)vectorData[0].second();
        } else
        {
            cout << "Format VideoOutput '" << TMMediaInfo::Name(deviceID) << "' not supported" << endl;
            return NULL;
        }
    } else
    {
        it = mVideoOutputClasses().find(deviceID);
        if (it != mVideoOutputClasses().end())
        {
            std::vector<std::pair<string, class_new_t>> &vectorData = mVideoOutputClasses()[deviceID];
            for(size_t i = 0; i < vectorData.size(); i++)
            {
                if (vectorData[i].first == class_name)
                {
                    return (TMVideoOutput *)vectorData[i].second();
                }
            }
        }
    }
    cout << "Format VideoOutput '" << class_name << "' not supported" << endl;
    return NULL;
}

TMAudioInput *TMFormatAudioInputFactory::CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name)
{
    map<TMMediaInfo::DeviceID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mAudioInputClasses().find(deviceID);
        if (it != mAudioInputClasses().end())
        {
            return (TMAudioInput *)it->second.second();
        } else
        {
            cout << "Format AudioInput '" << TMMediaInfo::Name(deviceID) << "' not supported" << endl;
            return NULL;
        }
    } else
    {
        for (it = mAudioInputClasses().begin(); it != mAudioInputClasses().end(); it++)
        {
            if ((it->first == deviceID) && (it->second.first == class_name))
                return (TMAudioInput *)it->second.second();
        }
    }
    cout << "Format AudioInput '" << class_name << "' not supported" << endl;
    return NULL;
}

TMAudioOutput *TMFormatAudioOutputFactory::CreateEntity(TMMediaInfo::DeviceID deviceID, string class_name)
{
    map<TMMediaInfo::DeviceID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty())
    {
        it = mAudioOutputClasses().find(deviceID);
        if (it != mAudioOutputClasses().end())
        {
            return (TMAudioOutput *)it->second.second();
        } else
        {
            cout << "Format AudioOutput '" << TMMediaInfo::Name(deviceID) << "' not supported" << endl;
            return NULL;
        }
    } else
    {
        for (it = mAudioOutputClasses().begin(); it != mAudioOutputClasses().end(); it++)
        {
            if ((it->first == deviceID) && (it->second.first == class_name))
                return (TMAudioOutput *)it->second.second();
        }
    }
    cout << "Format AudioOutput '" << class_name << "' not supported" << endl;
    return NULL;
}

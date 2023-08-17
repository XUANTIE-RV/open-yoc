/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_DATA_H
#define TM_DATA_H

#include <tmedia_core/common/error.h>
using namespace std;

class TMData
{
public:
    enum class Type : uint32_t
    {
        UNKNOWN,
        CUSTOM_DATA,

        STREAM_VIDEO,
        STREAM_AUDIO,

        FRAME_VIDEO,
        FRAME_AUDIO,
    };

    TMData() {}
    virtual ~TMData() {}
    virtual int UnRef() {return TMResult::TM_OK;}   // TODO: make it to be pure virtual function

    static const string Name(TMData::Type type)
    {
        switch(type)
        {
        case TMData::Type::STREAM_VIDEO:
            return "STREAM_VIDEO";
        case TMData::Type::STREAM_AUDIO:
            return "STREAM_AUDIO";
        case TMData::Type::FRAME_VIDEO:
            return "FRAME_VIDEO";
        case TMData::Type::FRAME_AUDIO:
            return "FRAME_AUDIO";
        case TMData::Type::CUSTOM_DATA:
            return "CUSTOM_DATA";
        default:
            return "UNKNOWN";
        }
    }

    Type mDataType;
    uint64_t mSeqNum;
};

#endif  /* TM_DATA_H */

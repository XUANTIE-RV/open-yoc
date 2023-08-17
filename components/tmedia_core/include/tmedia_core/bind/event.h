/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_EVENT_H
#define TM_EVENT_H

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <queue>

using namespace std;

class TMEvent
{
public:
    enum class Type : uint32_t
    {
        UNKNOWN,

        /* bidirection events */
        START,
        STOP,
        PAUSE,
        RESUME,

        /* downstream events */
        EOS,

        /* customized events */
        CUSTOM_DUMMY = 128,
    };

    TMEvent() : mType(Type::UNKNOWN) {};
    TMEvent(TMEvent::Type type, uint64_t timeStamp=0, uint64_t seqNum=0)
           : mType(type), mTimeStamp(timeStamp), mSeqNum(seqNum) {};
    virtual ~TMEvent() {};

    virtual void DumpInfo();
    const string Name();
    static const string Name(TMEvent::Type type);

    TMEvent::Type mType;
    uint64_t mTimeStamp;
    uint64_t mSeqNum;
};

#endif  /* TM_EVENT_H */

/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <iostream>

#define LOG_LEVEL 3
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/bind/event.h>

using namespace std;

const string TMEvent::Name()
{
    return Name(mType);
}

const string TMEvent::Name(TMEvent::Type type)
{
    switch (type)
    {
    case Type::START:
        return "START";
    case Type::STOP:
        return "STOP";
    case Type::PAUSE:
        return "PAUSE";
    case Type::RESUME:
        return "RESUME";
    case Type::EOS:
        return "EOS";
    case Type::CUSTOM_DUMMY:
        return "CUSTOM_DUMMY";
    default:
        return "Unknown";
    }
}

void TMEvent::DumpInfo()
{
    cout << "  Type      : " << Name(mType) << endl;
    cout << "  TimeStamp : " << mTimeStamp << endl;
    cout << "  SeqNum    : " << mSeqNum << endl;
}


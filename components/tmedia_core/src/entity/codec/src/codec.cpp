/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/entity/codec/codec.h>

using namespace std;

TMCodec::TMCodec()
    : mCodecID(TMMediaInfo::CodecID::UNKNOWN),
      mWorkMode(TMMediaInfo::WorkMode::UNKNOWN)
{
}

TMCodec::~TMCodec()
{
}

TMMediaInfo::CodecID TMCodec::GetCodecID()
{
    return mCodecID;
}

string TMCodec::GetCodecName()
{
    return TMMediaInfo::Name(mCodecID);
}

TMMediaInfo::WorkMode TMCodec::GetWorkMode()
{
    return mWorkMode;
}


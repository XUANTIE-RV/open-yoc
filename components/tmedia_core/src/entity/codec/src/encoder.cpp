/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/entity/codec/encoder.h>

using namespace std;

TMVideoEncoder::TMVideoEncoder()
{
    mWorkMode = TMMediaInfo::WorkMode::ENCODE;
}

TMVideoEncoder::~TMVideoEncoder()
{
}

TMAudioEncoder::TMAudioEncoder()
{
    mWorkMode = TMMediaInfo::WorkMode::ENCODE;
}

TMAudioEncoder::~TMAudioEncoder()
{
}

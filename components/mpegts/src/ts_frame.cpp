/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */
#include <mpegts/ts_frame.hpp>

TsFrame::TsFrame()
{
    reset();
}

void TsFrame::setFrame(const uint8_t *data, size_t size)
{
    if (data && size)
    {
        _mSize = size;
        _mData = (uint8_t *)data;
    }
}

void TsFrame::setTimestamp(uint64_t pts, uint64_t dts)
{
    _mPts  = pts;
    _mDts  = dts;
    //_mPcr  = pcr;
}

void TsFrame::setPid(uint16_t pid)
{
    _mPid  = pid;
}

void TsFrame::setType(ES_TYPE_ID type)
{
    _mType = type;
}

void TsFrame::reset()
{
    _mPid  = 0;
    _mSize = 0;
    _mData = NULL;
    _mType = ES_TYPE_ID::UNKNOWN;
    _mPts  = _mDts  = 0;
    //_mPcr = 0;
}

bool  TsFrame::frame_setted() const
{
    if (_mData && _mSize && _mPid && _mType)
    {
        return true;
    }
    else
    {
        return false;
    }
}



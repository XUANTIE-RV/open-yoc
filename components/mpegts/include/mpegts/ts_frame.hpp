/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_FRAME_HPP__
#define __TS_FRAME_HPP__

#include <mpegts/ts_typedef.h>

class TsFrame
{
    friend class TsMuxer;

public:
    TsFrame();

public:
    void         reset();
    void         setFrame(const uint8_t *data, size_t size);
    void         setTimestamp(uint64_t pts, uint64_t dts);
    void         setPid(uint16_t pid);
    void         setType(ES_TYPE_ID type);
    bool         frame_setted() const;

private:
    uint8_t      *_mData;
    size_t       _mSize;
    uint64_t     _mPts;
    uint64_t     _mDts;
    //uint64_t     _mPcr;
    ES_TYPE_ID    _mType;
    uint16_t     _mPid;
};
#endif

/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __TS_FRAME_HPP__
#define __TS_FRAME_HPP__

#include <memory>
#include <mpegts/ts_typedef.h>

class TsMuxer;
class TsFrame final
{
    friend class TsMuxer;
public:
    TsFrame();

public:
    void         reset();
    void         setFrame(const uint8_t *data, size_t size);
    void         setTimestamp(uint64_t pts, uint64_t dts, uint64_t pcr = 0);
    void         setPid(uint16_t pid);
    void         setType(es_type_t type);

private:
    uint8_t      *_mData;
    size_t       _mSize;
    uint64_t     _mPts;
    uint64_t     _mDts;
    uint64_t     _mPcr;
    es_type_t    _mType;
    uint16_t     _mPid;
};

#endif /* __TS_FRAME_HPP__ */


/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __TS_PACKET_HPP__
#define __TS_PACKET_HPP__

#include <memory>
#include <mpegts/ts_typedef.h>
#include <mpegts/bytes_io.hpp>

class TsPacket final
{
public:
    TsPacket(size_t capacity = 0);

    void                clear();
    void                copy(TsPacket *packet);

public:
    shared_ptr<BytesIO> mData;
    uint64_t            mPts;
    uint64_t            mDts;
    es_type_t           mType;
    uint16_t            mPid;
};


#endif /* __TS_PACKET_HPP__ */


/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __BYTES_IO_HPP__
#define __BYTES_IO_HPP__

#include <mpegts/ts_typedef.h>
#include <vector>
using namespace std;

class BytesIO final
{
public:
    BytesIO();
    BytesIO(size_t capacity);
    ~BytesIO();

    void       w8(uint8_t val);
    void       w16be(uint16_t val);
    void       w24be(uint32_t val);
    void       w32be(uint32_t val);
    void       w64be(uint64_t val);

    size_t     size();
    uint8_t    *data();
    void       setData(const uint8_t *data, size_t len, size_t offset);
    void       append(const uint8_t *data, size_t size);
    void       append(uint8_t ch, size_t size = 1);
    void       resetCurrPos();
    int        getCurrPos();

private:
    size_t           _mPos;
    vector<uint8_t>  _mData;
    size_t           _mCurrPos;
};
#endif

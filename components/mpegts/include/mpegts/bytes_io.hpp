/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __BYTES_IO_HPP__
#define __BYTES_IO_HPP__

#include <string>
#include <vector>
#include <mpegts/ts_common.h>

using namespace std;

class BytesIO final
{
public:
    BytesIO();
    BytesIO(size_t capacity);
    ~BytesIO();

public:
    void       w8(uint8_t val);
    void       w16be(uint16_t val);
    void       w24be(uint32_t val);
    void       w32be(uint32_t val);
    void       w64be(uint64_t val);
    uint8_t    r8();
    uint16_t   r16be();
    uint32_t   r24be();
    uint32_t   r32be();
    uint64_t   r64be();

    void       skip(size_t size);
    /**
     * @brief  reset the read position & remove data of readed
     * @return
     */
    void       rewind();
    bool       empty();
    size_t     size();

    /**
     * @brief  get the read pos of the inner vector
     * @return
     */
    size_t     pos();
    uint8_t    *data();
    void       clear();

    void       setData(const uint8_t *data, size_t len, size_t offset);

    /**
     * @brief  read data from current read pos of the inner vector
     * @param  [in] data
     * @param  [in] len
     * @return
     */
    int        readData(uint8_t *data, size_t len);
    void       append(const uint8_t *data, size_t size);
    void       append(uint8_t ch, size_t size = 1);

private:
    size_t           _mPos;
    vector<uint8_t>  _mData;
};

#endif /* __BYTES_IO_HPP__ */


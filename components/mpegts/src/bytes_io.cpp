/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/
#include <mpegts/bytes_io.hpp>

BytesIO::BytesIO()
{
    _mPos = 0;
}

BytesIO::BytesIO(size_t capacity)
{
    _mPos = 0;
    _mData.reserve(capacity);
}

BytesIO::~BytesIO()
{
}

void BytesIO::w8(uint8_t value)
{
    _mData.push_back(value);
    _mCurrPos += 1;
}

void BytesIO::w16be(uint16_t value)
{
    uint8_t *p = (uint8_t *)&value;

    for (int i = 1; i >= 0; --i)
    {
        _mData.push_back(p[i]);
        _mCurrPos++;
    }
}

void BytesIO::w24be(uint32_t value)
{
    uint8_t *p = (uint8_t *)&value;

    for (int i = 2; i >= 0; --i)
    {
        _mData.push_back(p[i]);
        _mCurrPos++;
    }
}

void BytesIO::w32be(uint32_t value)
{
    uint8_t *p = (uint8_t *)&value;

    for (int i = 3; i >= 0; --i)
    {
        _mData.push_back(p[i]);
        _mCurrPos++;
    }
}

void BytesIO::w64be(uint64_t value)
{
    uint8_t *p = (uint8_t *)&value;

    for (int i = 7; i >= 0; --i)
    {
        _mData.push_back(p[i]);
        _mCurrPos++;
    }
}

void BytesIO::resetCurrPos()
{
    _mCurrPos = 0;
}

int BytesIO::getCurrPos()
{
    return _mCurrPos;
}

void BytesIO::append(const uint8_t *data, size_t size)
{
    if (data && size)
    {
        _mData.insert(_mData.end(), data, data + size);
        _mCurrPos += size;
    }
}

void BytesIO::append(uint8_t ch, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        _mData.push_back(ch);
        _mCurrPos++;
    }
}

size_t BytesIO::size()
{
    return _mData.size();
}

uint8_t *BytesIO::data()
{
    return size() ? &_mData[0] : NULL;
}

void BytesIO::setData(const uint8_t *data, size_t len, size_t offset)
{
    if (data && len && offset + len <= size())
    {
        for (int i = 0; i < int(len); i++)
        {
            _mData[offset + i] = data[i];
        }
    }
}






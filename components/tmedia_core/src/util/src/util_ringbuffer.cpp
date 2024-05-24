/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>
#include <tmedia_core/util/util_ringbuffer.h>
#include <tmedia_core/common/syslog.h>

TMUtilRingBuffer::TMUtilRingBuffer(int size)
{
    mBuffer = (char *)malloc(size + 1);
    if (mBuffer == nullptr)
    {
        LOG_E("rb malloc fail\n");
    }
    mSize = size;
    mRidx = mWidx = 0;
}

TMUtilRingBuffer::~TMUtilRingBuffer()
{
    if (mBuffer)
    {
        free(mBuffer);
    }
}

int TMUtilRingBuffer::Read(char *buff, int size)
{
    int RealSize = 0;
    if (size <= 0)
    {
        return -1;
    }
    if (IsEmpty())
    {
        return 0;
    }
    /* get real read size */
    int buffer_size = AvaliableReadSpace();
    RealSize = std::min(size, buffer_size);
    /* cp data to user buffer */
    for (int i = 0; i < RealSize; i++)
    {
        buff[i] = mBuffer[mRidx];
        mRidx++;
        if (mRidx >= mSize + 1)
        {
            mRidx = 0;
        }
    }
    return RealSize;
}

int TMUtilRingBuffer::Write(char *buff, int size)
{
    if (AvaliableWriteSpace() < size)
    {
        LOG_D("rb full\n");
        return -1;
    }

    for (int i = 0; i < size; i++)
    {
        mBuffer[mWidx] = buff[i];
        mWidx++;
        if (mWidx >= mSize + 1)
        {
            mWidx = 0;
        }
    }
    return size;
}

int TMUtilRingBuffer::Clear()
{
    mRidx = mWidx = 0;
    return 0;
}

int TMUtilRingBuffer::AvaliableReadSpace()
{
    if (mRidx == mWidx)
    {
        return 0;
    }
    else if (mRidx < mWidx)
    {
        return mWidx - mRidx;
    }
    else
    {
        return mSize - (mRidx - mWidx - 1);
    }
}

int TMUtilRingBuffer::AvaliableWriteSpace()
{
    return (mSize - AvaliableReadSpace());
}

int TMUtilRingBuffer::IsEmpty()
{
    return (AvaliableReadSpace() == 0);
}

int TMUtilRingBuffer::IsFull()
{
    return (AvaliableWriteSpace() == 0);
}

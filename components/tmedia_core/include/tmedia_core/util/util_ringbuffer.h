/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_RINGBUFFER_H
#define TM_UTIL_RINGBUFFER_H

class TMUtilRingBuffer
{
public:
    TMUtilRingBuffer(int size);
    virtual ~TMUtilRingBuffer();
    int Read(char *buff, int size);
    int Write(char *buff, int size);
    int Clear();
    int AvaliableReadSpace();
    int AvaliableWriteSpace();
    int IsEmpty();
    int IsFull();
private:
    int mSize;
    int mRidx;
    int mWidx;
    char *mBuffer;
};

#endif  /* TM_UTIL_RINGBUFFER_H */

/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_PACKET_H
#define TM_PACKET_H

#include <string>
#include <tmedia_core/memory/buffer.h>
#include <tmedia_core/common/media_info.h>
#include <tmedia_core/common/audio_info.h>
#include <tmedia_core/common/data.h>
#include <tmedia_core/common/clock.h>

using namespace std;


struct TMNativePacketCtx;
typedef int (*TMPacketCtxFreeFunc)(struct TMNativePacketCtx *nativePacketCtx);

struct TMNativePacketCtx
{
public:
    static TMBuffer *CreateCtxBuffer();

public:
    int nativeId;
    TMPacketCtxFreeFunc freeFunc;
    bool internalBuffer;
    void *priv;
};

class TMPacket : public TMData
{
public:
    TMPacket();
    TMPacket(const TMPacket& packet);
    TMPacket& operator=(const TMPacket& packet);
    virtual ~TMPacket();

    void Init();

    int SetBuffer(TMBuffer *buffer);
    TMBuffer *GetBuffer() const;

    int PrepareBuffer(int size, TMBufferType bufType = TMBUFFER_TYPE_USER_MALLOC, int flags = 0);

    int SetNativePacketCtx(TMBuffer *buffer);
    TMBuffer *getNativePacketCtx() const;

    bool HasInternalBuffer();

    uint8_t *Alloc(uint32_t size);
    void     Free();

    int Ref(const TMPacket *packet);
    int MoveRef(TMPacket *packet);
    int UnRef();

    int Copy(const TMPacket *packet);
    int Copy(const uint8_t *data, int len);

    int Append(const TMPacket *packet);
    int Append(const uint8_t *data,int len);


    void Dump();

public:
    TMClock mPTS;
    TMClock mDTS;

    bool      mEOS;

    uint8_t  *mData;          /* buffer pointer */
    uint32_t  mDataMaxLength; /* buffer size in bytes. */
    uint32_t  mDataOffset;    /* Next reading or writing position in the  buffer */
    uint32_t  mDataLength;    /* Size of the actual  data in bytes */

    int       mStreamIndex;
    TMMediaInfo::CodecID mCodecID;
    void     *mPlatformPriv;

private:
    bool mInternalMalloc; //will be remove
    TMBuffer *mBuffer;
    TMBuffer *mNativePacketCtx;
};

class TMVideoPacket: public TMPacket
{
public:
    TMVideoPacket();
    virtual ~TMVideoPacket();
    void Dump();

public:
    TMMediaInfo::PictureType mPictureType;
};

class TMAudioPacket: public TMPacket
{
public:

    TMAudioPacket();
    virtual ~TMAudioPacket();
    int mSampleRate;
    AudioSampleBits_e       mSampleBits;
    AudioSampleChannels_e   mSampleChannels;
    AudioPcmDataType_e      mPcmDataType;

private:
};

#endif  /* TM_PACKET_H */

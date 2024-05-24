/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <iostream>

#include <tmedia_core/common/packet.h>
#include <tmedia_core/memory/buffer.h>
#include <tmedia_core/common/error.h>
#include <tmedia_core/common/syslog.h>

using namespace std;


static void NativePacketCtxFree(TMBuffer *buffer)
{
    struct TMNativePacketCtx *packetCtx = (struct TMNativePacketCtx *)TMBuffer_Data(buffer);

    if (packetCtx != NULL && packetCtx->freeFunc != NULL)
    {
        packetCtx->freeFunc(packetCtx);
    }

}

TMBuffer *TMNativePacketCtx::CreateCtxBuffer()
{
    int ctxBufSize = sizeof(struct TMNativePacketCtx);

    TMBuffer *ctxBuf = TMBuffer_New(ctxBufSize);

    if (ctxBuf == NULL)
    {
        printf("buffer new fail. \n");
        return NULL;
    }

    struct TMNativeFrameCtx *frameCtx = (struct TMNativeFrameCtx *)TMBuffer_Data(ctxBuf);
    memset(frameCtx, 0, ctxBufSize);
    TMBuffer_RegRefZeroCallBack(ctxBuf, NativePacketCtxFree);

    return ctxBuf;
}


TMPacket::TMPacket()
    : mData(NULL),
      mDataMaxLength(0),
      mDataOffset(0),
      mDataLength(0),
      mBuffer(NULL),
      mNativePacketCtx(NULL)
{
    mInternalMalloc = false;
}

TMPacket::TMPacket(const TMPacket& packet)
{
    mBuffer = packet.mBuffer;
    mPlatformPriv = packet.mPlatformPriv;
    mNativePacketCtx = packet.mNativePacketCtx;
    mInternalMalloc = packet.mInternalMalloc;

    mStreamIndex = packet.mStreamIndex;
    mCodecID = packet.mCodecID;
    mDataLength = packet.mDataLength;
    mDataOffset = packet.mDataOffset;
    mDataMaxLength = packet.mDataMaxLength;
    mData = packet.mData;
    mEOS = packet.mEOS;
    mDTS = packet.mDTS;
    mPTS = packet.mPTS;

    for(uint32_t i=0; i < mDataMaxLength; i++)
    {
        mData[i] = packet.mData[i];
    }

    if(packet.GetBuffer()) {
        TMBuffer_AddRef(packet.GetBuffer());
    }
}

TMPacket& TMPacket::operator=(const TMPacket& packet)
{
    if (&packet==this)
    {
        return *this;
    }
    mBuffer = packet.mBuffer;
    mPlatformPriv = packet.mPlatformPriv;
    mNativePacketCtx = packet.mNativePacketCtx;
    mInternalMalloc = packet.mInternalMalloc;

    mStreamIndex = packet.mStreamIndex;
    mCodecID = packet.mCodecID;
    mDataLength = packet.mDataLength;
    mDataOffset = packet.mDataOffset;
    mDataMaxLength = packet.mDataMaxLength;
    mData = packet.mData;
    mEOS = packet.mEOS;
    mDTS = packet.mDTS;
    mPTS = packet.mPTS;

    for(uint32_t i=0; i < mDataMaxLength; i++)
    {
        mData[i] = packet.mData[i];
    }

    if(packet.GetBuffer()) {
        TMBuffer_AddRef(packet.GetBuffer());
    }

    return *this;
}

TMPacket::~TMPacket()
{
    Free();
    UnRef();
}

uint8_t *TMPacket::Alloc(uint32_t size)
{
    bool flag = (size != mDataMaxLength);
    if (mData && flag)
        Free();
    if(!mData)
        mData = (uint8_t *)malloc(size);
    if (mData == NULL)
    {
        mDataMaxLength = 0;
        mDataLength = 0;
        mInternalMalloc = false;
        return NULL;
    }

    mDataMaxLength = size;
    mDataLength = 0;
    mInternalMalloc = true;
    return mData;
}

void TMPacket::Free()
{

    if (mInternalMalloc)
    {
        if (mData != NULL)
        {
            free(mData);
            mData = NULL;
            mDataMaxLength = 0;
        }
        mInternalMalloc = false;
    }
}

void TMPacket::Init()
{
    UnRef();

    mEOS = false;
    mData = 0;
    mDataMaxLength = 0;
    mDataOffset = 0;
    mDataLength = 0;

    mStreamIndex = 0;
    mPlatformPriv = NULL;

    mPTS.Set(TMClock::GetTime(TMCLOCK_MONOTONIC), TM_TIME_BASE_DEFAULT);
    mPTS.SetDefault(TMClock::GetTime(TMCLOCK_MONOTONIC), TM_TIME_BASE_DEFAULT);
    mDTS.Set(TMClock::GetTime(TMCLOCK_MONOTONIC), TM_TIME_BASE_DEFAULT);
    mDTS.SetDefault(TMClock::GetTime(TMCLOCK_MONOTONIC), TM_TIME_BASE_DEFAULT);
}

void TMPacket::Init(TMClockType_e type, uint32_t time_base)
{
    UnRef();

    mEOS = false;
    mData = 0;
    mDataMaxLength = 0;
    mDataOffset = 0;
    mDataLength = 0;

    mStreamIndex = 0;
    mPlatformPriv = NULL;

    TMClock_t time;
    time.time_base = TM_TIME_BASE_DEFAULT;
    time.timestamp = TMClock::GetTime(type);
    time.valid = true;

    mPTS.Set(time_base, time);
    mPTS.SetDefault(time);
    mDTS.Set(time_base, time);
    mDTS.SetDefault(time);
}

int TMPacket::SetBuffer(TMBuffer *buffer)
{
    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;

        mData =  NULL;
        mDataOffset = 0;
        mDataLength = mDataMaxLength = 0;
    }

    if (buffer != NULL)   //mmap buffer
    {
        mBuffer = TMBuffer_AddRef(buffer);

        mData = (uint8_t *)TMBuffer_Data(mBuffer);
        mDataOffset = 0;
        mDataLength = mDataMaxLength = TMBuffer_Size(mBuffer);
    }

    return TMResult::TM_OK;
}

TMBuffer *TMPacket::GetBuffer() const
{
    return mBuffer;
}

int TMPacket::PrepareBuffer(int size, TMBufferType bufType, int flags)
{
    int ret = TMResult::TM_OK;

    if (size <= 0)
    {
        printf("size need >0. \n");
        return TMResult::TM_EINVAL;
    }

    TMBuffer *buf = TMBuffer_NewEx(size, bufType, flags, NULL, NULL);
    if (buf == NULL)
    {
        printf("buffer new fail. \n");
        return TMResult::TM_ENOMEM;
    }

    ret = SetBuffer(buf);
    TMBuffer_UnRef(buf);

    return ret;
}

int TMPacket::SetNativePacketCtx(TMBuffer *buffer)
{

    if (mNativePacketCtx != NULL)   //unref native packet context first
    {
        TMNativePacketCtx *ctx = (TMNativePacketCtx *)TMBuffer_Data(mNativePacketCtx);

        if (mBuffer == NULL && ctx->internalBuffer)   /* clear data pointer */
        {
            mData = NULL;
            mDataOffset = 0;
            mDataLength = mDataMaxLength = 0;
        }

        TMBuffer_UnRef(mNativePacketCtx);
        mNativePacketCtx = NULL;
    }

    if (buffer != NULL)
    {
        mNativePacketCtx = TMBuffer_AddRef(buffer);
    }

    return TMResult::TM_OK;
}

TMBuffer *TMPacket::getNativePacketCtx() const
{
    return mNativePacketCtx;
}

bool TMPacket::HasInternalBuffer()
{
    if (mBuffer != NULL)
    {
        return true;
    }

    if (mNativePacketCtx != NULL)
    {
        TMNativePacketCtx *nfc  = (TMNativePacketCtx *)TMBuffer_Data(mNativePacketCtx);
        return nfc->internalBuffer;
    }

    return false;
}

int TMPacket::Ref(const TMPacket *packet)
{
    if (packet == NULL)
    {
        LOG_E("packet parameter can not be null. \n");
        return TMResult::TM_EINVAL;
    }

    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
    }

    if (mNativePacketCtx != NULL)
    {
        TMBuffer_UnRef(mNativePacketCtx);
        mNativePacketCtx = NULL;
    }

    bool bNeedCreateBuf = true;

    if (packet->mBuffer != NULL)
    {
        mBuffer = TMBuffer_AddRef(packet->mBuffer);
        bNeedCreateBuf = false;
    }

    if (packet->mNativePacketCtx != NULL)
    {
        TMNativePacketCtx *ctx = (TMNativePacketCtx *)TMBuffer_Data(mNativePacketCtx);

        if (ctx->internalBuffer)
        {
            bNeedCreateBuf = false;
        }

        mNativePacketCtx = TMBuffer_AddRef(packet->mNativePacketCtx);
    }


    if (bNeedCreateBuf)
    {
        mBuffer =  TMBuffer_New(packet->mDataLength);
        if (mBuffer == NULL)
        {
            printf("buffer new fail. \n");
            return -1;
        }

        mData = (uint8_t *)TMBuffer_Data(mBuffer);
        mDataOffset = 0;
        mDataLength = mDataMaxLength = packet->mDataLength;
        memcpy(mData + mDataOffset, packet->mData + mDataOffset, packet->mDataLength);
    }
    else
    {
        //copy data pointer
        mData = packet->mData;
        mDataMaxLength = packet->mDataMaxLength;
        mDataOffset = packet->mDataOffset;
        mDataLength = packet->mDataLength;
    }

    mPTS = packet->mPTS;
    mDTS = packet->mDTS;
    mEOS = packet->mEOS;

    return 0;
}

int TMPacket::UnRef()
{

    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
    }

    if (mNativePacketCtx != NULL)
    {
        TMBuffer_UnRef(mNativePacketCtx);
        mNativePacketCtx = NULL;
    }

    Free();
    mData = NULL;
    mDataOffset = 0;
    mDataMaxLength = mDataLength = 0;

    return TMResult::TM_OK;
}

int TMPacket::MoveRef(TMPacket *packet)
{
    int ret;
    if (packet == NULL)
    {
        LOG_E("packet parameter can not be null. \n");
        return TMResult::TM_EINVAL;
    }

    ret = Ref(packet);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("Ref fail.\n");
        return ret;
    }

    ret = packet->UnRef();

    return ret;
}


int TMPacket::Copy(const TMPacket *packet)
{
    if (packet == NULL)
    {
        printf("packet parameter can not be null. \n");
        return TMResult::TM_EINVAL;
    }

    if (mData == NULL || packet->mData == NULL || packet->mDataLength <= 0)
    {
        return TMResult::TM_EINVAL;
    }

    return Copy(packet->mData + packet->mDataOffset, packet->mDataLength);
}


int TMPacket::Copy(const uint8_t *data, int len)
{
    if (data == NULL || len <= 0)
    {
        printf("parameter can not be null. \n");
        return TMResult::TM_EINVAL;
    }

    if (mData == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    int leftLen = mDataMaxLength - mDataOffset;
    if (leftLen < len)
    {
        return TMResult::TM_BUF_TOO_SMALL;
    }

    memcpy(mData + mDataOffset, data, len);
    mDataLength = len;

    return TMResult::TM_OK;
}


int TMPacket::Append(const TMPacket *packet)
{

    if (packet == NULL)
    {
        printf("packet parameter can not be null. \n");
        return TMResult::TM_EINVAL;
    }

    return Append(packet->mData + packet->mDataOffset, packet->mDataLength);
}

int TMPacket::Append(const uint8_t *data, int len)
{
    if (data == NULL || len <= 0)
    {
        printf("parameter can not be null. \n");
        return TMResult::TM_EINVAL;
    }

    if (mData == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    int leftLen = mDataMaxLength - mDataOffset - mDataLength;
    if (leftLen < len)
    {
        return TMResult::TM_BUF_TOO_SMALL;
    }

    memcpy(mData + mDataOffset + mDataLength, data, len);
    mDataLength += len;

    return TMResult::TM_OK;
}

void TMPacket::Dump()
{
    cout << "   mData = " <<  static_cast<const void *>(mData) << endl;
    cout << "   mDataMaxLength = " << mDataMaxLength << endl;
    cout << "   mDataOffset = " << mDataOffset << endl;
    cout << "   mDataLength = " << mDataLength << endl;
    cout << "   mBuffer =" << static_cast<const void *>(mBuffer) << endl;
    cout << "   mNativePacketCtx =" << static_cast<const void *>(mNativePacketCtx) << endl;
    cout << "   mPTS = " << mPTS.Get().timestamp << "/" << mPTS.Get().time_base << endl;
    cout << "   mPTSDefault = " << mPTS.GetDefault().timestamp << "/" << mPTS.GetDefault().time_base << endl;
    cout << "   mDTS = " << mDTS.Get().timestamp << "/" << mDTS.Get().time_base << endl;
    cout << "   mDTSDefault = " << mDTS.GetDefault().timestamp << "/" << mDTS.GetDefault().time_base << endl;
    cout << "   mEOS =" << mEOS << endl;
}

TMVideoPacket::TMVideoPacket()
{
    mDataType = TMData::Type::STREAM_VIDEO;
    mPictureType = TMMediaInfo::PictureType::UNKNOWN;
}

TMVideoPacket::~TMVideoPacket()
{

}

void TMVideoPacket::Dump()
{
    string pictureTypeName;

    if ((uint32_t)mPictureType & (uint32_t)TMMediaInfo::PictureType::I)
        pictureTypeName += "I";
    if ((uint32_t)mPictureType & (uint32_t)TMMediaInfo::PictureType::P)
        pictureTypeName += "P";
    if ((uint32_t)mPictureType & (uint32_t)TMMediaInfo::PictureType::B)
        pictureTypeName += "B";

    cout << "   mDataType = " << TMData::Name(mDataType) << endl;
    cout << "   mPictureType = " << pictureTypeName << endl;

    TMPacket::Dump();
}

TMAudioPacket::TMAudioPacket()
{
    mDataType = TMData::Type::STREAM_AUDIO;
}

TMAudioPacket::~TMAudioPacket()
{

}


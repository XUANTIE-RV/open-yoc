/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#include <iostream>
#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <string.h>
#include <tmedia_core/util/util_inc.h>
#include <tmedia_core/common/frame.h>

using namespace std;

TMFrame::TMFrame():
    mPlanes(0)
{

    for (int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mData[i] = NULL;
    }

    mNativeFrameCtx = NULL;
    mBuffer = NULL;
    mPlatformPriv = NULL;
    mReleaseFunc = NULL;
}

TMFrame::TMFrame(const TMFrame& frame)
{
    this->mBuffer = frame.mBuffer;
    this->mReleaseFunc = frame.mReleaseFunc;
    this->mPlatformPriv = frame.mPlatformPriv;
    this->mNativeFrameCtx = frame.mNativeFrameCtx;
    this->mPlanes = frame.mPlanes;

    for(int i=0; i < TMFRAME_MAX_PLANE; i++)
    {
        this->mData[i] = frame.mData[i];
    }
    if(frame.GetBuffer()) {
        TMBuffer_AddRef(frame.GetBuffer());
    }
}

TMFrame& TMFrame::operator=(const TMFrame& frame)
{
    this->mBuffer = frame.mBuffer;
    this->mReleaseFunc = frame.mReleaseFunc;
    this->mPlatformPriv = frame.mPlatformPriv;
    this->mNativeFrameCtx = frame.mNativeFrameCtx;
    this->mPlanes = frame.mPlanes;

    for(int i=0; i < TMFRAME_MAX_PLANE; i++)
    {
        this->mData[i] = frame.mData[i];
    }

    if(frame.GetBuffer()) {
        TMBuffer_AddRef(frame.GetBuffer());
    }

    return *this;
}

TMFrame::~TMFrame()
{
    UnRef();
}

void TMFrame::Init()
{
    UnRef();
    mUsrPtr = NULL;
    mPTS.Set(TMClock::GetTime(TMCLOCK_MONOTONIC), TM_TIME_BASE_DEFAULT);
    mPTS.SetDefault(TMClock::GetTime(TMCLOCK_MONOTONIC), TM_TIME_BASE_DEFAULT);
}

void TMFrame::Init(TMClockType_e type, uint32_t time_base)
{
    UnRef();
    mUsrPtr = NULL;

    TMClock_t time;
    time.time_base = TM_TIME_BASE_DEFAULT;
    time.timestamp = TMClock::GetTime(type);
    time.valid = true;

    mPTS.Set(time_base, time);
    mPTS.SetDefault(time);
}

int TMFrame::SetBuffer(TMBuffer *buffer, int align, int stride)
{
    int ret = TMResult::TM_OK;

    if (buffer != NULL)   /* check buffer size*/
    {
        int bufSize = CalcBufferSize(align, stride);

        if (bufSize <= 0){
             return TMResult::TM_EINVAL;
        }

        if (TMBuffer_Size(buffer) < bufSize)
        {
            return TMResult::TM_EINVAL;
        }
    }

    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
        UnmapDataPointer();
    }

    if (buffer != NULL)
    {
        mBuffer = TMBuffer_AddRef(buffer);

        ret = MapDataPointer(align, stride);
        if (ret != TMResult::TM_OK) return ret;
    }

    return ret;
}

TMBuffer *TMFrame::GetBuffer() const
{
    return mBuffer;
}

int TMFrame::SetNativeFrameCtx(TMBuffer *buffer)
{
    if (mNativeFrameCtx != NULL)
    {

        TMNativeFrameCtx *ctx = (TMNativeFrameCtx *)TMBuffer_Data(mNativeFrameCtx);

        if (mBuffer == NULL && ctx->internalBuffer)
        {
            UnmapDataPointer();
        }

        TMBuffer_UnRef(mNativeFrameCtx);
        mNativeFrameCtx = NULL;
    }

    if (buffer != NULL)
    {
        mNativeFrameCtx = TMBuffer_AddRef(buffer);
    }

    return TMResult::TM_OK;
}


TMBuffer *TMFrame::GetNativeFrameCtx() const
{
    return mNativeFrameCtx;
}


int TMFrame::CopyData(const TMFrame *frame)
{
    return TMResult::TM_OK;
}

int TMFrame::CopyDataPointer(const TMFrame *frame)
{
    return TMResult::TM_OK;
}

int TMFrame::MapDataPointer(int addrAlign, int strideAlign)
{
    return TMResult::TM_OK;
}

int TMFrame::UnmapDataPointer()
{
    return TMResult::TM_OK;
}

int TMFrame::Ref(const TMFrame *frame)
{
    bool bNeedCreateBuf;
    int ret;

    bNeedCreateBuf = true;

    //Step1: copy property
    if ((ret = CopyProperty(frame)) != TMResult::TM_OK)
    {
        printf("copyProperty fail. \n");
        return ret;
    }

    // Step2: reference plane buffer
    if (mBuffer != NULL)   /* remove reference first */
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
    }

    if (frame->mBuffer != NULL)
    {
        mBuffer = TMBuffer_AddRef(frame->mBuffer);
        bNeedCreateBuf = false;
    }

    mPlanes = frame->mPlanes;

    //Step3: reference native frame context buffer
    if (mNativeFrameCtx != NULL)
    {
        TMBuffer_UnRef(mNativeFrameCtx);
        mNativeFrameCtx = NULL;
    }
    if (frame->mNativeFrameCtx != NULL)
    {
        mNativeFrameCtx = TMBuffer_AddRef(frame->mNativeFrameCtx);
        TMNativeFrameCtx *nfc  = (TMNativeFrameCtx *)TMBuffer_Data(mNativeFrameCtx);
        if (nfc->internalBuffer) bNeedCreateBuf = false;
    }

    //Step3: set mData pointer
    if (bNeedCreateBuf)
    {
        ret = PrepareBuffer();
        if (ret != TMResult::TM_OK)
        {
            printf("PrepareBuffer fail. \n");
            return ret;
        }
        ret = CopyData(frame);
        if (ret != TMResult::TM_OK)
        {
            printf("copyData fail. \n");
            return ret;
        }
    }
    else
    {
        CopyDataPointer(frame);
    }


    return TMResult::TM_OK;
}

int TMFrame::MoveRef(TMFrame *frame)
{
    if (frame == NULL)
    {
        printf("need frame parameter. \n");
        return TMResult::TM_EINVAL;
    }

    Ref(frame);
    frame->UnRef();

    return TMResult::TM_OK;
}


int TMFrame::UnRef()
{

    UnmapDataPointer();

    if (mReleaseFunc != NULL && mBuffer != NULL && TMBuffer_RefCount(mBuffer) <= 1)
    {
        mReleaseFunc(mPlatformPriv);
        mReleaseFunc = NULL;
        mPlatformPriv = NULL;
    }

    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
    }

    if (mNativeFrameCtx != NULL)
    {
        TMBuffer_UnRef(mNativeFrameCtx);
        mNativeFrameCtx = NULL;
    }

    mPlanes = 0;

    return TMResult::TM_OK;
}

bool TMFrame::HasInternalBuffer()
{
    if (mBuffer != NULL)
    {
        return true;
    }

    if (mNativeFrameCtx != NULL)
    {
        TMNativeFrameCtx *nfc  = (TMNativeFrameCtx *)TMBuffer_Data(mNativeFrameCtx);
        return nfc->internalBuffer;
    }

    return false;
}


static void NativeFrameCtxFree(TMBuffer *buffer)
{
    struct TMNativeFrameCtx *frameCtx = (struct TMNativeFrameCtx *)TMBuffer_Data(buffer);

    if (frameCtx != NULL && frameCtx->freeFunc != NULL)
    {
        frameCtx->freeFunc(frameCtx);
    }

}

TMBuffer *TMNativeFrameCtx::CreateCtxBuffer()
{
    int ctxBufSize = sizeof(struct TMNativeFrameCtx);

    TMBuffer *ctxBuf = TMBuffer_New(ctxBufSize);

    if (ctxBuf == NULL)
    {
        printf("buffer new fail. \n");
        return NULL;
    }

    struct TMNativeFrameCtx *frameCtx = (struct TMNativeFrameCtx *)TMBuffer_Data(ctxBuf);
    memset(frameCtx, 0, ctxBufSize);
    TMBuffer_RegRefZeroCallBack(ctxBuf, NativeFrameCtxFree);

    return ctxBuf;
}


TMVideoFrame::TMVideoFrame()
{
    Init();
}

TMVideoFrame::~TMVideoFrame()
{
    int ret = UnRef();
    if (ret != TMResult::TM_OK)
    {
        cout << "UnRef() failed, ret=" << ret << endl;
    }
}

TMVideoFrame& TMVideoFrame::operator=(const TMVideoFrame& frame)
{
    CopyProperty(&frame);
    TMFrame::operator=(frame);
    return *this;
}

void TMVideoFrame::Init()
{
    int i;

    TMFrame::Init();

    // TMVideoFrame extend members
    mPixelFormat = TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW;
    for (i = 0; i < TMFRAME_MAX_PLANE; i++)
        mStride[i] = 0;
    mWidth = 0;
    mHeight = 0;
    for(int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mStride[i] = 0;
        mOffset[i] = 0;
    }

    mDataType = TMData::Type::FRAME_VIDEO;
}

void TMVideoFrame::Init(TMClockType_e type, uint32_t time_base)
{
    int i;

    TMFrame::Init(type, time_base);

    // TMVideoFrame extend members
    mPixelFormat = TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW;
    for (i = 0; i < TMFRAME_MAX_PLANE; i++)
        mStride[i] = 0;
    mWidth = 0;
    mHeight = 0;
    for(int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mStride[i] = 0;
        mOffset[i] = 0;
    }

    mDataType = TMData::Type::FRAME_VIDEO;
}

int TMVideoFrame::Release()
{
    int ret;
    if (mReleaseFunc != NULL)
    {
        ret = mReleaseFunc(this);
        if (ret != TMResult::TM_OK)
        {
            cout << "mReleaseFunc() failed, ret=" << ret << endl;
            return ret;
        }
    }
    return TMResult::TM_OK;
}

int TMVideoFrame::RegisterReleaseFunc(TMFrameReleaseFunc releaseFunc)
{
    if (releaseFunc == NULL)
    {
        cout << "Input callback is NULL" << endl;
        return TMResult::TM_EINVAL;
    }

    mReleaseFunc = releaseFunc;
    return TMResult::TM_OK;
}

void TMVideoFrame::Dump()
{
    cout << "    PixelForamt: " << TMImageInfo::Name(mPixelFormat) << endl;;
    cout << "    mWidth = " << mWidth << endl;
    cout << "    mHeight = " << mHeight << endl;
    cout << "    mPlanes=" << mPlanes << endl;
    cout << "    mPTS = " << mPTS.Get().timestamp << "/" << mPTS.Get().time_base << endl;
    cout << "    mPTSDefault = " << mPTS.GetDefault().timestamp << "/" << mPTS.GetDefault().time_base << endl;

    cout << "    mData = {";
    for (uint32_t i = 0; i < mPlanes; i++)
    {
        cout << static_cast<const void *>(mData[i]) << ", ";
    }
    cout << "}" << endl;
    cout << "    mBuffer = " << static_cast<const void *>(mBuffer) << endl;
    cout << "    mNativeFrameCtx = " << static_cast<const void *>(mNativeFrameCtx) << endl;

    cout << "    mStride = {";
    for (uint32_t i = 0; i < mPlanes; i++)
    {
        cout << mStride[i] << ", ";
    }
    cout << "}" << endl;
    cout << "    mPlatformPriv=0x" << hex << mPlatformPriv << dec << endl;
    cout << "    mReleaseFunc=0x" << hex << mReleaseFunc << dec << endl;
}

int TMVideoFrame::CopyProperty(const TMFrame *frame)
{
    const TMVideoFrame *videoFrame = dynamic_cast<const TMVideoFrame *>(frame);
    if (videoFrame == NULL)
    {
        printf("Not a TMVideoFrame instance. \n");
        return TMResult::TM_EINVAL;
    }

    //copy other property
    mPixelFormat = videoFrame->mPixelFormat;
    for (int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mStride[i] = videoFrame->mStride[i];
    }
    mWidth = videoFrame->mWidth;
    mHeight = videoFrame->mHeight;
    mPlatformPriv = videoFrame->mPlatformPriv;
    mDataType = videoFrame->mDataType;
    mPTS = videoFrame->mPTS;
    mUsrPtr = videoFrame->mUsrPtr;

    return TMResult::TM_OK;

}

int TMVideoFrame::CopyData(const TMFrame *frame)
{

    const TMVideoFrame *videoFrame = dynamic_cast<const TMVideoFrame *>(frame);

    if (videoFrame == NULL)
    {
        printf("Not a TMVideoFrame instance. \n");
        return TMResult::TM_EINVAL;
    }

    if (mPlanes != frame->mPlanes)
    {
        return TMResult::TM_FORMAT_INVALID;
    }

    bool bSameLineSize = true;

    for (int i = 0; i < (int)mPlanes; i++)
    {
        if (videoFrame->mStride[i] != mStride[i])
        {
            bSameLineSize = false;
            break;
        }
    }

    TMImageInfo::PlaneInfo_t info;

    TMImageInfo::getPlaneInfo(videoFrame->mPixelFormat,
                              videoFrame->mWidth,
                              videoFrame->mHeight,
                              (int*)videoFrame->mStride,
                              &info);

    if (bSameLineSize)
    {
        for (int i = 0; i < (int)frame->mPlanes; i++)
        {
            memcpy(mData[i], frame->mData[i], info.planeSizes[i]); //copy by plane by plane
        }
    }
    else
    {
        for (int i = 0; i < (int)frame->mPlanes; i++)
        {
            int src_lineSize = videoFrame->mStride[i];
            int dst_lineSize = mStride[i];
            int cpySize = src_lineSize < dst_lineSize ? src_lineSize : dst_lineSize;

            int lineCount = info.planeSizes[i]/videoFrame->mStride[i];

            for (int j = 0; j < lineCount; j++)
            {
                memcpy(mData[i] + j * dst_lineSize, frame->mData[i] + j * src_lineSize, cpySize); //copy line by line
            }
        }
    }


    return TMResult::TM_OK;
}


int TMVideoFrame::CopyDataPointer(const TMFrame *frame)
{
    const TMVideoFrame *videoFrame = dynamic_cast<const TMVideoFrame *>(frame);

    if (videoFrame == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    mPlanes = frame->mPlanes;

    for (int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mData[i] = videoFrame->mData[i];
        mStride[i] = videoFrame->mStride[i];
    }

    return TMResult::TM_OK;
}

int TMVideoFrame::CalcBufferSize(int align, int stride) const
{
    //need set format first
    if (mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW || mWidth <= 0 || mHeight <= 0)
    {
        return -1;
    }

    if (align == 0)
    {
        align = IMAGE_ALIGN_DEFAULT;
    }

    TMImageInfo::PlaneInfo_t planeInfo;
    if (TMImageInfo::getPlaneInfo(mPixelFormat,
                                  mWidth,
                                  mHeight,
                                  &planeInfo,
                                  align,
                                  stride) != TMResult::TM_OK)
    {
        return -1;
    }

    return planeInfo.totalSize;
}


int TMVideoFrame::UnmapDataPointer()
{
    for (int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mData[i] = NULL;
        mStride[i] = 0;
    }

    mPlanes = 0;

    return TMResult::TM_OK;
}

int TMVideoFrame::MapDataPointer(int addrAlign, int strideAlign)
{
    if (mWidth <= 0 || mHeight <= 0)
    {
        return TMResult::TM_FORMAT_INVALID;
    }

    TMImageInfo::PlaneInfo_t planeInfo;
    if (TMImageInfo::getPlaneInfo(mPixelFormat,
                                  mWidth,
                                  mHeight,
                                  &planeInfo,
                                  addrAlign,
                                  strideAlign) != TMResult::TM_OK)
    {
        return TMResult::TM_FORMAT_INVALID;
    }

    if (TMBuffer_Size(mBuffer)  < planeInfo.totalSize)
    {
        return TMResult::TM_FORMAT_INVALID;
    }

    mPlanes = planeInfo.planeNum;

    int offset = 0;
    uint8_t *dataP = (uint8_t *)TMBuffer_Data(mBuffer);

    for (int i = 0; i < (int)mPlanes; i++)
    {
        mData[i] = dataP + offset;
        mStride[i] = planeInfo.stride[i];
        mOffset[i+1] = planeInfo.planeSizes[i];

        offset += planeInfo.planeSizes[i];
    }


    return TMResult::TM_OK;
}

int  TMVideoFrame::PrepareBuffer(TMBufferType bufType, int flags, int addrAlign, int strideAlign)
{

    if (mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW ||  mWidth <= 0 || mHeight <= 0)
    {
        return TMResult::TM_FORMAT_INVALID;
    }

    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
        UnmapDataPointer();
    }

    TMImageInfo::PlaneInfo_t planeInfo;

    if (strideAlign == 0)
    {
        strideAlign = IMAGE_ALIGN_DEFAULT;
    }

    if (addrAlign == 0)
    {
        addrAlign = IMAGE_ALIGN_DEFAULT;
    }

    if (TMImageInfo::getPlaneInfo(mPixelFormat,
                                  mWidth,
                                  mHeight,
                                  &planeInfo,
                                  addrAlign,
                                  strideAlign) != TMResult::TM_OK)
    {

        return TMResult::TM_FORMAT_INVALID;
    }

    mPlanes = planeInfo.planeNum;

    if (planeInfo.totalSize > 0)
    {
        TMBuffer *buffer = TMBuffer_NewEx(planeInfo.totalSize, bufType, flags, NULL, NULL);

        if (buffer == NULL)
        {
            return TMResult::TM_ENOMEM;
        }

        mBuffer = buffer;
        MapDataPointer(addrAlign, strideAlign);
    }

    return TMResult::TM_OK;
}

int TMVideoFrame::PrepareBuffer(TMBuffer *buffer, int addrAlign, int strideAlign)
{
    int ret;
    if (buffer == NULL)
        return TMResult::TM_EINVAL;

    if ((ret = SetBuffer(buffer)) != TMResult::TM_OK)
        return ret;

    if (strideAlign == 0)
    {
        strideAlign = IMAGE_ALIGN_DEFAULT;
    }

    if (addrAlign == 0)
    {
        addrAlign = IMAGE_ALIGN_DEFAULT;
    }

    MapDataPointer(addrAlign, strideAlign);

    return TMResult::TM_OK;
}

TMAudioFrame::TMAudioFrame()
{

}

TMAudioFrame::~TMAudioFrame()
{

}

void TMAudioFrame::Init()
{
    TMFrame::Init();

    mSampleBits = AUDIO_SAMPLE_BITS_UNKNOW;
    mSampleChannels = AUDIO_SAMPLE_CHANNE_UNKNOW;
    mPcmDataType = AUDIO_PCM_ACCESS_RW_UNKNOW;
    mSampleCount = 0;
    mDataType = TMData::Type::FRAME_AUDIO;
}

void TMAudioFrame::Init(TMClockType_e type, uint32_t time_base)
{
    TMFrame::Init(type, time_base);

    mSampleBits = AUDIO_SAMPLE_BITS_UNKNOW;
    mSampleChannels = AUDIO_SAMPLE_CHANNE_UNKNOW;
    mPcmDataType = AUDIO_PCM_ACCESS_RW_UNKNOW;
    mSampleCount = 0;
    mDataType = TMData::Type::FRAME_AUDIO;
}

int TMAudioFrame::CopyProperty(const TMFrame *frame)
{
    const TMAudioFrame *audioFrame = dynamic_cast<const TMAudioFrame *>(frame);

    if (audioFrame == NULL)
    {
        return TMResult::TM_EINVAL;
    }
    mSampleRate = audioFrame->mSampleRate;
    mSampleCount = audioFrame->mSampleCount;
    mSampleBits = audioFrame->mSampleBits;
    mSampleChannels = audioFrame->mSampleChannels;
    mPcmDataType = audioFrame->mPcmDataType;
    mPTS = audioFrame->mPTS;

    return TMResult::TM_OK;
}


int TMAudioFrame::CopyData(const TMFrame *frame)
{
    const TMAudioFrame *audioFrame = dynamic_cast<const TMAudioFrame *>(frame);

    if (audioFrame == NULL)
    {
        printf("Not a TMAudioFrame instance. \n");

        return TMResult::TM_EINVAL;
    }

    return TMResult::TM_OK;
}

int TMAudioFrame::CopyDataPointer(const TMFrame *frame)
{
    const TMAudioFrame *audioFrame = dynamic_cast<const TMAudioFrame *>(frame);

    if (audioFrame == NULL)
    {
        printf("Not a TMAudioFrame instance. \n");
        return TMResult::TM_EINVAL;
    }

    mPlanes = frame->mPlanes;

    for (int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mData[i] = audioFrame->mData[i];
    }

    return TMResult::TM_OK;
}

int TMAudioFrame::CalcBufferSize(int align, int stride) const
{
    return -1;
}

int TMAudioFrame::PrepareBuffer(TMBufferType bufType, int flags, int addrAlign, int strideAlign)
{
    if (mSampleBits == AUDIO_SAMPLE_BITS_UNKNOW || mSampleChannels == AUDIO_SAMPLE_CHANNE_UNKNOW || mPcmDataType == AUDIO_PCM_ACCESS_RW_UNKNOW ||  mSampleCount <= 0)
    {
        return TMResult::TM_FORMAT_INVALID;
    }

    if (mBuffer != NULL)
    {
        TMBuffer_UnRef(mBuffer);
        mBuffer = NULL;
        UnmapDataPointer();
    }

    if (addrAlign == 0)
    {
        addrAlign = AUDIO_ALIGN_DEFAULT;
    }
    int sampleChannel = (mSampleChannels == AUDIO_SAMPLE_CHANNEL_MONO ? 1 : 2);
    int sampleBits = (mSampleBits == AUDIO_SAMPLE_BITS_8BIT ? 1 : 2);
    int totalSize = AUDIO_ALIGN(sampleBits * mSampleCount, addrAlign) * sampleChannel;
    TMBuffer *buffer = TMBuffer_NewEx(totalSize, bufType, flags, NULL, NULL);

    if (buffer == NULL)
    {
        return TMResult::TM_ENOMEM;
    }

    mBuffer = buffer;
    MapDataPointer(addrAlign);

    return TMResult::TM_OK;
}

int TMAudioFrame::UnmapDataPointer()
{
    for (int i = 0; i < TMFRAME_MAX_PLANE; i++)
    {
        mData[i] = NULL;
    }

    mPlanes = 0;

    return TMResult::TM_OK;
}

int TMAudioFrame::MapDataPointer(int addrAlign, int strideAlign)
{
    if (mSampleBits == AUDIO_SAMPLE_BITS_UNKNOW || mSampleChannels == AUDIO_SAMPLE_CHANNE_UNKNOW || mPcmDataType == AUDIO_PCM_ACCESS_RW_UNKNOW ||  mSampleCount <= 0)
    {
        return TMResult::TM_FORMAT_INVALID;
    }
    if(mPcmDataType == AUDIO_PCM_ACCESS_RW_INTERLEAVED || mSampleChannels == AUDIO_SAMPLE_CHANNEL_MONO) {
        mPlanes = 1;
    } else {
        mPlanes = 2;
    }

    int offset = 0;
    uint8_t *dataP = (uint8_t *)TMBuffer_Data(mBuffer);

    int sampleBits = (mSampleBits == AUDIO_SAMPLE_BITS_8BIT ? 1 : 2);
    for (int i = 0; i < (int)mPlanes; i++)
    {
        mData[i] = dataP + offset;
        offset += AUDIO_ALIGN(mSampleCount * sampleBits, addrAlign);
    }

    return TMResult::TM_OK;
}




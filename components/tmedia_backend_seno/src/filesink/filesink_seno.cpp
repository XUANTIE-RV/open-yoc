/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <iostream>
#include <memory>
// #include <util_cvi.h>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/util/util_inc.h>

#include <tmedia_backend_seno/filesink/filesink_seno.h>
#include <tmedia_core/entity/filesink/filesink_factory.h>

using namespace std;

#define GET_STRING_PROP(propList, prop, type) do {TMProperty *p = (propList).Get(type);\
        if (p && p->Type == TMProperty::PropertyType::STRING) {prop = p->Value.String;}\
        else {cout << "error get prop=" << (uint32_t)type << endl;}} while(0)

#define GET_INT_PROP(propList, prop, type) do {TMProperty *p = (propList).Get(type);\
        if (p && p->Type == TMProperty::PropertyType::INT32) {prop = p->Value.Int;}\
        else {cout << "error get prop=" << (uint32_t)type << endl;}} while(0)

TMFileSinkSeno::TMFileSinkSeno()
    : TMFileSink("TMFileSinkSeno"),
    fd(-1), mFile(NULL), mOffset(0), mCurrentStat(STAT_CLOSED),
    mThreadRunning(false)
{
}

TMFileSinkSeno::~TMFileSinkSeno()
{
}

TMSinkPad *TMFileSinkSeno::GetSinkPad(int padID)
{
    return NULL;
}

int TMFileSinkSeno::Open(TMFileSinkParams &param, TMPropertyList *propList)
{
    if (!statCheck(STAT_OPENED)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    sem_init(&mWriteSem, 0, 0);
    pthread_mutex_init(&mWriteLock, NULL);

    if (propList) {
        int blen = 0;

        GET_STRING_PROP(*propList,  mProtocol,   TMFileSink::PropID::PROTOCOL);
        GET_STRING_PROP(*propList,  mUri,        TMFileSink::PropID::URI);
        GET_INT_PROP(*propList,     blen,        TMFileSink::PropID::BUFFER_LEN);
        myPropList = *propList;

        // cout << "fsink set buffer len: " << blen << endl;
        if (blen > 0) {
            pthread_attr_t  attr;
            
            mBuffer.Init(static_cast<uint32_t>(blen));
            int ret = pthread_attr_init(&attr);
            if (ret < 0) {
                printf("Create thread attr failed, ret = %d\n", ret);
                return -1;
            }

            mThreadRunning = true;
            ret = pthread_create(&mTid, &attr, &WriteTask, this);
            if (ret != 0) {
                printf("Create thread failed, ret = %d\n", ret);
                mThreadRunning = false;
                return -1;
            }
            pthread_setname_np(mTid, "tmfsink");
        }
    }

    // TODO: test directory write access    

    mCurrentStat = STAT_OPENED;
    return TMResult::TM_OK;
}

int TMFileSinkSeno::SetConfig(TMPropertyList &propList)
{
    GET_STRING_PROP(propList, mProtocol, TMFileSink::PropID::PROTOCOL);
    GET_STRING_PROP(propList, mUri, TMFileSink::PropID::URI);
    myPropList = propList;

    return TMResult::TM_OK;
}

int TMFileSinkSeno::GetConfig(TMPropertyList &propList)
{
    propList = myPropList;

    return TMResult::TM_OK;
}

int TMFileSinkSeno::Close()
{
    if (!statCheck(STAT_CLOSED)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    mOffset = 0;
    mCurrentStat = STAT_CLOSED;

    if (mThreadRunning) {
        mThreadRunning = false;
        sem_post(&mWriteSem);
        pthread_join(mTid, NULL);
    }

    sem_destroy(&mWriteSem);
    pthread_mutex_destroy(&mWriteLock);

    mBuffer.Deinit();
    myPropList.Reset();
    return TMResult::TM_OK;
}

int TMFileSinkSeno::Start()
{
    if (!statCheck(STAT_RUNNING)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    if (mProtocol.empty() || mUri.empty()) {
        cout << "protocol or uri empty" << endl;
        return TMResult::TM_EINVAL;
    }

    if (mProtocol == "file") 
    {
        mFile = fopen(mUri.c_str(), "wb");
        if (!mFile) 
        {
            cout << "file cannot open: " << mUri << endl;
            return TMResult::TM_EIO;
        }
    } 
    else 
    {
        cout << "error: file type not support" << mProtocol << endl;
        return TMResult::TM_EINVAL;
    }
    
    cout << "open " << mProtocol << " " << mUri << endl;
    mOffset = 0;
    mCurrentStat = STAT_RUNNING;
    return TMResult::TM_OK;
}

int TMFileSinkSeno::Stop()
{
    if (!statCheck(STAT_OPENED)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    if (mFile)
    {
        Flush();
        fclose(mFile);
        mFile = NULL;

        mOffset = 0;
        cout << "close " << mProtocol << " " << mUri << endl;
    }
    
    mCurrentStat = STAT_OPENED;
    return TMResult::TM_OK;
}

int TMFileSinkSeno::Flush()
{
    if (mBuffer.valid) {
        pthread_mutex_lock(&mWriteLock);

        uint8_t dumpBufIndex = PingpongBuffer::BUFFER_2 - mBuffer.updatingBuffer;
        PingpongBuffer::Buffer *dumpBuf = &mBuffer.buf[dumpBufIndex];
        PingpongBuffer::Buffer *updateBuf = &mBuffer.buf[mBuffer.updatingBuffer];

        /* waiting for dump finished */
        while (dumpBuf->stat == PingpongBuffer::Buffer::DUMPING) {
            usleep(10000);
        }

        if (updateBuf->usedLen) {
            size_t n = fwrite(updateBuf->data, 1, updateBuf->usedLen, mFile);

            cout << "filesink flushed: " << updateBuf->usedLen << endl;
            if (n != (size_t)updateBuf->usedLen) {
                updateBuf->usedLen = 0;
                cout << "error: file write" << endl;
                pthread_mutex_unlock(&mWriteLock);
                return TMResult::TM_EIO;
            }

            updateBuf->usedLen = 0;
        }

        pthread_mutex_unlock(&mWriteLock);
    }

    return TMResult::TM_OK;
}

int TMFileSinkSeno::SendPacket(TMPacket &pkt, int timeout)
{
    if (mCurrentStat != STAT_RUNNING)
    {
        cout << "error: sink in stat:" << mCurrentStat <<endl;
        return TMResult::TM_EINVAL;
    }

    TMResult::ID ret = TMResult::TM_OK;
    if (pkt.mData != NULL && pkt.mDataLength > 0) {
        /* if no buffer, write directly */
        if (!mBuffer.valid) {
            size_t n = fwrite(pkt.mData + pkt.mDataOffset, 1, pkt.mDataLength, mFile);
            if (n != (size_t)pkt.mDataLength) {
                cout << "error: file write" << endl;
                ret = TMResult::TM_EIO;
            }
            goto END;
        }

        pthread_mutex_lock(&mWriteLock);

        /* if has buffer, stuff it, write asyncronously when full */
        PingpongBuffer::Buffer *pbuf = &mBuffer.buf[mBuffer.updatingBuffer]; 
        uint32_t pktRemainSize;
        uint32_t pktCopySize;

        if (pkt.mDataLength + pbuf->usedLen <= pbuf->maxLen) {
            pktRemainSize   = 0;
            pktCopySize     = pkt.mDataLength;
        } else {
            pktRemainSize   = pkt.mDataLength + pbuf->usedLen - pbuf->maxLen;
            pktCopySize     = pbuf->maxLen - pbuf->usedLen;
        }

        if (pktCopySize) {
            // printf("fsink copy: copysize=%u usedlen=%u\n", pktCopySize, pbuf->usedLen);
            memcpy(pbuf->data + pbuf->usedLen, pkt.mData + pkt.mDataOffset, pktCopySize);
            pbuf->usedLen += pktCopySize;
        }

        /* buffer full, do buffer switch */
        if (pbuf->usedLen == pbuf->maxLen) {
            uint8_t newBufferIndex = PingpongBuffer::BUFFER_2 - mBuffer.updatingBuffer;

            if (mBuffer.buf[newBufferIndex].stat == PingpongBuffer::Buffer::DUMPING) {
                cout << "fsink buffer overflow" << endl;
                ret = TMResult::TM_EIO;
                pthread_mutex_unlock(&mWriteLock);
                goto END;
            }

            mBuffer.buf[mBuffer.updatingBuffer].stat = PingpongBuffer::Buffer::DUMPING;
            mBuffer.updatingBuffer = static_cast<PingpongBuffer::Index>(newBufferIndex);
            sem_post(&mWriteSem);
        }

        /* if remaining data exists, copy them to mBuffer */
        if (pktRemainSize) {
            PingpongBuffer::Buffer *updatingBuf = &mBuffer.buf[mBuffer.updatingBuffer]; 
            if (pktRemainSize > updatingBuf->maxLen - updatingBuf->usedLen) {
                cout << "fsink buffer remaining overflow" << endl;
                ret = TMResult::TM_EIO;
                pthread_mutex_unlock(&mWriteLock);
                goto END;
            }

            // printf("fsink copy: remainsize=%u usedlen=%u\n", pktRemainSize, updatingBuf->usedLen);
            memcpy( updatingBuf->data + updatingBuf->usedLen,
                    pkt.mData + pkt.mDataOffset + pkt.mDataLength - pktRemainSize,
                    pktRemainSize);
            updatingBuf->usedLen += pktRemainSize;
        }

        mOffset += pkt.mDataLength;
        pthread_mutex_unlock(&mWriteLock);
    }

END:
    pkt.UnRef();
    return ret;
}

void *TMFileSinkSeno::WriteTask(void *arg)
{
    TMFileSinkSeno *pthis = reinterpret_cast<TMFileSinkSeno *>(arg);
    uint8_t dumpBufIndex;
    PingpongBuffer::Buffer *dumpBuf;
    int ret;

    while (pthis->mThreadRunning) {
        ret = sem_wait(&pthis->mWriteSem);
        if (ret < 0) {
            continue;
        }
        
        if (!pthis->mThreadRunning) {
            break;
        }

        dumpBufIndex = PingpongBuffer::BUFFER_2 - pthis->mBuffer.updatingBuffer;
        dumpBuf = &pthis->mBuffer.buf[dumpBufIndex];

        if (dumpBuf->stat != PingpongBuffer::Buffer::DUMPING) {
            cout << "fsink error buffer stat" << endl;
            continue;
        }


        // cout << "writetask write bytes: " << dumpBuf->usedLen << endl; 
        size_t n = fwrite(dumpBuf->data, 1, dumpBuf->usedLen, pthis->mFile);
        if (n != (size_t)dumpBuf->usedLen) {
            cout << "error: file write" << endl;
        }

        dumpBuf->usedLen = 0;
        dumpBuf->stat   = PingpongBuffer::Buffer::IDLE;
    }

    return NULL;
}

bool TMFileSinkSeno::statCheck(STAT stat)
{
    switch (mCurrentStat) 
    {
        case STAT_CLOSED:
        if (stat == STAT_OPENED)
            return true;
        break;

        case STAT_OPENED:
        if (stat == STAT_CLOSED || stat == STAT_RUNNING)
            return true;
        break;

        case STAT_RUNNING:
        if (stat == STAT_OPENED)
            return true;
        break;
    }

    return false;
}

void TMFileSinkSeno::PingpongBuffer::Init(uint32_t size) {
    buf[BUFFER_1].stat       = Buffer::IDLE;
    buf[BUFFER_1].maxLen     = size;
    buf[BUFFER_1].usedLen    = 0;
    buf[BUFFER_1].data       = size ? (uint8_t *)malloc(size) : NULL;

    buf[BUFFER_2].stat       = Buffer::IDLE;
    buf[BUFFER_2].maxLen     = size;
    buf[BUFFER_2].usedLen    = 0;
    buf[BUFFER_2].data       = size ? (uint8_t *)malloc(size) : NULL;

    updatingBuffer           = BUFFER_1;

    valid = true;
}

void TMFileSinkSeno::PingpongBuffer::Deinit()
{
    if (valid) {
        buf[BUFFER_1].maxLen     = 0;
        buf[BUFFER_1].usedLen    = 0;
        buf[BUFFER_2].maxLen     = 0;
        buf[BUFFER_2].usedLen    = 0;

        usleep(50000);
        free(buf[BUFFER_1].data);
        free(buf[BUFFER_2].data);

        updatingBuffer     = BUFFER_1;

        valid = false;
    }
}


REGISTER_FILESINK_CLASS(TMMediaInfo::MediaFileType::SIMPLE, TMFileSinkSeno)

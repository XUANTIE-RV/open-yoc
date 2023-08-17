/**
 * @file record.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SROUCE_CACHE_H
#define CX_SROUCE_CACHE_H

#include <vector>
#include <pthread.h>
#include <cx/common/frame.h>
#include <cx/common/type.h>
#include <cx/source/source_interface.h>
#include <cx/common/log.h>

namespace cx {
namespace source {

/**
 * cache for only one producer, and one consumer
 */

template <class T1,class T2>
class Cache final {
public:
    Cache(const uint32_t size) : mSize(size), mFameBuffer(mSize, nullptr), mPacketBuffer(mSize, nullptr), mWptr(0), mRptr(0) 
    {
        pthread_condattr_t  cond_attr;
        pthread_condattr_init(&cond_attr);
        pthread_cond_init(&signal, &cond_attr);
        pthread_mutex_init(&lock, 0);
        pthread_condattr_destroy(&cond_attr);
    }
    ~Cache() 
    {
        pthread_cond_destroy(&signal);
        pthread_mutex_destroy(&lock);
    }

    int GetFrame(shared_ptr<T1> &frame) 
    {
        if (mWptr == mRptr) {
            return -1;
        }

        pthread_mutex_lock(&lock);
        frame = std::move(mFameBuffer[mRptr]);
        mRptr = (mRptr + 1) % mSize;
        pthread_mutex_unlock(&lock);

        return 0;
    }

    int GetPacket(shared_ptr<T2> &pkt) 
    {
        if (mWptr == mRptr) {
            return -1;
        }

        pthread_mutex_lock(&lock);
        pkt = std::move(mPacketBuffer[mRptr]);
        mRptr = (mRptr + 1) % mSize;
        pthread_mutex_unlock(&lock);

        return 0;
    }

    // using std::move, so frame cannot be used after return
    int AddFrame(shared_ptr<T1> &&frame)
    {
        if ((mWptr + 1) % mSize == mRptr) {
            CX_LOGD("cxcache", "cache full size:%d", mSize);
            //TODO: clean cache, cache newest frame?

            return -1;
        }
        // if(AvailableWriteSpace() < mSize/2) {
        //     CX_LOGE("cxcache", "cache avaliable:%d", AvailableWriteSpace());
        // }
        pthread_mutex_lock(&lock);
        mFameBuffer[mWptr] = std::move(frame);
        mWptr = (mWptr + 1) % mSize;
        pthread_cond_signal(&signal);
        pthread_mutex_unlock(&lock);

        return 0;
    }

    int AddPacket(shared_ptr<T2> &&pkt)
    {
        if ((mWptr + 1) % mSize == mRptr) {
            CX_LOGD("cxcache", "cache full size:%d", mSize);
            //TODO: clean cache, cache newest pkt?

            return -1;
        }
        // if(AvailableWriteSpace() < mSize/2) {
        //     CX_LOGE("cxcache", "cache avaliable:%d", AvailableWriteSpace());
        // }
        pthread_mutex_lock(&lock);
        mPacketBuffer[mWptr] = std::move(pkt);
        mWptr = (mWptr + 1) % mSize;
        pthread_cond_signal(&signal);
        pthread_mutex_unlock(&lock);

        return 0;
    }

    int Clear(void)
    {
        pthread_mutex_lock(&lock);

        while (mWptr != mRptr) {
            mFameBuffer[mRptr] = nullptr;
            mPacketBuffer[mRptr] = nullptr;
            mRptr = (mRptr + 1) % mSize;
        }

        pthread_mutex_unlock(&lock);

        return 0;
    }

    uint32_t AvailableReadSpace()
    {
        if (mRptr == mWptr) {
            return 0;
        } else if (mRptr < mWptr) {
            return mWptr - mRptr;
        } else {
            return mSize - (mRptr - mWptr - 1);
        }
    }
    uint32_t AvailableWriteSpace()
    {
        return (mSize - AvailableReadSpace());
    }
private:
    const uint32_t                mSize;
    std::vector<shared_ptr<T1>>   mFameBuffer;
    std::vector<shared_ptr<T2>>   mPacketBuffer;
    uint32_t mWptr;
    uint32_t mRptr;
    pthread_cond_t signal;
    pthread_mutex_t lock;
};

}
} 



#endif /* CX_SROUCE_CACHE_H */


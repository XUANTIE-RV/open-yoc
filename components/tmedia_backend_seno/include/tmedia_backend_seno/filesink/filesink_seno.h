/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FILESINK_SENO_H
#define TM_FILESINK_SENO_H

#include <string>
#include <pthread.h>
#include <semaphore.h>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>
#include <tmedia_core/entity/filesink/filesink.h>

class TMFileSinkSeno : public TMFileSink
{
public:
    TMFileSinkSeno();
    virtual ~TMFileSinkSeno();

    // TMSinkEntity interface
    virtual TMSinkPad *GetSinkPad(int padID = 0);

    // TMVpss interface
    virtual int Open(TMFileSinkParams &param, TMPropertyList *propList = NULL);
    virtual int SetConfig(TMPropertyList &propList);
    virtual int GetConfig(TMPropertyList &propList);
    virtual int Start();
    virtual int Stop();
    virtual int Close();
    virtual int Flush();

    virtual int SendPacket(TMPacket &pkt, int timeout);

private:
    TMPropertyList myPropList;

    typedef enum {
        STAT_CLOSED = 1,
        STAT_OPENED,
        STAT_RUNNING,
    } STAT;

    struct PingpongBuffer {
        PingpongBuffer() : valid(false) {}

        enum Index : uint8_t {
            BUFFER_1        = 0,
            BUFFER_2        = 1,
        };

        struct Buffer {
            enum Stat : uint8_t {
                IDLE = 0,
                DUMPING,        // in the middle of write disk
            };

            Stat            stat;
            uint8_t         *data;
            uint32_t        maxLen;     // total size of data
            uint32_t        usedLen;    // used len of data
        };
        
        void Init(uint32_t size);
        void Deinit();

        Buffer          buf[2];
        Index           updatingBuffer;

        bool            valid;
    };

    // TMFileSinkParams mParams;

    string          mProtocol;
    string          mUri;
    int             fd;
    FILE            *mFile;
    uint32_t        mOffset;
    STAT            mCurrentStat;
    PingpongBuffer  mBuffer;

    pthread_t       mTid;
    sem_t           mWriteSem;
    bool            mThreadRunning;
    pthread_mutex_t mWriteLock;
    static void *WriteTask(void *arg);

    bool statCheck(STAT stat);
};

#endif  /* TM_FILESINK_SENO_H */

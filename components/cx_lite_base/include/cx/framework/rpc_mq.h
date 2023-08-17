/**
 * @file rpc_mq.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_FRAMEWORK_RPC_MQ_H
#define CX_FRAMEWORK_RPC_MQ_H

#include <memory>
#include <string>
#include <functional>
#include <atomic>

// #include <semaphore.h>
#include <mqueue.h>
#include <cx/common/thread.h>

namespace cx {
namespace framework {


struct SvrProxyMsg {
    const char *funcName;
    void *args;
    // void *cb;
    // void *cb_priv;
};

class ServiceProxyClient {
public:
    ServiceProxyClient();
    ~ServiceProxyClient();

    int SendRequest(SvrProxyMsg &msg);

    mqd_t GetMq(void) {return mq;}


private:
    static std::atomic<uint32_t> mTaskId;
    mqd_t   mq;
    bool    mInited;
    static int count;

};

class ServiceProxyServer {
public:
    using CallbackFunc = std::function<void (SvrProxyMsg &msg)>;

    ServiceProxyServer(const mqd_t &mq);
    ~ServiceProxyServer();

    int Init(const CallbackFunc &func, const ThreadAttributes *attr = NULL);         // base thread
    int Deinit();

    // int RecvRequest();

private:
    static void     *Task(void *args);
    mqd_t           mq;
    CallbackFunc    mCallback;
    bool            mThreadRunning;
    pthread_t       mTid;
};



}
}
#endif /* CX_FRAMEWORK_RPC_MQ_H */


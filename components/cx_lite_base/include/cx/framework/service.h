/**
 * @file service.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_FRAMEWORK_SERVICE_H
#define CX_FRAMEWORK_SERVICE_H

#include <memory>
#include <string>
#include <map>
#include <cx/framework/rpc_mq.h>
#include <cx/common/thread.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>

namespace cx {
namespace framework {

struct ServiceApiUnkown;

class ServiceManager {
public:

    using AsyncTaskFunc = std::function<void(void)>;

    ServiceManager();
    virtual ~ServiceManager() = default;

    int Init();
    int Deinit();

    int SendCallback(const AsyncTaskFunc &tsk);

    virtual int ServiceInit() = 0;
    virtual int ServiceDeinit() = 0;

private:
    static void *AsyncTaskThread(void *arg);
    static std::atomic<uint32_t> mServiceId;

    pthread_t       mTid;
    bool            mCbTaskRunning;
    mqd_t           mq;
};

class ServiceBackend {
public:
    using ServiceApiUnkownPtr = ServiceApiUnkown *;

    ServiceBackend(const std::string &name);
    virtual ~ServiceBackend() = default;

    // service register there own api
    void RegisterServiceFunc(const std::string &name, ServiceApiUnkownPtr api);

    int Init(const std::string &svrname, ThreadAttributes *threadAttrs, mqd_t mq);
    int Deinit(void);

    bool IsApiExist(const std::string &apiName);

    int WaitApiFinish(const std::string &apiName, int & ret, int timeout_ms);

    void SetServiceManager(ServiceManager *manager) 
    {
        mManager = manager;
    }

protected:
    virtual int ServiceInit() = 0;
private:
    int Call(const std::string &name, void *args);

    bool mInited;
    const std::string mSvrname;
    std::map<const std::string, ServiceApiUnkownPtr> mServiceMap;

    std::unique_ptr<ServiceProxyServer> mProxyServer;

    ServiceManager          *mManager;
};

struct ServiceApiUnkown {
    // sem_t           semArgs;
    sem_t               sem;
    const std::string   apiName;
    SvrProxyMsg         msg;
    int                 funcRet;

    ServiceBackend      *backend;
    
    virtual int Invoke(ServiceManager *svrManager, void *args) = 0;

    void SetManager(ServiceManager *manager)
    {
        if (backend) {
            backend->SetServiceManager(manager);
        }
    }

    ServiceApiUnkown()
    : backend(nullptr)
    {
        sem_init(&sem, 0, 0);
    }

    virtual ~ServiceApiUnkown() = default;
};


}
}

#define API_CLASS_NAME(n)           Api_##n
#define REGISTER_SERVICE_API(api)   RegisterServiceFunc(""#api"", new API_CLASS_NAME(api))
#define DEFINE_API_IMPL(api)        struct API_CLASS_NAME(api) : public ServiceApiUnkown

#endif /* CX_FRAMEWORK_SERVICE_H */


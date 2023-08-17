/**
 * @file client.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_FRAMEWORK_CLIENT_H
#define CX_FRAMEWORK_CLIENT_H

#include <memory>
#include <string>
#include <cx/framework/service.h>
#include <pthread.h>

namespace cx {
namespace framework {

class ServiceClient final {
public:
    ServiceClient(ServiceBackend *manager);

    int Init(const char *svrname, ThreadAttributes *proxyAttrs);
    int Deinit(void);

    int Request(const char *apiName, void *args, int timeout);

private:
    bool            mInited;
    pthread_mutex_t mRequestLock;

    std::unique_ptr<ServiceBackend> mSvrManager;
    std::unique_ptr<ServiceProxyClient> mProxyClient;
};

}
}
#endif /* CX_FRAMEWORK_CLIENT_H */


/**
 * @file sdk_init.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_INIT_SDK_INIT_H
#define CX_INIT_SDK_INIT_H

#include <memory>
#include <vector>
#include <functional>

namespace cx {
namespace init {

class IoInit {
public:
    using IoFunc = std::function<void (void)>;
    static void RegisterIoFunc(IoFunc f) 
    {
        funcs.push_back(f);
    }

    static void Start() 
    {
        for (auto &f : funcs) {
            f();
        }
    }

private:
    static std::vector<IoFunc> funcs;
};


}
}

#define REGISTER_SERVICE_INIT_IO(service_name, io_func)     \
    class ServiceIoInit##service_name final { \
    public: \
        ServiceIoInit##service_name() \
        {   \
            cx::init::IoInit::RegisterIoFunc(io_func); \
        } \
    }; \
const static ServiceIoInit##service_name gRegSvrIo##service_name;

#endif /* CX_INIT_SDK_INIT_H */


/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_VPSS_FACTORY_H
#define TM_VPSS_FACTORY_H

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/vpss/vpss.h>

using namespace std;

class TMVpssFactory
{
public:
    static TMVpss *CreateEntity();

    static void RegisterClass(class_new_t func)
    {
        mVpssClass() = func;
    }
    static class_new_t &mVpssClass()
    {
        static class_new_t VpssClass;
        return VpssClass;
    }
};

class VpssRegister
{
public:
    VpssRegister(class_new_t func)
    {
        TMVpssFactory::RegisterClass(func);
    }
};

#define REGISTER_VPSS_CLASS(class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const VpssRegister reg; \
    };\
const VpssRegister class_name##Register::reg(class_name##Register::NewInstance);

#endif  /* TM_VPSS_FACTORY_H */

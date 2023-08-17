/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_VPM_FACTORY_H
#define TM_VPM_FACTORY_H

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/vpm/vpm.h>

using namespace std;

class TMVpmFactory
{
public:
    static TMVpm *CreateEntity();

    static void RegisterClass(class_new_t func)
    {
        mVpmClass() = func;
    }
    static class_new_t &mVpmClass()
    {
        static class_new_t VpmClass;
        return VpmClass;
    }
};

class VpmRegister
{
public:
    VpmRegister(class_new_t func)
    {
        TMVpmFactory::RegisterClass(func);
    }
};

#define REGISTER_VPM_CLASS(class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const VpmRegister reg; \
    };\
const VpmRegister class_name##Register::reg(class_name##Register::NewInstance);

#endif  /* TM_VPM_FACTORY_H */

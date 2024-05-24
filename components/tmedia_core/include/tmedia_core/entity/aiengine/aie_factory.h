/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef AIE_FACTORY_H
#define AIE_FACTORY_H

#include <tmedia_core/entity/entity.h>
#include <tmedia_core/entity/aiengine/image_proc.h>
#include <tmedia_core/entity/aiengine/interpreter.h>
#include <tmedia_core/entity/aiengine/fce.h>

using namespace std;

class ImageProcFactory
{
  public:
    static TMImageProc *CreateEntity(string class_name = "");

    static void RegisterClass(string class_name, class_new_t func)
    {
        vector<pair<string, class_new_t>>::iterator it;
        for (it = mImageProcClass().begin(); it != mImageProcClass().end(); it++)
        {
            if (it->first == class_name)
            {
                return;
            }
        }
        mImageProcClass().push_back(make_pair(class_name, func));
    }
    static vector<pair<string, class_new_t>> &mImageProcClass()
    {
        static vector<pair<string, class_new_t>> ImageProcClass;
        return ImageProcClass;
    }
};

class ImageProcRegister
{
  public:
    ImageProcRegister(string class_name, class_new_t func)
    {
        ImageProcFactory::RegisterClass(class_name, func);
    }
};

#define REGISTER_IMAGE_PROC_CLASS(class_name)                                                                          \
    class class_name##Register                                                                                         \
    {                                                                                                                  \
      public:                                                                                                          \
        static void *NewInstance()                                                                                     \
        {                                                                                                              \
            return new class_name;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
      private:                                                                                                         \
        static const ImageProcRegister reg;                                                                            \
    };                                                                                                                 \
    const ImageProcRegister class_name##Register::reg(TM_STR(class_name), class_name##Register::NewInstance);

class OSDFactory
{
  public:
    static TMOsd *CreateEntity(string class_name = "");

    static void RegisterClass(string class_name, class_new_t func)
    {
        vector<pair<string, class_new_t>>::iterator it;
        for (it = mOSDClass().begin(); it != mOSDClass().end(); it++)
        {
            if (it->first == class_name)
            {
                return;
            }
        }
        mOSDClass().push_back(make_pair(class_name, func));
    }
    static vector<pair<string, class_new_t>> &mOSDClass()
    {
        static vector<pair<string, class_new_t>> OSDClass;
        return OSDClass;
    }
};

class OSDRegister
{
  public:
    OSDRegister(string class_name, class_new_t func)
    {
        OSDFactory::RegisterClass(class_name, func);
    }
};

#define REGISTER_OSD_CLASS(class_name)                                                                                 \
    class class_name##Register                                                                                         \
    {                                                                                                                  \
      public:                                                                                                          \
        static void *NewInstance()                                                                                     \
        {                                                                                                              \
            return new class_name;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
      private:                                                                                                         \
        static const OSDRegister reg;                                                                                  \
    };                                                                                                                 \
    const OSDRegister class_name##Register::reg(TM_STR(class_name), class_name##Register::NewInstance);

class InterpreterFactory
{
  public:
    static TMInterpreter *CreateEntity(string class_name = "");

    static void RegisterClass(string class_name, class_new_t func)
    {
        vector<pair<string, class_new_t>>::iterator it;
        for (it = mInterpreterClass().begin(); it != mInterpreterClass().end(); it++)
        {
            if (it->first == class_name)
            {
                return;
            }
        }
        mInterpreterClass().push_back(make_pair(class_name, func));
    }
    static vector<pair<string, class_new_t>> &mInterpreterClass()
    {
        static vector<pair<string, class_new_t>> InterpreterClass;
        return InterpreterClass;
    }
};

class InterpreterRegister
{
  public:
    InterpreterRegister(string class_name, class_new_t func)
    {
        InterpreterFactory::RegisterClass(class_name, func);
    }
};

#define REGISTER_INTERPRETER_CLASS(class_name)                                                                         \
    class class_name##Register                                                                                         \
    {                                                                                                                  \
      public:                                                                                                          \
        static void *NewInstance()                                                                                     \
        {                                                                                                              \
            return new class_name;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
      private:                                                                                                         \
        static const InterpreterRegister reg;                                                                          \
    };                                                                                                                 \
    const InterpreterRegister class_name##Register::reg(TM_STR(class_name), class_name##Register::NewInstance);

class FCEFactory
{
  public:
    static FCE *CreateEntity();

    static void RegisterClass(class_new_t func)
    {
        mFCEClass() = func;
    }
    static class_new_t &mFCEClass()
    {
        static class_new_t FCEClass;
        return FCEClass;
    }
};

class FCERegister
{
  public:
    FCERegister(class_new_t func)
    {
        FCEFactory::RegisterClass(func);
    }
};

#define REGISTER_FCE_CLASS(class_name)                                                                                 \
    class class_name##Register                                                                                         \
    {                                                                                                                  \
      public:                                                                                                          \
        static void *NewInstance()                                                                                     \
        {                                                                                                              \
            return new class_name;                                                                                     \
        }                                                                                                              \
                                                                                                                       \
      private:                                                                                                         \
        static const FCERegister reg;                                                                                  \
    };                                                                                                                 \
    const FCERegister class_name##Register::reg(class_name##Register::NewInstance);

#endif // AIE_FACTORY_H

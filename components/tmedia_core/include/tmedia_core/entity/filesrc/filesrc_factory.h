/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <string>
#include <map>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/filesrc/filesrc.h>

#ifndef TM_FILESRC_FACTORY_H
#define TM_FILESRC_FACTORY_H

class TMFileSrcFactory
{
public:
    TMFileSrcFactory() {};
    ~TMFileSrcFactory() {};

    static TMFileSrc *CreateEntity(TMMediaInfo::MediaFileType formatID);
    static void RegisterClass(TMMediaInfo::MediaFileType formatID, class_new_t func)
    {
        mFileSrcClasses()[formatID] = func;
    }
    static std::map<TMMediaInfo::MediaFileType, class_new_t> &mFileSrcClasses()
    {
        static std::map<TMMediaInfo::MediaFileType, class_new_t> FileSrcClasses;
        return FileSrcClasses;
    }
};

class FileSrcRegister
{
public:
    FileSrcRegister(TMMediaInfo::MediaFileType formatID, class_new_t func)
    {
        TMFileSrcFactory::RegisterClass(formatID, func);
    }
};

#define REGISTER_FILESRC_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const FileSrcRegister reg; \
    };\
const FileSrcRegister class_name##Register::reg(id, class_name##Register::NewInstance);


#endif  // TM_FILESRC_FACTORY_H

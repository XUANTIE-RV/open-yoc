/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FILESINK_FACTORY_H
#define TM_FILESINK_FACTORY_H

#include <string>
#include <map>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/filesink/filesink.h>

class TMFileSinkFactory
{
public:
    TMFileSinkFactory() {};
    ~TMFileSinkFactory() {};

    static TMFileSink *CreateEntity(TMMediaInfo::MediaFileType formatID);
    static void RegisterClass(TMMediaInfo::MediaFileType formatID, class_new_t func)
    {
        mFileSinkClasses()[formatID] = func;
    }
    static std::map<TMMediaInfo::MediaFileType, class_new_t> &mFileSinkClasses()
    {
        static std::map<TMMediaInfo::MediaFileType, class_new_t> FileSinkClasses;
        return FileSinkClasses;
    }
};

class FileSinkRegister
{
public:
    FileSinkRegister(TMMediaInfo::MediaFileType formatID, class_new_t func)
    {
        TMFileSinkFactory::RegisterClass(formatID, func);
    }
};

#define REGISTER_FILESINK_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const FileSinkRegister reg; \
    };\
const FileSinkRegister class_name##Register::reg(id, class_name##Register::NewInstance);


#endif  /* TM_FILESINK_FACTORY_H */

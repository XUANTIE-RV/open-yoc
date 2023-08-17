/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_PARSER_FACTORY_H
#define TM_PARSER_FACTORY_H

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>
#include <tmedia_core/entity/parser/parser.h>

using namespace std;

class TMParserFactory
{
public:
    TMParserFactory() {};
    ~TMParserFactory() {};

    static TMParser* CreateEntity(TMMediaInfo::CodecID codecID, string class_name = "");
    static void RegisterClass(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        mParserClasses()[codecID] = make_pair(class_name, func);
    }
    static map<TMMediaInfo::CodecID, pair<string, class_new_t>> &mParserClasses()
    {
        static map<TMMediaInfo::CodecID, pair<string, class_new_t>> ParserClasses;
        return ParserClasses;
    }
};

class ParserRegister
{
public:
    ParserRegister(TMMediaInfo::CodecID codecID, string class_name, class_new_t func)
    {
        TMParserFactory::RegisterClass(codecID, class_name, func);
    }
};

#define REGISTER_PARSER_CLASS(id, class_name) \
    class class_name##Register { \
    public: \
        static void* NewInstance() { \
            return new class_name; \
        } \
    private: \
        static const ParserRegister reg; \
    };\
const ParserRegister class_name##Register::reg(id, TM_STR(class_name), class_name##Register::NewInstance);

#endif  // TM_PARSER_FACTORY_H

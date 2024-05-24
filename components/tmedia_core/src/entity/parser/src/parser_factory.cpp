/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/entity/parser/parser.h>
#include <tmedia_core/entity/parser/parser_factory.h>

using namespace std;

TMParser *TMParserFactory::CreateEntity(TMMediaInfo::CodecID codecID, string class_name)
{
    map<TMMediaInfo::CodecID, pair<string, class_new_t>>::iterator it;
    if (class_name.empty()) {
        it = mParserClasses().find(codecID);
        if (it != mParserClasses().end()) {
            return (TMParser*)it->second.second();
        } else {
            cout << "Parser '" << TMMediaInfo::Name(codecID) << "' not supported" << endl;
            return NULL;
        }
    } else {
        for (it = mParserClasses().begin(); it != mParserClasses().end(); it++) {
            if ((it->first == codecID) && (it->second.first == class_name))
                return (TMParser*)it->second.second();
        }
    }
    cout << "Parser '" << class_name << "' not supported" << endl;
    return NULL;
}

/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/entity/filesink/filesink_factory.h>

using namespace std;

TMFileSink *TMFileSinkFactory::CreateEntity(TMMediaInfo::MediaFileType formatID)
{
    auto it = mFileSinkClasses().find(formatID);
    if (it != mFileSinkClasses().end()) {
        return static_cast<TMFileSink *>(it->second());
    } else {
        cout << "FileSink Type '" << TMMediaInfo::Name(formatID) << "' not supported" << endl;
        return NULL;
    }
}

/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>

#include <tmedia_core/entity/filesrc/filesrc_factory.h>

using namespace std;

TMFileSrc *TMFileSrcFactory::CreateEntity(TMMediaInfo::MediaFileType formatID)
{
    auto it = mFileSrcClasses().find(formatID);
    if (it != mFileSrcClasses().end()) {
        return static_cast<TMFileSrc *>(it->second());
    } else {
        cout << "FileSrc Type '" << TMMediaInfo::Name(formatID) << "' not supported" << endl;
        return NULL;
    }
}

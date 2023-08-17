/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_SAVE_H
#define TM_UTIL_SAVE_H

#include <string>
#include <tmedia_core/common/common_inc.h>

using namespace std;

class TMUtilSave
{
public:
    static int PgmPicture(TMVideoFrame &frame, string &filename);
    static int Picture(TMVideoFrame &frame, const char *filename, bool append=false);
    static int Picture(TMVideoFrame &frame, string &filename, bool append=false);
    static int Packet(TMPacket &pkt, string &filename);
};

#endif  /* TM_UTIL_SAVE_H */

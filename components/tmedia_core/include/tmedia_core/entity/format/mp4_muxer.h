/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_MP4_MUXER_H
#define TM_MP4_MUXER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/media_info.h>
#include <tmedia_core/entity/parser/parser.h>

using namespace std;

class TMMp4Muxer : public TMFormatMuxer
{
public:
    TMMp4Muxer();
    ~TMMp4Muxer();

protected:

private:
};

#endif  /* TM_MP4_MUXER_H */

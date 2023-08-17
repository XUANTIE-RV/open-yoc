/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_CODEC_H
#define TM_CODEC_H

#include <string>
#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/codec/codec_params.h>

using namespace std;

class TMCodec
{
public:
    TMCodec();
    virtual ~TMCodec();

    TMMediaInfo::CodecID GetCodecID();
    string GetCodecName();
    TMMediaInfo::WorkMode GetWorkMode();

    virtual int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Flush() = 0;
    virtual int Stop()  = 0;
    virtual int Close() = 0;

protected:
    TMMediaInfo::CodecID  mCodecID;
    TMMediaInfo::Type     mMediaType;
    TMMediaInfo::WorkMode mWorkMode;
};

#endif  /* TM_CODEC_H */

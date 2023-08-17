/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FILESRC_H
#define TM_FILESRC_H

#include <string>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>

class TMFileSrc : public TMSrcEntity
{
public:
    enum class PropID : uint32_t
    {    
        PROTOCOL,                   // char *, "file", "mem"
        URI,                        // char *, file location uri
    };

    struct TMFileSrcParams
    {
    };

    TMFileSrc(string name) : TMSrcEntity(name) {}
    virtual ~TMFileSrc() = default;

    // TMSinkEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;

    virtual int Open(TMFileSrcParams &param, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Stop() = 0;
    virtual int Close() = 0;
    virtual uint32_t GetSize() = 0;
    virtual int Seek(uint32_t offset) = 0;
    virtual int RecvPacket(TMPacket &pkt, int timeout) = 0;
};


#endif  /* TM_FILESRC_H */

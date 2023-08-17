/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FILESINK_H
#define TM_FILESINK_H

#include <string>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>

class TMFileSink : public TMSinkEntity
{
public:
    enum class PropID : uint32_t
    {        
        PROTOCOL,                   // char *, "file", "mem"
        URI,                        // char *, file location uri
        BUFFER_LEN,                 // uint32_t, buffer len, 0 for no buffer
    };

    struct TMFileSinkParams
    {
    };

    TMFileSink(string name) : TMSinkEntity(name) {}
    virtual ~TMFileSink() = default;

    // TMSinkEntity interface
    virtual TMSinkPad *GetSinkPad(int padID = 0) = 0;

    virtual int Open(TMFileSinkParams &param, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Stop() = 0;
    virtual int Close() = 0;
    virtual int Flush() = 0;
    virtual int SendPacket(TMPacket &pkt, int timeout) = 0;
};


#endif  /* TM_FILESINK_H */

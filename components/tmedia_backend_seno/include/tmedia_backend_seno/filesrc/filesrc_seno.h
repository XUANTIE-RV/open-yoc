/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_FILESRC_SENO_H
#define TM_FILESRC_SENO_H

#include <string>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>
#include <tmedia_core/entity/filesrc/filesrc.h>

class TMFileSrcSeno : public TMFileSrc
{
public:
    TMFileSrcSeno();
    virtual ~TMFileSrcSeno();

    // TMSrcEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0);

    // TMVpss interface
    virtual int Open(TMFileSrcParams &param, TMPropertyList *propList = NULL);
    virtual int SetConfig(TMPropertyList &propList);
    virtual int GetConfig(TMPropertyList &propList);
    virtual int Start();
    virtual int Stop();
    virtual int Close();
    virtual uint32_t GetSize();
    virtual int Seek(uint32_t offset);

    virtual int RecvPacket(TMPacket &pkt, int timeout);

private:
    TMPropertyList myPropList;

    typedef enum {
        STAT_CLOSED = 1,
        STAT_OPENED,
        STAT_RUNNING,
    } STAT;

    string mProtocol;
    string mUri;
    int fd;
    FILE *mFile;
    uint32_t mOffset;
    bool eof;
    STAT mCurrentStat;

    bool statCheck(STAT stat);
};

#endif  /* TM_FILESRC_SENO_H */

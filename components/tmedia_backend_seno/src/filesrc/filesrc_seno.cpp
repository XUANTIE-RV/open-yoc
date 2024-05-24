/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <memory>
// #include <util_cvi.h>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/util/util_inc.h>

#include <tmedia_backend_seno/filesrc/filesrc_seno.h>
#include <tmedia_core/entity/filesrc/filesrc_factory.h>

using namespace std;

#define BUFFER_BLOCK_SIZE   380
#define GET_STRING_PROP(propList, prop, type) do {TMProperty *p = (propList).Get(type);\
        if (p) {prop = p->Value.String;}} while(0)

TMFileSrcSeno::TMFileSrcSeno()
    : TMFileSrc("TMFileSrcSeno"),
    fd(-1), mFile(NULL), mOffset(0), eof(false), mCurrentStat(STAT_CLOSED)
{
}

TMFileSrcSeno::~TMFileSrcSeno()
{
}

TMSrcPad *TMFileSrcSeno::GetSrcPad(int padID)
{
    return NULL;
}

int TMFileSrcSeno::Open(TMFileSrcParams &param, TMPropertyList *propList)
{
    if (!statCheck(STAT_OPENED)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    // mProtocol = param.protocol;
    // mUri = param.uri;

    if (propList) {
        GET_STRING_PROP(*propList, mProtocol, TMFileSrc::PropID::PROTOCOL);
        GET_STRING_PROP(*propList, mUri, TMFileSrc::PropID::URI);
        myPropList = *propList;
    }

    // TODO: test directory write access    

    mCurrentStat = STAT_OPENED;
    return TMResult::TM_OK;
}

int TMFileSrcSeno::SetConfig(TMPropertyList &propList)
{
    GET_STRING_PROP(propList, mProtocol, TMFileSrc::PropID::PROTOCOL);
    GET_STRING_PROP(propList, mUri, TMFileSrc::PropID::URI);
    myPropList = propList;

    return TMResult::TM_OK;
}

int TMFileSrcSeno::GetConfig(TMPropertyList &propList)
{
    propList = myPropList;

    return TMResult::TM_OK;
}

int TMFileSrcSeno::Close()
{
    if (!statCheck(STAT_CLOSED)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    mOffset = 0;
    eof = false;
    mCurrentStat = STAT_CLOSED;

    myPropList.Reset();
    return TMResult::TM_OK;
}

int TMFileSrcSeno::Start()
{
    if (!statCheck(STAT_RUNNING)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    if (mProtocol.empty() || mUri.empty()) {
        cout << "protocol or uri empty" << endl;
        return TMResult::TM_EINVAL;
    }

    if (mProtocol == "file") 
    {
        mFile = fopen(mUri.c_str(), "rb");
        if (!mFile)
        {
            cout << "file cannot open: " << mUri << endl;
            return TMResult::TM_EIO;
        }
    } 
    else 
    {
        cout << "error: file type not support" << endl;
        return TMResult::TM_EINVAL;
    }
    
    cout << "open " << mProtocol << " " << mUri << endl;
    mOffset = 0;
    eof = false;
    mCurrentStat = STAT_RUNNING;
    return TMResult::TM_OK;
}

int TMFileSrcSeno::Stop()
{
    if (!statCheck(STAT_OPENED)) {
        cout << "stat error " << mCurrentStat << "->" << STAT_CLOSED <<endl;
        return TMResult::TM_EINVAL;
    }

    if (mFile)
    {
        fclose(mFile);
        mFile = NULL;

        cout << "close " << mProtocol << " " << mUri << endl;
    }
    
    mOffset = 0;
    mCurrentStat = STAT_OPENED;
    return TMResult::TM_OK;
}

int TMFileSrcSeno::RecvPacket(TMPacket &pkt, int timeout)
{
    (void) timeout; // TODO: timeout implementation

    if (mCurrentStat != STAT_RUNNING)
    {
        cout << "error: src in stat:" << mCurrentStat <<endl;
        return TMResult::TM_EINVAL;
    }

    if (eof) {
        cout << "file eof" << endl;
        return TMResult::TM_EOF;
    }

    // TODO: use buffer pool
    pkt.Init();
    int ret = pkt.PrepareBuffer(BUFFER_BLOCK_SIZE);
    if (ret != TMResult::TM_OK) {
        cout << "pkt prepare error" <<endl;
        return TMResult::TM_EIO;
    }

    size_t n = fread(pkt.mData, 1, (size_t)pkt.mDataMaxLength, mFile);
    if (n != (size_t)pkt.mDataMaxLength) {
        if (errno) {
            cout << "read failed" << endl;
            eof = true;
            return TMResult::TM_EIO;
        }

        cout << "read end " << n << "bytes" << endl;
        eof = true;
        return TMResult::TM_EOF;
    }

    cout << "read " << n << " bytes" << endl;
    mOffset += n;

    return TMResult::TM_OK;
}

uint32_t TMFileSrcSeno::GetSize()
{
    return -1;
}

int TMFileSrcSeno::Seek(uint32_t offset)
{
    // if (offset < 0) {
    //     return TMResult::TM_EINVAL;
    // }

    // if (offset > GetSize()) {
    //     return TMResult::TM_EINVAL;
    // }
    if (!mFile) {
        cout << "seek failed: start first" << endl;
        return TMResult::TM_EINVAL;
    }

    int ret = fseek(mFile, offset, SEEK_SET);
    if (ret != 0) {
        
        return TMResult::TM_EIO;
    }

    this->mOffset = offset;
    return TMResult::TM_OK;
}


bool TMFileSrcSeno::statCheck(STAT stat)
{
    switch (mCurrentStat) 
    {
        case STAT_CLOSED:
        if (stat == STAT_OPENED)
            return true;
        break;

        case STAT_OPENED:
        if (stat == STAT_CLOSED || stat == STAT_RUNNING)
            return true;
        break;

        case STAT_RUNNING:
        if (stat == STAT_OPENED)
            return true;
        break;
    }

    return false;
}

REGISTER_FILESRC_CLASS(TMMediaInfo::MediaFileType::SIMPLE, TMFileSrcSeno)

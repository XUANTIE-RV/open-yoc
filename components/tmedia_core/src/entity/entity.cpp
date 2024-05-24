/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>

#define LOG_LEVEL 3 // Should at the top of all codes
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/entity/entity.h>

using namespace std;

TMEntity::TMEntity(string name)
    : mName(name), mPool(NULL), mPadsCreated(false),
      mState(State::RESET), mPipeline(NULL)
{
}

TMEntity::~TMEntity()
{
    DetachPool();
}

void TMEntity::AttachPool(TMBufferPool *pool)
{
    mPool = pool;
}

void TMEntity::DetachPool()
{
    mPool = NULL;
}

TMBufferPool* TMEntity::GetAttachPool()
{
    return mPool;
}

int TMEntity::PushEventToPipeline(TMEvent *event)
{
    if (mPipeline == NULL)
        return TMResult::TM_OK;

    int ret = mPipeline->PushEventToSubscriber(this, event);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("mPipeline->PushEventToSubscriber() failed: %s\n", TMResult::StrError(ret));
        return TMResult::TM_EVENT_PUSH_FAILED;
    }

    return TMResult::TM_OK;
}

TMSrcPad *TMSrcEntity::GetSrcPad(int padID)
{
    int ret;
    if (!mPadsCreated)
    {
        ret = CreatePads();
        if (ret != TMResult::TM_OK)
        {
            LOG_E("CreatePads() failed, ret=%d : '%s'\n", ret, TMResult::StrError(ret));
            return NULL;
        }
        mPadsCreated = true;
    }

    if (mSrcPads.find(padID) == mSrcPads.end())
    {
        LOG_E("Entity('%s')::SrcPad(%d) not found\n", mName.c_str(), padID);
        return NULL;
    }

    if (mSrcPads[padID] == NULL)
    {
        LOG_E("Entity('%s')::SrcPad(%d) is NULL\n", mName.c_str(), padID);
        return NULL;
    }

    return mSrcPads[padID];
}

int TMSrcEntity::DestoryPads()
{
    auto iterSink = mSrcPads.begin();
    while (iterSink != mSrcPads.end()) {
        delete(iterSink->second);
        mSrcPads.erase(iterSink++);
    }
    return TMResult::TM_OK;
}

int TMSrcEntity::PushEventToAllSinkPads(TMEvent *event)
{
    int ret;
    for (auto iter = mSrcPads.begin(); iter != mSrcPads.end(); iter++)
    {
        TMSrcPad *srcPad = iter->second;
        if (srcPad != NULL && srcPad->mPeerPad != NULL)
        {
            ret = srcPad->PushEvent(event);
            if (ret != TMResult::TM_OK)
            {
                LOG_E("Entity('%s')::Pad('%s') PushEvent(%u) failed: '%s'\n",
                    mName.c_str(), srcPad->mName.c_str(), (uint32_t)event->mType,
                    TMResult::StrError(ret));
                return TMResult::TM_EVENT_PUSH_FAILED;
            }
        }
    }

    return TMResult::TM_OK;
}

TMSinkPad *TMSinkEntity::GetSinkPad(int padID)
{
    int ret;
    if (!mPadsCreated)
    {
        ret = CreatePads();
        if (ret != TMResult::TM_OK)
        {
            LOG_E("CreatePads() failed, ret=%d : '%s'\n", ret, TMResult::StrError(ret));
            return NULL;
        }
        mPadsCreated = true;
    }

    if (mSinkPads.find(padID) == mSinkPads.end())
    {
        LOG_E("Entity('%s')::SrcPad(%d) not found\n", mName.c_str(), padID);
        return NULL;
    }

    if (mSinkPads[padID] == NULL)
    {
        LOG_E("Entity('%s')::SrcPad(%d) is NULL\n", mName.c_str(), padID);
        return NULL;
    }

    return mSinkPads[padID];
}

int TMSinkEntity::DestoryPads()
{
    auto iterSink = mSinkPads.begin();
    while (iterSink != mSinkPads.end()) {
        delete(iterSink->second);
        mSinkPads.erase(iterSink++);
    }
    return TMResult::TM_OK;
}

TMSrcPad *TMFilterEntity::GetSrcPad(int padID)
{
    int ret;
    if (!mPadsCreated)
    {
        ret = CreatePads();
        if (ret != TMResult::TM_OK)
        {
            LOG_E("CreatePads() failed, ret=%d : '%s'\n", ret, TMResult::StrError(ret));
            return NULL;
        }
        mPadsCreated = true;
    }

    if (mSrcPads.find(padID) == mSrcPads.end())
    {
        LOG_E("Entity('%s')::SrcPad(%d) not found\n", mName.c_str(), padID);
        return NULL;
    }

    if (mSrcPads[padID] == NULL)
    {
        LOG_E("Entity('%s')::SrcPad(%d) is NULL\n", mName.c_str(), padID);
        return NULL;
    }

    return mSrcPads[padID];
}

TMSinkPad *TMFilterEntity::GetSinkPad(int padID)
{
    int ret;
    if (!mPadsCreated)
    {
        ret = CreatePads();
        if (ret != TMResult::TM_OK)
        {
            LOG_E("CreatePads() failed, ret=%d : '%s'\n", ret, TMResult::StrError(ret));
            return NULL;
        }
        mPadsCreated = true;
    }

    if (mSinkPads.find(padID) == mSinkPads.end())
    {
        LOG_E("Entity('%s')::SrcPad(%d) not found\n", mName.c_str(), padID);
        return NULL;
    }

    if (mSinkPads[padID] == NULL)
    {
        LOG_E("Entity('%s')::SrcPad(%d) is NULL\n", mName.c_str(), padID);
        return NULL;
    }

    return mSinkPads[padID];
}

int TMFilterEntity::DestoryPads()
{
    auto iterSrc = mSrcPads.begin();
    while (iterSrc != mSrcPads.end()) {
        delete(iterSrc->second);
        mSrcPads.erase(iterSrc++);
    }

    auto iterSink = mSinkPads.begin();
    while (iterSink != mSinkPads.end()) {
        delete(iterSink->second);
        mSinkPads.erase(iterSink++);
    }
    return TMResult::TM_OK;
}

int TMFilterEntity::PushEventToAllSinkPads(TMEvent *event)
{
    int ret;
    for (auto iter = mSrcPads.begin(); iter != mSrcPads.end(); iter++)
    {
        TMSrcPad *srcPad = iter->second;
        if (srcPad != NULL && srcPad->mPeerPad != NULL)
        {
            ret = srcPad->PushEvent(event);
            if (ret != TMResult::TM_OK)
            {
                LOG_E("Entity('%s')::Pad('%s') PushEvent(%u) failed\n",
                    mName.c_str(), srcPad->mName.c_str(), (uint32_t)event->mType);
                return TMResult::TM_EVENT_PUSH_FAILED;
            }
        }
    }

    return TMResult::TM_OK;
}


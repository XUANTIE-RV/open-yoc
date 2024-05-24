/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <iostream>

#define LOG_LEVEL 3 // Should at the top of all codes
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/entity/codec/decoder.h>

using namespace std;

TMVideoDecoder::TMVideoDecoder()
    : mThreadData(NULL), mThreadRunFlag(false)
{
    mWorkMode = TMMediaInfo::WorkMode::DECODE;
}

TMVideoDecoder::~TMVideoDecoder()
{
}

int TMVideoDecoder::StartProcessData()
{
    DEBUG_LOG_FUNC_ENTER();

    if (mThreadData != NULL)
    {
        LOG_E("mThreadData is not NULL\n");
        return TMResult::TM_EPERM;
    }

    mThreadRunFlag = true;
    mThreadData = new std::thread(&TMVideoDecoder::ThreadRoutine, this);

    LOG_I("Started\n");
    return TMResult::TM_OK;
}

int TMVideoDecoder::StopProcessData()
{
    DEBUG_LOG_FUNC_ENTER();
    if (mThreadData == NULL)
    {
        //LOG_E("mThreadData is not NULL\n");
        return TMResult::TM_OK;
    }

    mThreadRunFlag = false;
    mThreadData->join();
    delete mThreadData;
    mThreadData = NULL;

    // TODO: flush data/event in queue

    LOG_I("Stopped\n");
    return TMResult::TM_OK;
}

int TMVideoDecoder::Close()
{
    DEBUG_LOG_FUNC_ENTER();
    return TMResult::TM_OK;
}

void TMVideoDecoder::ThreadRoutine()
{
    int ret;
    int sequenceNumber = 0;
    TMVideoFrame frame;

    while (mThreadRunFlag)
    {
        if (mState == TMEntity::State::PAUSED)
        {
            LOG_D("Enter PAUSED state\n");
            unique_lock<mutex> locker(mPipeMutex);
            while (mState == TMEntity::State::PAUSED)
            {
                mPipeCondition.wait(locker);    // by mPipeCondition.notify_all();
            }
            locker.unlock();
            LOG_D("Leave PAUSED state\n");
            continue;
        }

        ret = RecvFrame(frame, 1000);
        if (ret == TMResult::TM_EAGAIN)
        {
            // TODO: remove sleep after timeout works on all platforms
            this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        else if (ret != TMResult::TM_OK)
        {
            LOG_E("RecvFrame() failed: %s", TMResult::StrError(ret));
            break;
        }
        frame.mSeqNum = sequenceNumber++;
        ret = mSrcPads[0]->PushData(&frame);
        if (ret != TMResult::TM_OK)
        {
            LOG_E("PushData failed, ret=%d\n", ret);
            frame.UnRef();
        }
    }

    mThreadRunFlag = false;
    LOG_I("thread exit\n");
}

int TMVideoDecoder::EnqueueData(TMSinkPad *pad, TMData *data, int timeout)
{
    int ret;
    if (pad == NULL)
    {
        LOG_E("pad is NULL\n");
        return TMResult::TM_EINVAL;
    }

    if (data == NULL)
    {
        LOG_E("data is NULL\n");
        return TMResult::TM_EINVAL;
    }

    TMPacket *packet = dynamic_cast<TMPacket *>(data);
    if (packet == NULL)
    {
        LOG_E("data is not TMPacket type\n");
        return TMResult::TM_EINVAL;
    }
    //packet->Dump();

SEND_AGAIN:
    ret = SendPacket(*packet, 1000);
    if (ret != TMResult::TM_OK)
    {
        if (ret == TMResult::TM_EAGAIN)
        {
            this_thread::sleep_for(std::chrono::milliseconds(10));
            goto SEND_AGAIN;
        }
        else if (ret == TMResult::TM_ENOMEM)
        {
            LOG_D("SendPacket return with TM_ENOMEM, need wait for frame back to pool\n");
            this_thread::sleep_for(std::chrono::milliseconds(10));
            goto SEND_AGAIN;
        }
        else
        {
            LOG_E("decoder->SendPacket() failed: %s\n", TMResult::StrError(ret));
            packet->UnRef();
            return TMResult::TM_BACKEND_ERROR;
        }
    }

    ret = packet->UnRef();
    if (ret != TMResult::TM_OK)
    {
        LOG_E("SendPacket() failed, ret=%d\n", ret);
    }

    return ret;
}

int TMVideoDecoder::ProcessEvent(TMEvent *event)
{
    DEBUG_LOG_FUNC_ENTER();
    if (event == NULL)
    {
        LOG_E("event is NULL\n");
        return TMResult::TM_EINVAL;
    }

    switch (event->mType)
    {
    case TMEvent::Type::START:
        return PipeStart();
    case TMEvent::Type::STOP:
        return PipeStop();
    case TMEvent::Type::PAUSE:
        return PipePause();
    case TMEvent::Type::RESUME:
        return PipeStart();
    default:
        LOG_E("Not supported event type(%umake)\n", (uint32_t)event->mType);
        return TMResult::TM_NOT_SUPPORT;
    }
}

int TMVideoDecoder::PipeStart()
{
    DEBUG_LOG_FUNC_ENTER();
    int ret;

    if (mState == TMEntity::State::RUNNING)
    {
        return TMResult::TM_OK;
    }

    if (mState != TMEntity::State::READY && mState != TMEntity::State::PAUSED)
    {
        LOG_E("Entity('%s') mState(%u) error\n", mName.c_str(), (uint32_t)mState);
        return TMResult::TM_STATE_ERROR;
    }

    /* Send Start event to all peer entities */
    TMEvent event(TMEvent::Type::START);
    ret = PushEventToAllSinkPads(&event);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("PushEventToAllSinkPads() failed, ret=%d: '%s'\n", ret, TMResult::StrError(ret));
        return ret;
    }

    if (mState == TMEntity::State::READY)
    {
        ret = Start();  // call final virtual function
        if (ret != TMResult::TM_OK)
        {
            LOG_E("Start() failed, ret=%d: '%s'\n", ret, TMResult::StrError(ret));
            return ret;
        }

        ret = StartProcessData();
        if (ret != TMResult::TM_OK)
        {
            LOG_E("StartProcessData() failed, ret=%d\n", ret);
            return ret;
        }
    }
    else
    {
        /* else in PAUSED state notify thread continue */
        mState = TMEntity::State::RUNNING;
        mPipeCondition.notify_all();
    }

    return TMResult::TM_OK;
}

int TMVideoDecoder::PipePause()
{
    DEBUG_LOG_FUNC_ENTER();
    /* Send Start event to all peer entities */
    TMEvent event(TMEvent::Type::PAUSE);
    if (PushEventToAllSinkPads(&event) != TMResult::TM_OK)
    {
        LOG_E("PushEventToAllSinkPads() failed\n");
        return TMResult::TM_EVENT_PUSH_FAILED;
    }

    mState = TMEntity::State::PAUSED;
    return TMResult::TM_OK;
}

int TMVideoDecoder::PipeStop()
{
    DEBUG_LOG_FUNC_ENTER();
    int ret;

    if (mState == TMEntity::State::READY)
    {
        return TMResult::TM_OK;
    }

    /* if current mState is PAUSED, the ThreadRoutine is stucked,
     * should notify mPipeCondition first, then restore mState
     */
    State tmpState = mState;
    if (mState == State::PAUSED)
    {
        mState = State::READY;
        mPipeCondition.notify_all();
    }
    ret = StopProcessData();
    if (ret != TMResult::TM_OK)
    {
        LOG_E("StopProcessData() failed, ret=%d\n", ret);
        return ret;
    }
    mState = tmpState;  // restore State to avoid Stop() check state fail

    ret = Stop();
    if (ret != TMResult::TM_OK)
    {
        LOG_E("Call final virtual override Stop() failed, ret=%d\n", ret);
        return ret;
    }

    /* Send Start event to all peer entities */
    TMEvent event(TMEvent::Type::STOP);
    ret = PushEventToAllSinkPads(&event);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("PushEventToAllSinkPads() failed\n");
        return ret;
    }

    return TMResult::TM_OK;
}

int TMVideoDecoder::CreatePads()
{
    TMPad::Param_s param;

    param = {"VideoDecoder_SinkPad", TMPad::Mode::AUTO, TMData::Type::STREAM_VIDEO};
    TMSinkPad *sinkPad = new TMSinkPad(this, &param);
    if (sinkPad == NULL)
    {
        LOG_E("new TMSrcPad() failed\n");
        return TMResult::TM_ENOMEM;
    }
    mSinkPads[0] = sinkPad;

    param = {"VideoDecoder_SrcPad", TMPad::Mode::PUSH, TMData::Type::FRAME_VIDEO};
    TMSrcPad *srcPad = new TMSrcPad(this, &param);
    if (srcPad == NULL)
    {
        LOG_E("new TMSrcPad() failed\n");
        return TMResult::TM_ENOMEM;
    }
    mSrcPads[0] = srcPad;

    return TMResult::TM_OK;
}

int TMVideoDecoder::DestoryPads()
{
    return TMFilterEntity::DestoryPads();
}

TMAudioDecoder::TMAudioDecoder()
{
    mWorkMode = TMMediaInfo::WorkMode::DECODE;
}

TMAudioDecoder::~TMAudioDecoder()
{
}


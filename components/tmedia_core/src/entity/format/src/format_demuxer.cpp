/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#define LOG_LEVEL 3 // Should at the top of all codes
#include <thread>
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/entity/format/format_demuxer.h>

using namespace std;

TMFormatDemuxer::TMFormatDemuxer()
    : mThread(NULL), mThreadRunFlag(false)
{
}

TMFormatDemuxer::~TMFormatDemuxer()
{
    // delete all pads in mSrcPads
    DestoryPads();
}

void TMFormatDemuxer::ThreadRoutine()
{
    int ret;
    TMPacket packet;
    uint64_t seqNum = 0;

    while (mThreadRunFlag)
    {
        if (mState == TMEntity::State::PAUSED)
        {
            cout << "Info : Enter PAUSED state" << endl;
            unique_lock<mutex> locker(mPipeMutex);
            while (mState == TMEntity::State::PAUSED)
            {
                mPipeCondition.wait(locker);    // by mPipeCondition.notify_all();
            }
            locker.unlock();
            cout << "Info : Leave PAUSED state" << endl;
            continue;
        }

        ret = ReadPacket(packet);
        if (ret == TMResult::TM_EOF)
        {
            LOG_I("ReadPacket() meet EOF, exit thread loop\n");
            break;
        }
        else if (ret != TMResult::TM_OK)
        {
            LOG_E("ReadPacket failed, exit thread loop\n");
            break;
        }
        packet.mSeqNum = seqNum++;

        // loop in mSrcPads and push data to related pad(s)
        for (auto iter = mSrcPads.begin(); iter != mSrcPads.end(); iter++)
        {
            TMSrcPad *srcPad = iter->second;
            if (srcPad->mDataType == packet.mDataType)
            {
                ret = srcPad->PushData(&packet);
                if (ret != TMResult::TM_OK)
                {
                    LOG_E("PushData failed, ret=%d:'%s'\n", ret, TMResult::StrError(ret));
                    packet.UnRef();
                }

                // FIXME: only push to one pad, but if more than one, need packet.Ref()
                LOG_D("PushData(mSeqNum=%lu) OK\n", packet.mSeqNum);
                break;
            }
            else
            {
                packet.UnRef();
            }
        }
    }

    mThreadRunFlag = false;
    LOG_I("thread exit\n");
}

int TMFormatDemuxer::StartProcessData()
{
    DEBUG_LOG_FUNC_ENTER();

    if (mThread != NULL)
    {
        LOG_E("mThread is not NULL\n");
        return TMResult::TM_EPERM;
    }

    mThreadRunFlag = true;
    mThread = new std::thread(&TMFormatDemuxer::ThreadRoutine, this);

    LOG_I("started\n");
    return TMResult::TM_OK;
}

int TMFormatDemuxer::StopProcessData()
{
    DEBUG_LOG_FUNC_ENTER();

    if (mThread == NULL)
    {
        //LOG_E("mThread is not NULL\n");
        return TMResult::TM_EPERM;
    }

    mThreadRunFlag = false;
    mThread->join();
    delete mThread;
    mThread = NULL;

    // TODO: flush data/event in queue

    LOG_I("Demuxer stopped\n");
    return TMResult::TM_OK;
}

TMSrcPad *TMFormatDemuxer::GetSrcPad(TMData::Type dataType, int streamID)
{
    if (!mPadsCreated)
    {
        int ret = CreatePads();
        if (ret != TMResult::TM_OK)
        {
            LOG_E("CreatePads() failed, ret=%d : '%s'\n", ret, TMResult::StrError(ret));
            return NULL;
        }
        mPadsCreated = true;
    }

    if (streamID >= 0)  // TODO: Add streamID search support
    {
        LOG_E("streamID not supported yet\n");
        return NULL;
    }

    for (auto iter = mSrcPads.begin(); iter != mSrcPads.end(); iter++)
    {
        TMSrcPad *srcPad = iter->second;
        if (srcPad != NULL && srcPad->mDataType == dataType)
        {
            return srcPad;
        }
    }

    LOG_E("Can't find src pad dataType is:%u\n", static_cast<unsigned int>(dataType));
    return NULL;
}

int TMFormatDemuxer::PipeStart()
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
        LOG_E("PushEventToAllSinkPads() failed\n");
        return ret;
    }

    if (mState == TMEntity::State::READY)
    {
        ret = Start();  // call final virtual function
        if (ret != TMResult::TM_OK)
        {
            LOG_E("Call final virtual override Start() failed, ret=%d\n", ret);
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

int TMFormatDemuxer::PipePause()
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

int TMFormatDemuxer::PipeStop()
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

int TMFormatDemuxer::CreatePads()
{
    TMFormatInfo fmtInfo;
    int ret = GetFormatInfo(fmtInfo);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("GetFormatInfo() failed, ret=%d\n", ret);
        return ret;
    }

    for (uint32_t i = 0; i < fmtInfo.mStreamCount; i++)
    {
        TMSrcPad *srcPad;
        TMPad::Param_s padParam;

        padParam.mode = TMPad::Mode::AUTO;

        TMStreamInfo *streamInfo = fmtInfo.mStreamInfoMap[i];
        switch (streamInfo->mMediaType)
        {
        case TMMediaInfo::Type::VIDEO:
            padParam.name = "Demuxer_Video_SrcPad(" + to_string(i) + ")";
            padParam.data_type = TMData::Type::STREAM_VIDEO;
            break;
        case TMMediaInfo::Type::AUDIO:
            padParam.name = "Demuxer_Audio_SrcPad(" + to_string(i) + ")";
            padParam.data_type = TMData::Type::STREAM_VIDEO;
            break;
        default:
            padParam.name = "Demuxer_Unknown_SrcPad(" + to_string(i) + ")";
            LOG_I("Unknow stream_id(%d) MediaType(%d)\n", i, static_cast<int>(streamInfo->mMediaType));
            padParam.data_type = TMData::Type::UNKNOWN;
            continue;
        }

        srcPad = new TMSrcPad(this, &padParam);
        if (srcPad == NULL)
        {
            LOG_E("new TMSrcPad failed\n");
            return TMResult::TM_ENOMEM;
        }

        mSrcPads[i] = srcPad;
        LOG_D("Add srcpad:'%s'(%p) to entity:'%s'\n",
            srcPad->mName.c_str(), srcPad, mName.c_str());
        //srcPad->DumpInfo();
    }

    return TMResult::TM_OK;
}

int TMFormatDemuxer::DestoryPads()
{
    return TMSrcEntity::DestoryPads();
}


/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */
#include <tmedia_backend_seno/format/ts_muxer_seno.h>

using namespace std;

TMTsMuxerSeno::TMTsMuxerSeno()
{
}

TMTsMuxerSeno::~TMTsMuxerSeno()
{
    Close();
}

int TMTsMuxerSeno::Open(TMPropertyList *propList)
{
    if (propList != NULL)
    {
        vector<int> diffIDArray;
        int diffCount = mCurrentPropertyList.FindDiff(diffIDArray, propList);

        for (uint32_t i = 0; i < diffIDArray.size(); i++)
        {
            int diffID = diffIDArray[i];

            TMProperty *currProperty = &mCurrentPropertyList.mProperties[diffID];
            TMProperty *newProperty = &propList->mProperties[diffID];

            if (0)
            {
                cout << "PropID(" << diffID << ":'" << currProperty->Name << "') changed." << endl;
                cout << "from :";
                currProperty->Dump();
                cout << "to be:";
                newProperty->Dump();
            }
            currProperty->Assign(newProperty);
        }

        if (diffCount <= 0)
        {
            LOG_D("No different property found\n");
        }
    }
    uint32_t BufSize = mCurrentPropertyList.GetUint(TMTsMuxer::PropID::TSMUXER_BUF_SIZE);
    mVideoMux      = new TsMuxer();
    mVideoFrame    = new TsFrame();
    mAudioMux      = new TsMuxer();
    mAudioFrame    = new TsFrame();
    mRingBufferPtr = new TMUtilRingBuffer(BufSize);
    mSendAble = true;
    mRecvAble = true;
    return TMResult::TM_OK;
}

int TMTsMuxerSeno::Close()
{
    if (mVideoMux)
    {
        delete mVideoMux;
        mVideoMux = NULL;
    }
    if (mVideoFrame)
    {
        delete mVideoFrame;
        mVideoFrame = NULL;
    }
    if (mAudioMux)
    {
        delete mAudioMux;
        mAudioMux = NULL;
    }
    if (mAudioFrame)
    {
        delete mAudioFrame;
        mAudioFrame = NULL;
    }
    if (mRingBufferPtr)
    {
        delete mRingBufferPtr;
        mRingBufferPtr = NULL;
    }
    return TMResult::TM_OK;
}

int TMTsMuxerSeno::Start()
{
    return TMResult::TM_OK;
}

int TMTsMuxerSeno::Stop()
{
    return TMResult::TM_OK;
}

int TMTsMuxerSeno::SetConfig(TMPropertyList &propertyList)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMTsMuxerSeno::GetConfig(TMPropertyList &propertyList)
{
    propertyList.Reset();
    TMPropertyMap_t *currProps = &(mCurrentPropertyList.mProperties);
    for (auto iter = currProps->begin(); iter != currProps->end(); iter++)
    {
        TMProperty *prop = &iter->second;
        propertyList.Add(prop);
    }
    return TMResult::TM_OK;
}

int TMTsMuxerSeno::MapType(TMMediaInfo::CodecID mtype)
{
    switch (mtype)
    {
    case TMMediaInfo::CodecID::H264:
        return 27;
    case TMMediaInfo::CodecID::MP3:
        return 3;
    case TMMediaInfo::CodecID::G711:
        return 144;
    default:
        return 27;
    }
}

int TMTsMuxerSeno::SendPacket(TMPacket &pkt, int timeout)
{
    auto timeoutMs = std::chrono::milliseconds(timeout);
    if (pkt.mDataType == TMData::Type::STREAM_VIDEO)
    {
        mMuxerLock.lock();
        int ret = 0;
        int stream_id = pkt.mStreamIndex;
        int estype = MapType(pkt.mCodecID);
        TMClock_t TimePTS, TimeDTS, TimePTSOrig, TimeDTSOrig;
        TimePTS.time_base = MPEG_TIME_BASE;
        TimeDTS.time_base = MPEG_TIME_BASE;
        TimePTSOrig = pkt.mPTS.Get();
        TimeDTSOrig = pkt.mDTS.Get();
        TMClock::TimestampConvert(&TimePTS, &TimePTSOrig);
        TMClock::TimestampConvert(&TimeDTS, &TimeDTSOrig);
        pkt.mPTS.Set(TimePTS);
        pkt.mDTS.Set(TimeDTS);

        mVideoFrame->reset();
        mVideoMux->reset();
        mVideoFrame->setFrame((const uint8_t *)pkt.mData, pkt.mDataLength);
        mVideoFrame->setTimestamp(pkt.mPTS.Get().timestamp, pkt.mDTS.Get().timestamp);
        mVideoFrame->setPid(stream_id);
        mVideoFrame->setType(ES_TYPE_ID(estype));

        mVideoMux->addPmt(TEST_PACKET_PID::TEST_PMT_PID);
        mVideoMux->addPmtElement(TEST_PACKET_PID::TEST_PMT_PID, ES_TYPE_ID(estype), stream_id);
        mVideoMux->pack(mVideoFrame);
        ret = mRingBufferPtr->Write((char *)(mVideoMux->data()), mVideoMux->size());
        mMuxerLock.unlock();
        if (ret == -1)
        {
            mSendAble = false;
            LOG_D("SENDV waiting for recv...\n");
            unique_lock<mutex> lck(mMuxerWaitSendV);

            if (!mMuxerCV.wait_for(lck, timeoutMs, [this](){return this->mSendAble;}))
            {
                LOG_D("wait timeout(%d ms)\n", timeout);
                return TMResult::TM_TIMEOUT;
            }
            return TMResult::TM_EBUSY;
        }
        mRecvAble = true;
        mMuxerCV.notify_all();
        //LOG_D("Send Video Packet successufully! Stream id = %d, packet length = %d, ts packet length = %ld\n", stream_id, packetptr->mDataLength, mVideoMux->size());
        return TMResult::TM_OK;

    }
    else if (pkt.mDataType == TMData::Type::STREAM_AUDIO)
    {
        mMuxerLock.lock();
        int ret = 0;
        int stream_id = pkt.mStreamIndex;
        int estype = MapType(pkt.mCodecID);
        TMClock_t TimePTS, TimeDTS, TimePTSOrig, TimeDTSOrig;
        TimePTS.time_base = MPEG_TIME_BASE;
        TimeDTS.time_base = MPEG_TIME_BASE;
        TimePTSOrig = pkt.mPTS.Get();
        TimeDTSOrig = pkt.mDTS.Get();
        TMClock::TimestampConvert(&TimePTS, &TimePTSOrig);
        TMClock::TimestampConvert(&TimeDTS, &TimeDTSOrig);
        pkt.mPTS.Set(TimePTS);
        pkt.mDTS.Set(TimeDTS);

        mAudioFrame->reset();
        mAudioMux->reset();
        mAudioFrame->setFrame((const uint8_t *)pkt.mData, pkt.mDataLength);
        mAudioFrame->setTimestamp(pkt.mPTS.Get().timestamp, pkt.mDTS.Get().timestamp);
        mAudioFrame->setPid(stream_id);
        mAudioFrame->setType(ES_TYPE_ID(estype));

        mAudioMux->addPmt(TEST_PACKET_PID::TEST_PMT_PID);
        mAudioMux->addPmtElement(TEST_PACKET_PID::TEST_PMT_PID, ES_TYPE_ID(estype), stream_id);
        mAudioMux->pack(mAudioFrame);
        ret = mRingBufferPtr->Write((char *)(mAudioMux->data()), mAudioMux->size());
        mMuxerLock.unlock();
        if (ret == -1)
        {
            mSendAble = false;
            LOG_D("SENDA waiting for recv...\n");
            unique_lock<mutex> lck(mMuxerWaitSendA);

            if (!mMuxerCV.wait_for(lck, timeoutMs, [this](){return this->mSendAble;}))
            {
                LOG_D("wait timeout(%d ms)\n", timeout);
                return TMResult::TM_TIMEOUT;
            }
            return TMResult::TM_EBUSY;
        }
        mRecvAble = true;
        mMuxerCV.notify_all();
        //LOG_D("Send Audio Packet successufully! Stream id = %d, packet length = %d, ts packet length = %ld\n", stream_id, packetptr->mDataLength, mAudioMux->size());
        return TMResult::TM_OK;
    }
    else
    {
        LOG_E("Error packet DataType!\n");
        return TMResult::TM_STATE_ERROR;
    }
}




int TMTsMuxerSeno::RecvPacket(TMPacket &pkt, int timeout)
{
    auto timeoutMs = std::chrono::milliseconds(timeout);
    mMuxerLock.lock();
    int length = mRingBufferPtr->AvaliableReadSpace();

    if (length)
    {
        pkt.Init();
        pkt.PrepareBuffer(length);
        mRingBufferPtr->Read((char *)(pkt.mData), length);
        pkt.mDataLength = length;
        //LOG_D("Receive TS packet successfully! packet length = %d bytes\n", length);
        mSendAble = true;
        mRingBufferPtr->Clear();
        mMuxerLock.unlock();
        mMuxerCV.notify_all();
        return TMResult::TM_OK;
    }
    else
    {
        mSendAble = true;
        mRecvAble = false;
        mRingBufferPtr->Clear();
        mMuxerLock.unlock();
        mMuxerCV.notify_all();
        LOG_D("recv program goes silent...\n");
        unique_lock<mutex> lck(mMuxerWaitRecv);

        if (!mMuxerCV.wait_for(lck, timeoutMs, [this](){return this->mRecvAble;}))
        {
            LOG_D("wait timeout(%d ms)\n", timeout);
            return TMResult::TM_TIMEOUT;
        }
        return TMResult::TM_EAGAIN;
    }
}

REGISTER_MUXER_CLASS(TMMediaInfo::FormatID::TS, TMTsMuxerSeno)




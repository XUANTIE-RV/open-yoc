/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#include <tmedia_backend_seno/format/ts_demuxer_seno.h>

using namespace std;

TMTsDemuxerSeno::TMTsDemuxerSeno()
{
}

TMTsDemuxerSeno::~TMTsDemuxerSeno()
{
    Close();
}

int TMTsDemuxerSeno::Open(string fileName, TMPropertyList *propList)
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
    uint32_t InputBufSize = mCurrentPropertyList.GetUint(TMTsDemuxer::PropID::TSDEMUXER_INBUF_SIZE);
    uint32_t OutputBufSize = mCurrentPropertyList.GetUint(TMTsDemuxer::PropID::TSDEMUXER_OUTBUF_SIZE);

    mDemux  = new TsDemuxer();
    mPacket = new Packet();
    mRingBufferInPtr = new TMUtilRingBuffer(InputBufSize);
    mRingBufferOutPtr = new TMUtilRingBuffer(OutputBufSize);
    mPnum = 0;
    for (int i = 0; i < 188; i++)
    {
        mTempPesData[i] = '\0';
    }
    mSendAble = true;
    mRecvAble = true;
    mPacketFull = false;
    return TMResult::TM_OK;
}

int TMTsDemuxerSeno::Close()
{
    if (mDemux)
    {
        delete mDemux;
        mDemux = NULL;
    }
    if (mPacket)
    {
        delete mPacket;
        mPacket = NULL;
    }
    if (mRingBufferInPtr)
    {
        delete mRingBufferInPtr;
        mRingBufferInPtr = NULL;
    }
    if (mRingBufferOutPtr)
    {
        delete mRingBufferOutPtr;
        mRingBufferOutPtr = NULL;
    }
    return TMResult::TM_OK;
}

int TMTsDemuxerSeno::GetFormatInfo(TMFormatInfo &fmtInfo)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMTsDemuxerSeno::GetCodecParam(TMCodecParams &codecParams)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMTsDemuxerSeno::Start()
{
    return TMResult::TM_OK;
}

int TMTsDemuxerSeno::Stop()
{
    return TMResult::TM_OK;
}

int TMTsDemuxerSeno::Seek(int64_t timestamp)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMTsDemuxerSeno::ReadPacket(TMPacket &packet)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMTsDemuxerSeno::SetConfig(TMPropertyList &propertyList)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMTsDemuxerSeno::GetConfig(TMPropertyList &propertyList)
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

int TMTsDemuxerSeno::SendPacket(TMPacket &pkt, int timeout)
{
    auto timeoutMs = std::chrono::milliseconds(timeout);
    mDemuxerLock.lock();
    int ret = 0;
    ret = mRingBufferInPtr->Write((char *)(pkt.mData), pkt.mDataLength);
    mDemuxerLock.unlock();

    if (ret == -1)
    {
        mSendAble = false;
        LOG_D("Waiting for recv...\n");
        unique_lock<mutex> lck(mDemuxerWaitSend);

        if (!mDemuxerCV.wait_for(lck, timeoutMs, [this](){return this->mSendAble;}))
        {
            LOG_D("wait timeout(%d ms)\n", timeout);
            return TMResult::TM_TIMEOUT;
        }
        return TMResult::TM_EBUSY;
    }
    mRecvAble = true;
    mDemuxerCV.notify_all();
    return TMResult::TM_OK;
}

TMMediaInfo::CodecID TMTsDemuxerSeno::MapType(ES_TYPE_ID mtype)
{
    switch (mtype)
    {
    case ES_TYPE_ID::VIDEO_H264:
        return TMMediaInfo::CodecID::H264;
    case ES_TYPE_ID::VIDEO_H265:
        return TMMediaInfo::CodecID::H265;
    case ES_TYPE_ID::AUDIO_MPEG1:
        return TMMediaInfo::CodecID::MP3;
    case ES_TYPE_ID::AUDIO_G711:
        return TMMediaInfo::CodecID::G711;
    default:
        return TMMediaInfo::CodecID::UNKNOWN;
    }
}

int TMTsDemuxerSeno::RecvPacket(TMPacket &pkt, int timeout)
{
    auto timeoutMs = std::chrono::milliseconds(timeout);
    mDemuxerLock.lock();
    int real_size = 0;
    bool decret = 0;
    int  stream_id_before = -1;
    uint64_t   pts_before = 0;
    uint64_t   dts_before = 0;
    ES_TYPE_ID estype_before = ES_TYPE_ID::UNKNOWN;
    int first_time = 1;
    int pes_data_len = 0;
    int write_num = 0;
    int flag_eof = 0;
    int flag_left = 0;
    TMClock_t time_pts_mpeg, time_dts_mpeg;

    while (true)
    {
        if (!mPacketFull)
        {
            real_size = mRingBufferInPtr->Read((char *)(mPacket->m_data), TS_PACKET_SIZE);
        }
        else
        {
            mPacketFull = false;
            real_size = TS_PACKET_SIZE;
        }

        if (real_size == 0)
        {
            flag_eof = 1;
            break;
        }

        if (real_size != TS_PACKET_SIZE)
        {
            LOG_D("Find there is a ts packet less than 188 bytes at the end of the packet sent in.\n");
            flag_eof = 1;
            flag_left = 1;
            break;
        }

        if (mPacket->m_data[0] != SYNC_BYTE)
        {
            LOG_E("[error]: Sync byte not found.\n");
            return TMResult::TM_STATE_ERROR;
        }
        decret = mDemux->demuxPacket(*mPacket);

        if (!decret)
        {
            return TMResult::TM_STATE_ERROR;
        }

        if (mDemux->mOutPropty.stream_id == -1)
        {
            continue;
        }

        if (first_time)
        {
            first_time = 0;
            mRingBufferOutPtr->Clear();
            pes_data_len = int(mDemux->mOutPropty.pes_data.size());
            copy(mDemux->mOutPropty.pes_data.begin(), mDemux->mOutPropty.pes_data.end(), mTempPesData);
            mRingBufferOutPtr->Write(mTempPesData, pes_data_len);
            stream_id_before = mDemux->mOutPropty.stream_id;
            pts_before = mDemux->mOutPropty.pts;
            dts_before = mDemux->mOutPropty.dts;
            estype_before = mDemux->mOutPropty.estype;
            time_pts_mpeg.timestamp = pts_before;
            time_dts_mpeg.timestamp = dts_before;
            time_pts_mpeg.time_base = MPEG_TIME_BASE;
            time_dts_mpeg.time_base = MPEG_TIME_BASE;
            time_pts_mpeg.valid = true;
            time_dts_mpeg.valid = true;
            pkt.Init();
            pkt.mPTS.Set(time_pts_mpeg);
            pkt.mPTS.SetDefault(time_pts_mpeg);
            pkt.mDTS.Set(time_dts_mpeg);
            pkt.mDTS.SetDefault(time_dts_mpeg);
            pkt.mStreamIndex = stream_id_before;
            pkt.mCodecID = MapType(estype_before);
            mPnum++;
        }
        else
        {
            if (stream_id_before != mDemux->mOutPropty.stream_id || \
                    pts_before != mDemux->mOutPropty.pts || \
                    dts_before != mDemux->mOutPropty.dts || \
                    estype_before != mDemux->mOutPropty.estype)
            {
                mPacketFull = true;
                break;
            }
            else
            {
                pes_data_len = int(mDemux->mOutPropty.pes_data.size());
                copy(mDemux->mOutPropty.pes_data.begin(), mDemux->mOutPropty.pes_data.end(), mTempPesData);
                int ret = 0;
                ret = mRingBufferOutPtr->Write(mTempPesData, pes_data_len);

                if (ret == -1)
                {
                    mPacketFull = true;
                    break;
                }
                mPnum++;
            }
        }
    }
    mSendAble = true;
    mDemuxerCV.notify_all();
    write_num = mRingBufferOutPtr->AvaliableReadSpace();

    if (write_num)
    {
        pkt.PrepareBuffer(write_num);
        mRingBufferOutPtr->Read((char *)(pkt.mData), write_num);
        pkt.mDataLength = write_num;
        mRingBufferOutPtr->Clear();
        mDemuxerLock.unlock();
        return TMResult::TM_OK;
    }

    if (mPacketFull)
    {
        mDemuxerLock.unlock();
        return TMResult::TM_OK;
    }
    else if (flag_eof && flag_left)
    {
        mRecvAble = false;
        mRingBufferInPtr->Clear();
        mDemuxerLock.unlock();
        LOG_D("Recv program goes silent...\n");
        unique_lock<mutex> lck(mDemuxerWaitRecv);

        if (!mDemuxerCV.wait_for(lck, timeoutMs, [this](){return this->mRecvAble;}))
        {
            LOG_D("wait timeout(%d ms)\n", timeout);
            return TMResult::TM_TIMEOUT;
        }
        return TMResult::TM_EAGAIN;
    }
    else  //flag_eof
    {
        mRecvAble = false;
        mRingBufferInPtr->Clear();
        mDemuxerLock.unlock();
        LOG_D("Recv program goes silent...\n");
        unique_lock<mutex> lck(mDemuxerWaitRecv);

        if (!mDemuxerCV.wait_for(lck, timeoutMs, [this](){return this->mRecvAble;}))
        {
            LOG_D("wait timeout(%d ms)\n", timeout);
            return TMResult::TM_TIMEOUT;
        }
        return TMResult::TM_EAGAIN;
    }
}

REGISTER_DEMUXER_CLASS(TMMediaInfo::FormatID::TS, TMTsDemuxerSeno)



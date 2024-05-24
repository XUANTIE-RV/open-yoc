/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */


#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <iostream>

#define LOG_LEVEL 3
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/util/util_inc.h>
#include <tmedia_core/bind/pad.h>
#include <tmedia_core/entity/entity.h>

using namespace std;

TMPad::TMPad(TMEntity *entity, TMPad::Param_s *param)
    : mName(""),
      mMode(Mode::AUTO),
      mDataType(TMData::Type::UNKNOWN),
      mDirection(Direction::UNKNOWN),
      mActive(false),
      mParent(entity)
{
    if (param != NULL)
    {
        mName = param->name;
        mMode = param->mode;
        mDataType = param->data_type;
    }
}

TMPad::~TMPad()
{
}

void TMPad::DumpInfo()
{
    cout << "  Name      : " << mName << endl;
    switch(mDirection)
    {
    case Direction::SRC:
        cout << "  Direction : SRC" << endl;
        break;
    case Direction::SINK:
        cout << "  Direction : SINK" << endl;
        break;
    default:
        cout << "  Direction : UNKNOWN(" << (uint32_t)mDirection << endl;
        break;
    }

    switch(mMode)
    {
    case Mode::AUTO:
        cout << "  Mode      : AUTO" << endl;
        break;
    case Mode::PUSH:
        cout << "  Mode      : PUSH" << endl;
        break;
    case Mode::PULL:
        cout << "  Mode      : PULL" << endl;
        break;
    }

    cout << "  This pad  : " << hex << this << dec << endl;
}

int TMPad::IsActive()
{
    return mActive;
}

bool TMPad::IsSrcPad()
{
    return (mDirection == Direction::SRC);
}

bool TMPad::IsSinkPad()
{
    return (mDirection == Direction::SINK);
}

int TMPad::GetParent(TMEntity **entity)
{
    if (entity == NULL)
    {
        cout << "Error: entity is NULL" << endl;
        return TMResult::TM_EINVAL;
    }

    if (mParent == NULL)
    {
        cout << "Error: mParent is NULL" << endl;
        *entity = NULL;
        return TMResult::TM_ENTITY_NOT_FOUND;
    }

    *entity = mParent;
    return TMResult::TM_OK;
}

TMSrcPad::TMSrcPad(TMEntity *entity, TMPad::Param_s *param)
    : TMPad(entity, param), mPeerPad(NULL)
{
    mDirection = Direction::SRC;
}

int TMSrcPad::Bind(TMPad *pad)
{
    if (pad == NULL)
    {
        cout << "Error: pad is NULL" << endl;
        return TMResult::TM_EINVAL;
    }

    TMSinkPad *peerPad = dynamic_cast<TMSinkPad *>(pad);
    if (peerPad == NULL)
    {
        cout << "Error: pad is not TMSinkPad>" << endl;
        return TMResult::TM_EINVAL;
    }

    peerPad->mPeerPad = this;
    mPeerPad = peerPad;

    return TMResult::TM_OK;
}

int TMSrcPad::UnBind()
{
    if (mPeerPad != NULL)
    {
        mPeerPad->mPeerPad = NULL;
        mPeerPad = NULL;
    }
    return TMResult::TM_OK;
}

int TMSrcPad::PushData(TMData *data)
{
    int ret;
    if (mPeerPad == NULL)
    {
        LOG_E("Peer pad not exists\n");
        return TMResult::TM_PEER_PAD_NOT_FOUND;
    }

    if (mMode != Mode::PUSH && mMode != Mode::AUTO)
    {
        LOG_E("pad is not in PUSH or AUTO mode\n");
        return TMResult::TM_EPERM;
    }

    if (!mPeerPad->IsSinkPad())
    {
        LOG_E("mPeerPad is not Sink\n");
        return TMResult::TM_EPERM;
    }

    LOG_D("TMSrcPad('%s')::PushData(mSeqNum=%lu)\n",  mName.c_str(), data->mSeqNum);
    TMSinkPad *sinkPad = static_cast<TMSinkPad *>(mPeerPad);

    if (sinkPad == NULL)
    {
        LOG_E("sinkPad is NULL\n");
        return TMResult::TM_EPERM;
    }

    ret = sinkPad->EnqueueData(data);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("mPeerPad->EnqueueData() failed, ret=%d : '%s'\n", ret, TMResult::StrError(ret));
        return TMResult::TM_EPERM;
    }

    return TMResult::TM_OK;
}

int TMSrcPad::PushEvent(TMEvent *event)
{
    if (mPeerPad == NULL)
    {
        LOG_E("mPeerPad is NULL\n");
        return TMResult::TM_PEER_PAD_NOT_FOUND;
    }

    int ret = mPeerPad->ProcessEvent(event);
    if (ret != TMResult::TM_OK)
    {
        LOG_E("Pad('%s')->ProcessEvent(%u) failed, ret=%d: '%s'\n",
               mPeerPad->mName.c_str(), (uint32_t)event->mType, ret, TMResult::StrError(ret));
    }
    return ret;
}

int TMSrcPad::DequeueData(TMData **data, int timeout)
{
    int ret;
    if (mParent != NULL)
    {
        ret = mParent->DequeueData(data);
        if (ret == TMResult::TM_OK)
        {
            return TMResult::TM_OK;
        }
        else
        {
            LOG_E("mParent->DequeueData() failed, ret=%d\n", ret);
            return ret;
        }
    }
    else
    {
        LOG_E("mParent is NULL\n");
        return TMResult::TM_EPERM;
    }
}

int TMSrcPad::ProcessEvent(TMEvent *event)
{
    if (event == NULL)
    {
        LOG_E("event is NULL\n");
        return TMResult::TM_EINVAL;
    }
    if (mParent == NULL)
    {
        LOG_E("mParent is NULL\n");
        return TMResult::TM_EPERM;
    }

    return mParent->ProcessEvent(event);
}

void TMSrcPad::DumpInfo()
{
    cout << endl << "  >>>> Dumping TMSrcPad info begin  >>>>" << endl;
    TMPad::DumpInfo();
    if (mPeerPad != NULL)
        cout << "  Peer Pad  : " << hex << mPeerPad << dec << endl;
    else
        cout << "  Peer Pad  : NULL" << endl;
    cout << "  <<<< Dumping TMSrcPad info end    <<<<" << endl << endl;
}

TMSinkPad::TMSinkPad(TMEntity *entity, TMPad::Param_s *param)
    : TMPad(entity, param), mPeerPad(NULL)
{
    mDirection = Direction::SINK;
}


int TMSinkPad::EnqueueData(TMData *data, int timeout)
{
    int ret;

    if (mParent != NULL)
    {
        ret = mParent->EnqueueData(this, data);
    }
    else
    {
        LOG_W("mParent is NULL\n");
        data->UnRef();
        ret = TMResult::TM_OK;
    }
    return ret;
}

int TMSinkPad::PullData(TMData **data)
{
    if (mPeerPad == NULL)
    {
        LOG_E("Peer pad not exists\n");
        return TMResult::TM_PEER_PAD_NOT_FOUND;
    }

    if (mMode != Mode::PULL && mMode != Mode::AUTO)
    {
        LOG_E("pad is not in PULL or AUTO mode\n");
        return TMResult::TM_EPERM;
    }

    return mPeerPad->DequeueData(data);
}

int TMSinkPad::PushEvent(TMEvent *event)
{
    if (mPeerPad == NULL)
    {
        LOG_E("mPeerPad is NULL\n");
        return TMResult::TM_PEER_PAD_NOT_FOUND;
    }

    return mPeerPad->ProcessEvent(event);
}

int TMSinkPad::ProcessEvent(TMEvent *event)
{
    if (event == NULL)
    {
        LOG_E("event is NULL\n");
        return TMResult::TM_EINVAL;
    }
    if (mParent == NULL)
    {
        LOG_E("mParent is NULL\n");
        return TMResult::TM_EPERM;
    }

    return mParent->ProcessEvent(event);
}

void TMSinkPad::DumpInfo()
{
    cout << endl << "  >>>> Dumping TMSinkPad info begin >>>>" << endl;
    TMPad::DumpInfo();
    if (mPeerPad != NULL)
        cout << "  Peer Pad  : " << hex << mPeerPad << dec << endl;
    else
        cout << "  Peer Pad  : NULL" << endl;
    cout << "  <<<< Dumping TMSinkPad info end   <<<<" << endl << endl;
}


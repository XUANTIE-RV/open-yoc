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
#include <tmedia_core/bind/pipeline.h>

using namespace std;

TMPipeline::TMPipeline(string name)
    : mName(name), mState(State::RESET)
{
}

TMPipeline::~TMPipeline()
{
}

int TMPipeline::AddEntity(initializer_list<TMEntity *> ilEntity)
{
    int count = ilEntity.size();

    LOG_D("entityList count is %d\n", count);
    for (auto iter = ilEntity.begin(); iter != ilEntity.end(); iter++)
    {
        TMEntity *entity = *iter;
        entity->mPipeline = this;
        LOG_D("Add entity '%s'\n", entity->mName.c_str());

        mEntityList.push_back(entity);
    }

    mState = State::READY;
    return TMResult::TM_OK;
}

int TMPipeline::RemoveEntity()
{
    if (mState != State::READY)
    {
        LOG_E("Pipeline state error\n");
        return TMResult::TM_PIPE_OPERATE_FAILED;
    }

    mEntityList.clear();
    mState = State::RESET;
    return TMResult::TM_OK;
}

int TMPipeline::Start()
{
    // Get first entity
    TMEntity *entity = *(mEntityList.begin());
    LOG_D("First entity is '%s'\n", entity->mName.c_str());

    // Start the first entity, and push start event traversing all the entities in pipeline
    if (entity->PipeStart() != TMResult::TM_OK)
    {
        LOG_E("entity('%s')->PipeStart() failed\n", entity->mName.c_str());
        return TMResult::TM_PIPE_OPERATE_FAILED;
    }

    mState = State::RUNNING;
    return TMResult::TM_OK;
}

int TMPipeline::Stop()
{
    TMEntity *entity = *(mEntityList.begin());
    LOG_D("First entity is '%s'\n", entity->mName.c_str());

    if (entity->PipeStop() != TMResult::TM_OK)
    {
        LOG_E("entity('%s')->PipeStop() failed\n", entity->mName.c_str());
        return TMResult::TM_PIPE_OPERATE_FAILED;
    }

    mState = State::READY;
    return TMResult::TM_OK;
}

int TMPipeline::Pause()
{
    TMEntity *entity = *(mEntityList.begin());
    LOG_D("First entity is '%s'\n", entity->mName.c_str());

    if (entity->PipePause() != TMResult::TM_OK)
    {
        LOG_E("entity('%s')->PipePause() failed\n", entity->mName.c_str());
        return TMResult::TM_PIPE_OPERATE_FAILED;
    }

    mState = State::PAUSED;
    return TMResult::TM_OK;
}

int TMPipeline::SubscribeEvents(TMEventSubscriber *subscriber, initializer_list<TMEvent::Type> &ilEvents)
{
    if (mSubScriberEventsMap[subscriber] != NULL)
    {
        delete(mSubScriberEventsMap[subscriber]);
        mSubScriberEventsMap[subscriber] = NULL;
    }

    list<TMEvent::Type> *listEventTypes = new list<TMEvent::Type>;
    for (auto iter = ilEvents.begin(); iter != ilEvents.end(); iter++)
    {
        TMEvent::Type eventType = *iter;
        listEventTypes->push_back(eventType);
    }

    mSubScriberEventsMap[subscriber] = listEventTypes;
    return TMResult::TM_OK;
}

int TMPipeline::UnSubscribeEvents(TMEventSubscriber *subscriber)
{
    // TODO: remove iter from mSubScriberEventsMap, and should delete events list first
    return TMResult::TM_NOT_IMPLEMENTED;
}

int TMPipeline::PushEventToSubscriber(TMEntity *entity, TMEvent *event)
{
    for (auto iter = mSubScriberEventsMap.begin(); iter != mSubScriberEventsMap.end(); iter++)
    {
        list<TMEvent::Type> *listEventTypes = iter->second;
        for (auto iterEvent = listEventTypes->begin(); iterEvent != listEventTypes->end(); iterEvent++)
        {
            TMEvent::Type &eventType = *iterEvent;
            if (eventType == event->mType)
            {
                TMEventSubscriber *subscriber = iter->first;
                subscriber->ProcessEvent(entity, event);
            }
        }
    }
    return TMResult::TM_OK;
}

int TMPipeline::PushEventToEntity(TMEntity *entity, TMEvent *event)
{
    return TMResult::TM_NOT_IMPLEMENTED;
}

TMPipeline::State TMPipeline::GetState()
{
    return mState;
}

void TMPipeline::DumpInfo()
{
    // from first entity in mEntityList;
}


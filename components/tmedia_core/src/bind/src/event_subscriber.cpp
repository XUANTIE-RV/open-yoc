/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#define LOG_LEVEL 3 // Should at the top of all codes

#include <tmedia_core/common/syslog.h>
#include <iostream>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/bind/event_subscriber.h>

using namespace std;

TMEventSubscriber::TMEventSubscriber()
    : mPipeline(NULL), mProcessFunc(NULL)
{
}

TMEventSubscriber::TMEventSubscriber(TMPipeline *pipeline, EventSubscriberProcessFunc func,
                                     initializer_list<TMEvent::Type> &ilEvents)
    : mPipeline(pipeline), mProcessFunc(func)
{
    if (pipeline != NULL)
    {
        pipeline->SubscribeEvents(this, ilEvents);
    }
}

TMEventSubscriber::~TMEventSubscriber()
{
    if (mPipeline != NULL)
    {
        mPipeline->UnSubscribeEvents(this);
    }
}

int TMEventSubscriber::ProcessEvent(TMEntity *entity, TMEvent *event)
{
    if (mProcessFunc == NULL)
    {
        LOG_E("Event process function has not registered\n");
        return TMResult::TM_EVENT_PUSH_FAILED;
    }

    return mProcessFunc(entity, event);
}

int TMEventSubscriber::RegisterEventProcessFunction(EventSubscriberProcessFunc func)
{
    mProcessFunc = func;
    return TMResult::TM_OK;
}


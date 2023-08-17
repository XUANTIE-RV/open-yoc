/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_EVENT_SUBSCRIBER_H
#define TM_EVENT_SUBSCRIBER_H

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include "pipeline.h"

using namespace std;
class TMPipeline;

typedef int (*EventSubscriberProcessFunc)(TMEntity *entity, TMEvent *event);

class TMEventSubscriber
{
public:
    TMEventSubscriber();
    TMEventSubscriber(TMPipeline *pipeline, EventSubscriberProcessFunc func,
                      initializer_list<TMEvent::Type> &ilEvents);
    ~TMEventSubscriber();

    int ProcessEvent(TMEntity *entity, TMEvent *event);
    int RegisterEventProcessFunction(EventSubscriberProcessFunc func);

private:
    TMPipeline *mPipeline;
    EventSubscriberProcessFunc mProcessFunc;
};

#endif  /* TM_EVENT_SUBSCRIBER_H */

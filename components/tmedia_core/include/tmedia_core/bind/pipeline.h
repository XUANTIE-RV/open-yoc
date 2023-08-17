/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_PIPELINE_H
#define TM_PIPELINE_H

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <typeinfo>
#include <list>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/bind/pad.h>
#include <tmedia_core/bind/event.h>
#include <tmedia_core/bind/event_subscriber.h>
#include <tmedia_core/entity/entity.h>

using namespace std;
class TMEventSubscriber;

/* Pipeline state and operate
 *
 *     Open()            Start()
 *   ┌─────────→ READY →──────────────────────────┐
 *   |           ↓ ↑ ↑                            |
 *   ↑   Close() | | | Stop()         Pause()     ↓
 * RESET ←───────┘ | └──────← PAUSED ←──────← RUNNING
 *                 |            ↓     Start()   ↑ ↓
 *                 |            └───────────────┘ |
 *                 |                  Stop()      |
 *                 └──────────────────────────────┘
 */

class TMPipeline
{
public:
    enum class State : uint32_t
    {
        RESET,  // No entity added
        READY,  // All entity added
        PAUSED, // Entity not running and holding data in queue.
        RUNNING // Entity running state, process events and buffers.
    };

    TMPipeline(string name = "");
    virtual ~TMPipeline();

    /* base interface */
    int AddEntity(initializer_list<TMEntity*> ilEntity);
    int RemoveEntity();
    int Start();
    int Stop();
    int Pause();

    /* event interface */
    int SubscribeEvents(TMEventSubscriber *subscriber, initializer_list<TMEvent::Type> &ilEvents);
    int UnSubscribeEvents(TMEventSubscriber *subscriber);

    int PushEventToSubscriber(TMEntity *entity, TMEvent *event);
    int PushEventToEntity(TMEntity *entity, TMEvent *event);

    /* misc interface */
    TMPipeline::State GetState();
    void DumpInfo();

private:
    string mName;
    list<TMEntity *> mEntityList;
    State mState;

    typedef map<TMEventSubscriber *, list<TMEvent::Type> *> SubScriberEventsMap_t;
    SubScriberEventsMap_t mSubScriberEventsMap;
};

#endif  /* TM_PIPELINE_H */

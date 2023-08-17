/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_ENTITY_H
#define TM_ENTITY_H

#include <tmedia_core/bind/pad.h>
#include <tmedia_core/bind/pipeline.h>

using namespace std;
class TMPipeline;

#define TM_STR_BASE(R) #R
#define TM_STR(R)      TM_STR_BASE(R)

typedef void* (*class_new_t)();

namespace TMedia {
    void tmedia_backend_seno_init();
    void tmedia_backend_lgpl_init();
    void tmedia_backend_light_init();
    #define BackendInit() tmedia_backend_seno_init();         \
                          TMedia::tmedia_backend_lgpl_init(); \
                          TMedia::tmedia_backend_light_init();
}

/* Entity state and operate
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

class TMEntity
{
public:
    enum class State : uint32_t
    {
        RESET,  // Entity's default state. In this state, no resource should be allocated.
        READY,  // Entity should get all resources after open.
        PAUSED, // Entity not running and holding data in queue.
        RUNNING // Entity running state, process events and buffers.
    };

    TMEntity(string name);
    virtual ~TMEntity();

    void AttachPool(TMBufferPool *pool);
    void DetachPool();
    TMBufferPool* GetAttachPool();

    string mName;
    virtual int EnqueueData(TMSinkPad *pad, TMData *data, int timeout=-1) {return TMResult::TM_NOT_IMPLEMENTED;}
    virtual int DequeueData(TMData **data, int timeout=-1)                {return TMResult::TM_NOT_IMPLEMENTED;}
    virtual int ProcessEvent(TMEvent *event)                              {return TMResult::TM_NOT_IMPLEMENTED;}

    virtual int PipeStart()  {return TMResult::TM_NOT_IMPLEMENTED;}  // TODO: pure virtual
    virtual int PipePause()  {return TMResult::TM_NOT_IMPLEMENTED;}  // TODO: pure virtual
    virtual int PipeStop()   {return TMResult::TM_NOT_IMPLEMENTED;}  // TODO: pure virtual
    TMEntity::State GetState() {return mState;}

    int PushEventToPipeline(TMEvent *event);

    friend class TMPipeline;

protected:
    virtual int CreatePads()  = 0;
    virtual int DestoryPads() = 0;

    TMBufferPool *mPool;
    bool mPadsCreated;
    TMEntity::State mState;
    TMPipeline *mPipeline;
};

class TMSrcEntity : public TMEntity
{
public:
    TMSrcEntity(string name = "") : TMEntity(name) {}
    virtual ~TMSrcEntity() { DestoryPads(); }
    virtual TMSrcPad *GetSrcPad(int padID = 0);
    int PushEventToAllSinkPads(TMEvent *event);


protected:
    virtual int CreatePads()  override {return TMResult::TM_NOT_IMPLEMENTED;} // TODO: pure virtual
    virtual int DestoryPads() override;
    map<int, TMSrcPad *>mSrcPads;
};

class TMSinkEntity : public TMEntity
{
public:
    TMSinkEntity(string name = "") : TMEntity(name) {}
    virtual ~TMSinkEntity() { DestoryPads(); }
    virtual TMSinkPad *GetSinkPad(int padID = 0);

protected:
    virtual int CreatePads()  override {return TMResult::TM_NOT_IMPLEMENTED;} // TODO: pure virtual
    virtual int DestoryPads() override;
    map<int, TMSinkPad *>mSinkPads;
};

class TMFilterEntity : public TMEntity
{
public:
    TMFilterEntity(string name = "") : TMEntity(name) {}
    virtual ~TMFilterEntity() { DestoryPads(); }

    virtual TMSrcPad *GetSrcPad(int padID = 0);
    virtual TMSinkPad *GetSinkPad(int padID = 0);
    int PushEventToAllSinkPads(TMEvent *event);

protected:
    virtual int CreatePads()  override {return TMResult::TM_NOT_IMPLEMENTED;} // TODO: pure virtual
    virtual int DestoryPads() override;
    map<int, TMSrcPad *>mSrcPads;
    map<int, TMSinkPad *>mSinkPads;
};

#endif  /* TM_ENTITY_H */

/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#ifndef TM_PAD_H
#define TM_PAD_H

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <typeinfo>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/bind/event.h>

using namespace std;

class TMEntity;

class TMPad
{
public:
    enum class Direction : uint32_t
    {
        UNKNOWN,    // Unkonw pin data direction
        SRC,        // Output pin data direction
        SINK        // Input  pin data direction
    };

    enum class Mode : uint32_t
    {
        AUTO,       // Pad will not handle dataflow
        PUSH,       // Pad handles dataflow in downstream push mode
        PULL        // Pad handles dataflow in upstream pull mode
    };

    typedef struct
    {
        string       name;
        Mode         mode;
        TMData::Type data_type;
    } Param_s;

    TMPad(TMEntity *entity, TMPad::Param_s *param);
    virtual ~TMPad();

    // event interface
    int SendEvent(TMEvent *event);   // send event to peer pad, it'll triger ProcessEvent()

    // connection interface
    int GetParent(TMEntity **entity);   // Get parent(entity)

    // state & mode interface
    int  IsActive();
    bool IsSrcPad();
    bool IsSinkPad();

    // misc
    virtual void DumpInfo();

    string mName;
    Mode mMode;
    TMData::Type mDataType;

protected:
    Direction mDirection;

    bool mActive;
    TMEntity *mParent;
};

class TMSinkPad;
class TMSrcPad : public TMPad
{
    friend class TMSinkPad;

public:
    TMSrcPad(TMEntity *entity, TMPad::Param_s *param);
    virtual ~TMSrcPad() {}

    virtual int Bind(TMPad *pad);               // bind with peer Pad
    virtual int UnBind();                       // un-bind with peer Pad

    /* call by parent */
    int PushData(TMData *data);         // push Data to peer Pad, will triger Peer's EnqueueData()
    int PushEvent(TMEvent *event);      // push Event to peer Pad, will triger Peer's ChainEvent()

    /* call by peer */
    int DequeueData(TMData **data, int timeout=-1); // dequeue Data from Parent
    int ProcessEvent(TMEvent *event);               // process Event by Parent

    /* misc */
    void DumpInfo() override;

    TMSinkPad *mPeerPad;
};

class TMSinkPad : public TMPad
{
    friend class TMSrcPad;

public:
    TMSinkPad(TMEntity *entity, TMPad::Param_s *param);
    virtual ~TMSinkPad() {}

    /* call by parent */
    int PullData(TMData **data);        // pull Data from peer Pad, it'll triger EnqueueData()
    int PushEvent(TMEvent *event);      // push Event to peer Pad, will triger Peer's ChainEvent()

    /* call by peer */
    int EnqueueData(TMData *data, int timeout=-1);  // Call by push mode src pad in PushData()
    int ProcessEvent(TMEvent *event);               // process Event by Parent

    /* misc */
    void DumpInfo() override;

protected:
    TMSrcPad *mPeerPad;
};

#endif  /* TM_PAD_H */

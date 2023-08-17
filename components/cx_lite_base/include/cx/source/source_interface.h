/**
 * @file record.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SROUCE_INTERFACE_H
#define CX_SROUCE_INTERFACE_H

#include <memory>
#include <cx/common/frame.h>

namespace cx {
namespace source {

class ChannelSink {
public:
    ChannelSink() {}
    virtual ~ChannelSink() {}

    void SetSrcPadID(int id) { padID = id; }
    int GetSrcPadID() { return padID; }
private:
    int padID;
};

class SrcChannel {
public:
    typedef enum {
        CHANNEL_STATUS_NONE = 0,
        CHANNEL_STATUS_OPEN,
        CHANNEL_STATUS_STARTED,
        CHANNEL_STATUS_STOPED,
        CHANNEL_STATUS_CLOSE,
    } channel_status_e;

    virtual ~SrcChannel() = default;

    virtual int Open(void) = 0;
    virtual int Close(void) = 0;
    virtual int Start(void) = 0;
    virtual int Stop(void) = 0;

    virtual int Bind(ChannelSink &sink) { return 0; }
    virtual int UnBind(ChannelSink &sink) { return 0; }
};


// class SrcManager {
// public:
//     virtual ~SrcManager() = default;

//     virtual int AddConfig(SrcChannelConfig &config) = 0;
//     virtual int CreateChannel(int id, shared_ptr<SrcChannel> &channel) = 0;
// };

}
} 



#endif /* CX_SROUCE_MANAGER_H */


/**
 * @file vo_channel_sink.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SOURCE_VO_CHANNEL_SINK
#define CX_SOURCE_VO_CHANNEL_SINK

#include <cx/source/source_interface.h>
#include <cx/source/entity_wrapper.h>

namespace cx {
namespace source {

class VoChannelSink : public ChannelSink
{
public:
    VoChannelSink() {}
    virtual ~VoChannelSink() {}

    std::unique_ptr<cx::source::VoWrapper>               mVo;
};

}
}
#endif //CX_SOURCE_VO_CHANNEL_SINK
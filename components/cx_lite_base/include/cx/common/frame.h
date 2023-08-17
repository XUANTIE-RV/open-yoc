/**
 * @file frame.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_FRAME_H
#define CX_COMMON_FRAME_H

#include <tmedia_core/common/packet.h>
#include <tmedia_core/common/frame.h>

namespace cx {

struct CxVideoFrame {
    TMVideoFrame data;
};

struct CxPacket {
    TMPacket data;
};

struct CxVideoPacket {
    TMVideoPacket data;
};

struct CxAudioFrame {
    TMAudioFrame data;
};

struct CxAudioPacket {
    TMAudioPacket data;
};

}


#endif /* CX_COMMON_FRAME_H */


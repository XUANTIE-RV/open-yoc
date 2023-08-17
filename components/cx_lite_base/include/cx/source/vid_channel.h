/**
 * @file vid_channel.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SROUCE_VID_CHANNEL_H
#define CX_SROUCE_VID_CHANNEL_H

#include <memory>
#include <vector>
#include <string>
#include <cx/common/frame.h>
#include <cx/common/type.h>
#include <cx/source/entity_wrapper.h>
#include <cx/hardware/board/board_params.h>
#include <cx/source/source_interface.h>
#include <cx/source/cache.hpp>

namespace cx {
namespace source {

struct VidInputConfig {
    struct Vpss {
        std::string label;
        ImageSize   size;
        uint16_t    fps;
        int16_t     rotateAngle;
        int16_t     startFrame;
        CropType    cropType;
        PixelFormat format;
    };

    board::SensorConfig::Tag sensorTag;
    std::vector<Vpss> vpss;

    bool valid() {
        return sensorTag.valid();
    }
};

struct VidChannelConfig {
    VidInputConfig  *vidInput;

    ImageSize       outputSize;
    uint16_t        outputFps;
    VencoderType    encoder;   
    uint16_t        targetRate;
};


class VidChannel final : public SrcChannel {
public:
    VidChannel();
    ~VidChannel();

    virtual int Open(void) override;
    virtual int Close(void) override;
    virtual int Start(void) override;
    virtual int Stop(void) override;
    virtual int Bind(ChannelSink &sink);
    virtual int UnBind(ChannelSink &sink);

    int RecvFrame(shared_ptr<CxVideoFrame> &frame, int timeout);
    int RecvFrame(shared_ptr<CxVideoPacket> &pkt, int timeout);
    void SetPicQuality(int value);
    void SetCameraMode(CameraWorkMode mode);
    void Dump();

    void SetConfig(const VidChannelConfig &config)
    {
	    mConfig = config;
    }

    VidChannelConfig *GetConfig(void)
    {
        return &mConfig;
    }

    channel_status_e mStatus;
private:
    VidChannelConfig                                                 mConfig;
    std::shared_ptr<ViWrapper>                                       mVi;
    std::vector<std::shared_ptr<VpssWrapper>>                        mVpss;
    std::shared_ptr<EncodeWrapper>                                   mEncoder;
    std::shared_ptr<Cache<CxVideoFrame, CxVideoPacket>>              mCache;
    
    enum SinkPoint : uint16_t {
        SINK_VPSS = 0,
        SINK_VENC,
        SINK_JENC,
        SINK_CACHE,
    } mSinkType;
    int16_t vidChannelID;
    friend class VidSrcManager;
    static std::vector<pair<shared_ptr<ViWrapper>, shared_ptr<VpssWrapper>>> gViVpss;
    static std::vector<shared_ptr<VpssWrapper>> gVpss;
    CameraWorkMode mCameraMode;
};

using ChannelPtr = std::shared_ptr<VidChannel>;

}
} 
#endif /* CX_SROUCE_VID_CHANNEL_H */


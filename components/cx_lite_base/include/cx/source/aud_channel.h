/**
 * @file aud_channel.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SROUCE_AUD_CHANNEL_H
#define CX_SROUCE_AUD_CHANNEL_H

#include <cx/source/source_interface.h>
#include <cx/source/cache.hpp>
#include <tmedia_core/entity/codec/codec_inc.h>
#include <cx/source/entity_wrapper.h>

namespace cx {
namespace source {

struct AudChannelConfig {
    AudioEncoderType            encoder;
    AudioSampleBits             sampleBits;
    uint16_t                    sampleRate;
    AudioSampleChannels         sampleChannels;
    AudioPcmDataType            pcmDataType;
};

class AudChannel final : public SrcChannel {
public:
    AudChannel();
    ~AudChannel();

    virtual int Open(void) override;
    virtual int Close(void) override;
    virtual int Start(void) override;
    virtual int Stop(void) override;
    int RecvFrame(shared_ptr<CxAudioFrame> &frame, int timeout);
    int RecvFrame(shared_ptr<CxAudioPacket> &pkt, int timeout);
    void Dump();
    AudChannelConfig   mConfig;
    channel_status_e mStatus;
private:
    AudioInputWrapper                                       mAudIn;
    std::shared_ptr<EncodeWrapper>                          mEncoder;
    std::shared_ptr<Cache<CxAudioFrame, CxAudioPacket>>     mCache;
    
    int16_t audChannelID;
    friend class AudSrcManager;
};

using AudChannelPtr = std::shared_ptr<AudChannel>;

}
} 
#endif /* CX_SROUCE_AUD_CHANNEL_H */

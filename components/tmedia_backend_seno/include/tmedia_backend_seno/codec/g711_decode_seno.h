/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_G711_DECODER_SENO_H
#define TM_G711_DECODER_SENO_H

#include <tmedia_core/entity/codec/codec_inc.h>

class TMG711DecoderSeno : public TMG711Decoder
{
public:
    TMG711DecoderSeno();
    virtual ~TMG711DecoderSeno();

    // TMFilterEntity interface
    TMSrcPad *GetSrcPad(int padID = 0) final override;
    TMSinkPad *GetSinkPad(int padID = 0) final override;

    // TMCodec interface
    int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) final override;
    int SetConfig(TMPropertyList &propList) final override;
    int GetConfig(TMPropertyList &propList) final override;
    int Start() final override;
    int Flush() final override;
    int Stop()  final override;
    int Close() final override;

    // TMAudioDecoder extend interface
    int SendPacket(TMAudioPacket &pkt, int timeout) final override;
    int RecvFrame(TMAudioFrame &frame, int timeout) final override;

private:
    std::vector<shared_ptr<TMAudioFrame>> frameVector;
    G711CodecType_e codecType;
};

#endif  /* TM_G711_DECODER_SENO_H */

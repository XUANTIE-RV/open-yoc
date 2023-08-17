/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_G711_DECODER_H
#define TM_G711_DECODER_H

class TMG711Decoder : public TMAudioDecoder
{
public:
    enum class PropID : uint32_t
    {
        // ID                      Data Type
        CODEC_TYPE,
    };

    TMG711Decoder() {}
    virtual ~TMG711Decoder() {}

    // TMFilterEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;
    virtual TMSinkPad *GetSinkPad(int padID = 0) = 0;

    // TMCodec interface
    virtual int Open(TMCodecParams &codecParam, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Flush() = 0;
    virtual int Stop()  = 0;
    virtual int Close() = 0;

    // TMAudioDecoder extend interface
    virtual int SendPacket(TMAudioPacket &pkt, int timeout) = 0;
    virtual int RecvFrame(TMAudioFrame &frame, int timeout) = 0;
};

#endif /* TM_G711_DECODER_H */

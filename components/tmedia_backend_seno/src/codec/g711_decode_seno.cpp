/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_seno/codec/g711_decode_seno.h>
#include "g711.h"

TMG711DecoderSeno::TMG711DecoderSeno()
{
    codecType = G711_TYPE_UNKNOW;
}

TMG711DecoderSeno::~TMG711DecoderSeno()
{

}

// TMFilterEntity interface
TMSrcPad *TMG711DecoderSeno::GetSrcPad(int padID) 
{
    return nullptr;
}

TMSinkPad *TMG711DecoderSeno::GetSinkPad(int padID) 
{
    return nullptr;
}

// TMCodec interface
int TMG711DecoderSeno::Open(TMCodecParams &codecParam, TMPropertyList *propList) 
{
    int type;
    if (propList->Get(TMG711Decoder::PropID::CODEC_TYPE, &type) != 0) {
        cout << "G711 Decode: unknown type" << endl;
        return TMResult::TM_EINVAL;
    } else {
        cout << "G711 Decode: codec type:" << type << endl;
        codecType = (G711CodecType_e)type;
    }
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::SetConfig(TMPropertyList &propList) 
{
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::GetConfig(TMPropertyList &propList) 
{
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::Start() 
{
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::Flush() 
{   
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::Stop()  
{
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::Close() 
{
    return TMResult::TM_OK;
}

// TMAudioDecoder extend interface
int TMG711DecoderSeno::SendPacket(TMAudioPacket &pkt, int timeout) 
{
    int sampleChannel = (pkt.mSampleChannels == AUDIO_SAMPLE_CHANNEL_MONO ? 1 : 2);
    int sampleBits = (pkt.mSampleBits == AUDIO_SAMPLE_BITS_8BIT ? 1 : 2);

    shared_ptr<TMAudioFrame> frame = make_shared<TMAudioFrame>();
    frame->Init();
    frame->mSampleChannels = pkt.mSampleChannels;
    frame->mSampleBits = pkt.mSampleBits;
    frame->mPcmDataType = pkt.mPcmDataType;
    frame->mSampleCount = pkt.mDataLength*2/(sampleChannel*sampleBits);
    frame->mPTS = pkt.mPTS;
    frame->mSampleRate = pkt.mSampleRate;

    frame->PrepareBuffer(TMBUFFER_TYPE_USER_MALLOC, 0, 0, 32);

    if(codecType == G711_TYPE_A) {
        if(sampleChannel == 1) {
            for(unsigned int i=0; i<pkt.mDataLength; i++) {
                *(short *)(frame->mData[0]+2*i) = alaw2linear(pkt.mData[i]);
            }
        } else if(sampleChannel == 2) {
            for(unsigned int i=0; i<pkt.mDataLength/2; i++) {
                *(short *)(frame->mData[0]+2*i) = alaw2linear(pkt.mData[i]);
            }
            for(unsigned int i=0; i<pkt.mDataLength/2; i++) {
                *(short *)(frame->mData[1]+2*i) = alaw2linear(pkt.mData[pkt.mDataLength/2+i]);
            }
        }
    } else if(codecType == G711_TYPE_U) {
        if(sampleChannel == 1) {
            for(unsigned int i=0; i<pkt.mDataLength; i++) {
                *(short *)(frame->mData[0]+2*i) = ulaw2linear(pkt.mData[i]);
            }
        } else if(sampleChannel == 2) {
            for(unsigned int i=0; i<pkt.mDataLength/2; i++) {
                *(short *)(frame->mData[0]+2*i) = ulaw2linear(pkt.mData[i]);
            }
            for(unsigned int i=0; i<pkt.mDataLength/2; i++) {
                *(short *)(frame->mData[1]+2*i) = ulaw2linear(pkt.mData[pkt.mDataLength/2+i]);
            }
        }
    }
    
    frameVector.push_back(frame);

    pkt.UnRef();
    return TMResult::TM_OK;
}

int TMG711DecoderSeno::RecvFrame(TMAudioFrame &frame, int timeout) 
{
    if(frameVector.size() == 0) {
        return TMResult::TM_EAGAIN;
    }
    frame.MoveRef(frameVector[0].get());
    vector<shared_ptr<TMAudioFrame>>::iterator first = frameVector.begin();
	frameVector.erase(first);
    return TMResult::TM_OK;
}

REGISTER_AUDIO_DECODER_CLASS(TMMediaInfo::CodecID::G711, TMG711DecoderSeno)

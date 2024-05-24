/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_seno/codec/g711_encode_seno.h>
#include "g711.h"

TMG711EncoderSeno::TMG711EncoderSeno()
{
    codecType = G711_TYPE_UNKNOW;
}

TMG711EncoderSeno::~TMG711EncoderSeno()
{

}

// TMFilterEntity interface
TMSrcPad *TMG711EncoderSeno::GetSrcPad(int padID) 
{
    return nullptr;
}

TMSinkPad *TMG711EncoderSeno::GetSinkPad(int padID) 
{
    return nullptr;
}

// TMCodec interface
int TMG711EncoderSeno::Open(TMCodecParams &codecParam, TMPropertyList *propList) 
{
    int type;
    if (propList->Get(TMG711Encoder::PropID::CODEC_TYPE, &type) != 0) {
        cout << "G711 Encode: unknown type" << endl;
        return TMResult::TM_EINVAL;
    } else {
        cout << "G711 Encode: codec type:" << type << endl;
        codecType = (G711CodecType_e)type;
    }
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::SetConfig(TMPropertyList &propList) 
{
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::GetConfig(TMPropertyList &propList) 
{
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::Start() 
{
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::Flush() 
{   
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::Stop()  
{
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::Close() 
{
    return TMResult::TM_OK;
}

// TMAudioEncoder extend interface
int TMG711EncoderSeno::SendFrame(TMAudioFrame &frame, int timeout) 
{
    int sampleChannel = (frame.mSampleChannels == AUDIO_SAMPLE_CHANNEL_MONO ? 1 : 2);
    int sampleBits = (frame.mSampleBits == AUDIO_SAMPLE_BITS_8BIT ? 1 : 2);
    int totalSize = AUDIO_ALIGN(sampleBits * frame.mSampleCount, 32) * sampleChannel;

    shared_ptr<TMAudioPacket> pkt = make_shared<TMAudioPacket>();
    pkt->Init();
    pkt->PrepareBuffer(totalSize/2);
    pkt->mDataLength = 0;
    pkt->mPTS = frame.mPTS;

    unsigned char buffer[totalSize/2];
    for(int i=0; i<totalSize/2; i++) {
        if(codecType == G711_TYPE_A) {
            buffer[i] = linear2alaw(*(short*)(frame.mData[0]+2*i));
        } else if(codecType == G711_TYPE_U) {
            buffer[i] = linear2ulaw(*(short*)(frame.mData[0]+2*i));
        }
    }
    pkt->Append(buffer, totalSize/2);
    pktVector.push_back(pkt);

    frame.UnRef();
    return TMResult::TM_OK;
}

int TMG711EncoderSeno::RecvPacket(TMAudioPacket &pkt, int timeout) 
{
    if(pktVector.size() == 0) {
        return TMResult::TM_EAGAIN;
    }
    pkt.MoveRef(pktVector[0].get());
    vector<shared_ptr<TMAudioPacket>>::iterator first = pktVector.begin();
	pktVector.erase(first);
    return TMResult::TM_OK;
}

REGISTER_AUDIO_ENCODER_CLASS(TMMediaInfo::CodecID::G711, TMG711EncoderSeno)

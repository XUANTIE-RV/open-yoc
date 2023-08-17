/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include "link_visual_struct.h"

#include <cx/cloud_lv_service.h>
#include <lv_internal_api.h>
#include <tmedia_core/entity/format/format_inc.h>
#include <tmedia_core/entity/codec/codec_inc.h>
#include <cx/source/vid_manager.h>
#include <cx/source/aud_manager.h>
#include "lv_util.h"
#include <cx/common/util.h>

#define TAG "cloud_lv_internal"

using namespace std;

extern shared_ptr<cx::source::VidChannel> channelLiveHD, channelLiveSD, channelLiveLD;
extern shared_ptr<cx::source::AudChannel> channelLiveAudio;

/* internal interface*/

static int cloud_dir_save = LV_INTERCOM_DIR_FROM_CLOUD;
int linkvisual_switch_intercom(void *hdl, int dir_to_cloud)
{
    CX_LOGD(TAG, "%s %d", __func__, dir_to_cloud);
    cloud_dir_save = dir_to_cloud;
    return 0;
}

int linkvisual_get_intercom_dir(void)
{
    return cloud_dir_save;
}

void linkvisual_audio_play(const lv_on_push_streaming_data_param_s *param) 
{
    static int entityCreate = 1;
    static TMAudioDecoder *g711Decoder;
    static TMAudioOutput *audioOutEntity;
    if(entityCreate) {
/* create g711 decode entity */
        g711Decoder = TMAudioDecoderFactory::CreateEntity(TMMediaInfo::CodecID::G711);
        TMAudioEncoderParams codecParam;
        TMPropertyList propList;
        propList.Reset();
        propList.Assign(TMG711Encoder::PropID::CODEC_TYPE, G711_TYPE_A);
        g711Decoder->Open(codecParam, &propList);

/* create audio output entity */
        audioOutEntity = TMFormatAudioOutputFactory::CreateEntity(TMMediaInfo::DeviceID::SPEAKER);
        propList.Reset();			
        propList.Assign(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_RATE, 16000);			
        propList.Assign(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_BITS, AUDIO_SAMPLE_BITS_16BIT);			
        propList.Assign(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_CHANNELS, AUDIO_SAMPLE_CHANNEL_STEREO);			
        propList.Assign(TMAudioOutput::PropID::AUDIO_OUT_DATA_TYPE, AUDIO_PCM_ACCESS_RW_INTERLEAVED);

        audioOutEntity->Open("AudioIn", &propList);
        audioOutEntity->Start();
        entityCreate = 0;
    }
/* g711 decode */
    TMAudioPacket pkt;
    pkt.PrepareBuffer(param->len);
    pkt.mDataLength = 0;
    pkt.mSampleBits = cx::MapAudioSampleBits(lv::MapAudioSampleBits_LV2CX(param->audio_param->sample_bits));
    pkt.mSampleChannels = cx::MapAudioSampleChannels(lv::MapAudioSampleChannels_LV2CX(param->audio_param->channel));
    pkt.mPcmDataType = cx::MapAudioPcmDataType(cx::AUD_PCM_ACCESS_RW_NONINTERLEAVED);
    pkt.mSampleRate = lv::MapAudioSampleRate_LV2CX(param->audio_param->sample_rate);
    pkt.Append((uint8_t*)param->p, param->len);

    if(lv::MapAudioEncodeType_LV2CX(param->audio_param->format) == cx::AENC_G711A) {
        g711Decoder->SendPacket(pkt, 100);
        TMAudioFrame frame;
        if(g711Decoder->RecvFrame(frame, 100) == TMResult::TM_OK) {
/* audio output */
            audioOutEntity->SendFrame(frame, 100);
            frame.UnRef();
        }
    } else {
        CX_LOGE(TAG, "encoder type:%d not support", lv::MapAudioEncodeType_LV2CX(param->audio_param->format));
    }
}

static int currentClarity = 2;
static shared_ptr<cx::source::VidChannel> &GetCurrentVideoChan()
{
    if(currentClarity == 0) {
        return channelLiveLD;
    } else if(currentClarity == 1) {
        return channelLiveSD;
    } else {
        return channelLiveHD;
    }
}
void linkvisual_set_live_clarity(int clarity)
{
    if(GetCurrentVideoChan()->mStatus == cx::source::SrcChannel::CHANNEL_STATUS_STARTED) {
        linkvisual_start_video_encode(0);
        linkvisual_start_audio_encode(0);
        currentClarity = clarity;
        linkvisual_start_video_encode(1);
        linkvisual_start_audio_encode(1);
    } else {
       currentClarity = clarity; 
    }
}


int linkvisual_start_video_encode(int start) 
{
    if(start) {
        if(GetCurrentVideoChan()->mStatus != cx::source::SrcChannel::CHANNEL_STATUS_OPEN) {
            GetCurrentVideoChan()->Open();
        }
        GetCurrentVideoChan()->Start();
    } else {
        GetCurrentVideoChan()->Stop();
    }
    return 0;
}

int linkvisual_start_audio_encode(int start) 
{
    if(start) {
        if(channelLiveAudio->mStatus != cx::source::SrcChannel::CHANNEL_STATUS_OPEN) {
            channelLiveAudio->Open();
        }
        channelLiveAudio->Start();
    } else {
        channelLiveAudio->Stop();
    }
    return 0;
}

int linkvisual_get_video_attributes(lv_video_param_s *param)
{
    param->format = lv::MapEncodeFormat_CX2LV(GetCurrentVideoChan()->GetConfig()->encoder);
    param->fps = GetCurrentVideoChan()->GetConfig()->outputFps;

    return 0;
}

int linkvisual_get_audio_attributes(lv_audio_param_s *param)
{
    param->format = lv::MapAudioEncodeType_CX2LV(channelLiveAudio->mConfig.encoder);
    param->sample_rate = lv::MapAudioSampleRate_CX2LV(channelLiveAudio->mConfig.sampleRate);
    param->sample_bits = lv::MapAudioSampleBits_CX2LV(channelLiveAudio->mConfig.sampleBits);
    param->channel = lv::MapAudioSampleChannels_CX2LV(channelLiveAudio->mConfig.sampleChannels);
    return 0;
}

int linkvisual_get_encoded_video(void *data, uint32_t max_size, int *is_iframe, long long *timestamp)
{
    int len;
    shared_ptr<cx::CxVideoPacket> pkt = make_shared<cx::CxVideoPacket>();
    TMBuffer* dataBuf = TMBuffer_NewFromData(data, max_size, NULL, NULL);
    pkt->data.SetBuffer(dataBuf);
    TMBuffer_UnRef(dataBuf);
    pkt->data.mDataLength = 0;

    len = GetCurrentVideoChan()->RecvFrame(pkt, 40);
    if(len < 0) {
        return len;
    }

    *is_iframe = lv::MapVideoFrameType_CX2LV(pkt->data.mPictureType);
    *timestamp = pkt->data.mPTS.Get().timestamp/1000;
    len = pkt->data.mDataLength;

    pkt->data.UnRef();
    
    return len;
}

int linkvisual_get_encoded_audio(void *data, uint32_t max_size, long long *timestamp)
{
    int ret;
    shared_ptr<cx::CxAudioPacket> packet = make_shared<cx::CxAudioPacket>();
    ret = channelLiveAudio->RecvFrame(packet, 40);
    if(ret < 0) {
        return ret;
    }
    ret = packet->data.mDataLength;

    *timestamp = packet->data.mPTS.Get().timestamp/1000;
    if(max_size < (uint32_t)ret) {
        CX_LOGD(TAG, "size too big %d", ret);
        return -1;
    }
    memcpy(data, packet->data.mData, ret);
    packet->data.UnRef();
    return ret;
}
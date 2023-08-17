/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <tmedia_backend_cvitek/cvi_audio_output/audio_output_cvi.h>
#include <tmedia_core/entity/format/format_factory.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"

TMAudioOutputCvi::TMAudioOutputCvi()
{
    mStart = 0;
    pthread_mutex_init(&mMutex, NULL);
    mCurrentPropertyList.Reset();
    mRingBuffer = new TMUtilRingBuffer(50*1024);
}

TMAudioOutputCvi::~TMAudioOutputCvi()
{
    pthread_mutex_destroy(&mMutex);
    delete mRingBuffer;
}

TMSinkPad *TMAudioOutputCvi::GetSinkPad(int padID)  
{
	return nullptr;
}

void *TMAudioOutputCvi::Task(void *args)
{
    int ret;
    TMAudioOutputCvi *p = reinterpret_cast<TMAudioOutputCvi *> (args);
    char *outputBuffer = new char[AUDIO_PERIOD_SIZE*4];

    TMEDIA_PRINTF("Audio Output task start\n");
    while (1) {
        if(p->mStart) {
            int rbCount = p->mRingBuffer->AvaliableReadSpace();
            if(rbCount >= AUDIO_PERIOD_SIZE*4) {
                ret = p->mRingBuffer->Read(outputBuffer, AUDIO_PERIOD_SIZE*4);
            } else {
                ret = AUDIO_PERIOD_SIZE*4;
                memset(outputBuffer, 0, AUDIO_PERIOD_SIZE*4);
            }
            aos_pcm_writei(p->playback_handle, outputBuffer, aos_pcm_bytes_to_frames(p->playback_handle, ret));
        } else {
            usleep(20000);
        }
    }
    delete outputBuffer;
    return NULL;
}

int TMAudioOutputCvi::Open(string deviceName, TMPropertyList *propList) 
{
    int sampleRate;		    
    AudioSampleBits_e       sampleBits;		
    AudioSampleChannels_e   sampleChannels;		
    AudioPcmDataType_e      pcmDataType;		
    propList->Get(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_RATE, (int *)&sampleRate);
    propList->Get(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_BITS, (int *)&sampleBits);		
    propList->Get(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_CHANNELS, (int *)&sampleChannels);		
    propList->Get(TMAudioOutput::PropID::AUDIO_OUT_DATA_TYPE, (int *)&pcmDataType);

    mCurrentPropertyList.Assign(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_RATE, sampleRate);
    mCurrentPropertyList.Assign(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_BITS, sampleBits);
    mCurrentPropertyList.Assign(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_CHANNELS, sampleChannels);
    mCurrentPropertyList.Assign(TMAudioOutput::PropID::AUDIO_OUT_DATA_TYPE, pcmDataType);

    TMEDIA_PRINTF("Audio Output open: SampleRate:%d SampleBits:%d SampleChannels:%d PcmDataType:%d\n", sampleRate, sampleBits, sampleChannels, pcmDataType);

/* create audio output task */
    pthread_t       tid;
    pthread_attr_t  attr;
    const char *name = "audio_output";
    
    int ret = pthread_attr_init(&attr);
    if (ret < 0) {
        TMEDIA_PRINTF("Audio Output: Create thread attr failed, ret = %d\n", ret);
        return -1;
    }
    attr.sched_priority -= 5;
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret < 0) {
        TMEDIA_PRINTF("Audio Output: Set thread attr failed, ret = %d\n", ret);
        return -1;
    }
    ret = pthread_create(&tid, &attr, &Task, this);
    if (ret != 0) {
        TMEDIA_PRINTF("Audio Output: Create thread failed, ret = %d\n", ret);
        return -1;
    }
    pthread_setname_np(tid, name);

    return TMResult::TM_OK;
}

int TMAudioOutputCvi::Close()                                                  
{
    TMEDIA_PRINTF("Audio Output close\n");
    return TMResult::TM_OK;
}

int TMAudioOutputCvi::SetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMAudioOutputCvi::GetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMAudioOutputCvi::Start()                                                  
{
    pthread_mutex_lock(&mMutex);
    TMEDIA_PRINTF("Audio Output start\n");

    unsigned int rate = 16000;
    aos_pcm_hw_params_t *playback_hw_params;
    aos_pcm_open (&playback_handle, "pcmP0", AOS_PCM_STREAM_PLAYBACK, 0); 
    aos_pcm_hw_params_alloca(&playback_hw_params); 
    aos_pcm_hw_params_any(playback_handle, playback_hw_params); 
    playback_hw_params->period_size = AUDIO_PERIOD_SIZE;
    playback_hw_params->buffer_size = AUDIO_PERIOD_SIZE * 4;
    aos_pcm_hw_params_set_access(playback_handle, playback_hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED); 
    aos_pcm_hw_params_set_format(playback_handle, playback_hw_params, 16); 
    aos_pcm_hw_params_set_rate_near(playback_handle, playback_hw_params, &rate, NULL);
    aos_pcm_hw_params_set_channels(playback_handle, playback_hw_params, 2);
    aos_pcm_hw_params(playback_handle, playback_hw_params);

    mRingBuffer->Clear();
    mStart = 1;
    pthread_mutex_unlock(&mMutex);
    return TMResult::TM_OK;
}

int TMAudioOutputCvi::Stop()                                                   
{
    pthread_mutex_lock(&mMutex);
    mStart = 0;
    aos_pcm_close(playback_handle);
    pthread_mutex_unlock(&mMutex);
    TMEDIA_PRINTF("Audio Output stop\n");
    return TMResult::TM_OK;
}

int TMAudioOutputCvi::SendFrame(TMAudioFrame &frame, int timeout)              
{
    pthread_mutex_lock(&mMutex);
    if(mStart == 0) {
        TMEDIA_PRINTF("Video Output not start\n");
        pthread_mutex_unlock(&mMutex);
        return TMResult::TM_STATE_ERROR;
    }

    int sampleRate;		    
    AudioSampleBits_e       sampleBits;		
    AudioSampleChannels_e   sampleChannels;		
    AudioPcmDataType_e      pcmDataType;		
    mCurrentPropertyList.Get(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_RATE, (int *)&sampleRate);
    mCurrentPropertyList.Get(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_BITS, (int *)&sampleBits);		
    mCurrentPropertyList.Get(TMAudioOutput::PropID::AUDIO_OUT_SAMPLE_CHANNELS, (int *)&sampleChannels);		
    mCurrentPropertyList.Get(TMAudioOutput::PropID::AUDIO_OUT_DATA_TYPE, (int *)&pcmDataType);

    if(sampleBits != AUDIO_SAMPLE_BITS_16BIT) {
        return TMResult::TM_NOT_SUPPORT;
    }
    int totalSize = AUDIO_ALIGN((frame.mSampleBits == AUDIO_SAMPLE_BITS_8BIT ? 1 : 2) * frame.mSampleCount, 32) * (frame.mSampleChannels == AUDIO_SAMPLE_CHANNEL_MONO ? 1 : 2);
    int length = 0;
    short convertBuff[totalSize];
    if(frame.mSampleChannels == AUDIO_SAMPLE_CHANNEL_MONO) {
        for(int i=0; i<totalSize/2; i++) {
            convertBuff[i * 2] = *((short *)(frame.mData[0]+i*2));
            convertBuff[i * 2 + 1] = convertBuff[i * 2];
        }
        length = totalSize*2;
    } else if(frame.mSampleChannels == AUDIO_SAMPLE_CHANNEL_STEREO && pcmDataType == AUDIO_PCM_ACCESS_RW_INTERLEAVED) {
        memcpy(convertBuff, frame.mData[0], totalSize);
        length = totalSize;
    } else if(frame.mSampleChannels == AUDIO_SAMPLE_CHANNEL_STEREO && pcmDataType == AUDIO_PCM_ACCESS_RW_NONINTERLEAVED) {
        for(int i=0; i<totalSize/4; i++) {
            convertBuff[i * 2] = *((short *)(frame.mData[0]+i*2));
            convertBuff[i * 2 + 1] = *((short *)(frame.mData[1] + i*2));
        }
        length = totalSize;
    }
    if(frame.mSampleRate == sampleRate) {
        mRingBuffer->Write((char *)convertBuff, length);
    } else if(frame.mSampleRate < sampleRate) {
        int step = sampleRate/frame.mSampleRate;
        short FreqAdjustBuffer[length*step/2];
        for(int i=0; i<length/2; i++) {
            for(int j=0; j<step; j++) {
                FreqAdjustBuffer[i*step+j] =  convertBuff[i];
            }
        }
        mRingBuffer->Write((char *)FreqAdjustBuffer, length*step);
    } else if(frame.mSampleRate > sampleRate) {
        int step = frame.mSampleRate/sampleRate;
        short FreqAdjustBuffer[length/(step*2)];
        for(int i=0; i<length/2; i++) {
            FreqAdjustBuffer[i] =  convertBuff[i*step];
        }
        mRingBuffer->Write((char *)FreqAdjustBuffer, length/step);
    }

    pthread_mutex_unlock(&mMutex);
    return TMResult::TM_OK;
}

REGISTER_AUDIO_OUTPUT_CLASS(TMMediaInfo::DeviceID::SPEAKER, TMAudioOutputCvi)
/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <tmedia_backend_cvitek/cvi_audio_input/audio_input_cvi.h>
#include <tmedia_core/entity/format/format_factory.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include <alsa/pcm.h>
#include "cviaudio_algo_interface.h"
#include "cvi_comm_aio.h"

TMAudioInputCvi::TMAudioInputCvi()
{
    mStart = 0;
    mCurrentPropertyList.Reset();
    pthread_mutex_init(&mMutex, NULL);

    micDataIn = (short *)malloc(CVIAUDIO_AEC_LENGTH * 2 * 2);   //晶视3A算法每次只能处理10ms数据 且需要吃两倍内存!
    refDataIn =  (short *)malloc(CVIAUDIO_AEC_LENGTH * 2 * 2);
    algoDataOut =  (short *)malloc(PERIOD_FRAMES_SIZE * 2 * 2);
}

TMAudioInputCvi::~TMAudioInputCvi()
{
    pthread_mutex_destroy(&mMutex);
    free(micDataIn);
    free(refDataIn);
    free(algoDataOut);
}

TMSrcPad *TMAudioInputCvi::GetSrcPad(int padID)  
{
	return nullptr;
}

int TMAudioInputCvi::Open(string deviceName, TMPropertyList *propList) 
{
    int sampleRate;		    
    AudioSampleBits_e       sampleBits;		
    AudioSampleChannels_e   sampleChannels;		
    AudioPcmDataType_e      pcmDataType;		
    propList->Get(TMAudioInput::PropID::AUDIO_IN_SAMPLE_RATE, (int *)&sampleRate);
    propList->Get(TMAudioInput::PropID::AUDIO_IN_SAMPLE_BITS, (int *)&sampleBits);
    propList->Get(TMAudioInput::PropID::AUDIO_IN_SAMPLE_CHANNELS, (int *)&sampleChannels);
    propList->Get(TMAudioInput::PropID::AUDIO_IN_DATA_TYPE, (int *)&pcmDataType);

    if(sampleBits == AUDIO_SAMPLE_BITS_8BIT || sampleBits == AUDIO_SAMPLE_BITS_UNKNOW) {
        printf("Audio Input not support sampleBits:%d\n", sampleBits);
        return TMResult::TM_NOT_SUPPORT;
    }

    if(sampleChannels != AUDIO_SAMPLE_CHANNEL_MONO) {
        printf("Audio Input not support sampleChannels:%d\n", sampleChannels);
        return TMResult::TM_NOT_SUPPORT;
    }

    mCurrentPropertyList.Assign(TMAudioInput::PropID::AUDIO_IN_SAMPLE_RATE, sampleRate);
    mCurrentPropertyList.Assign(TMAudioInput::PropID::AUDIO_IN_SAMPLE_BITS, sampleBits);
    mCurrentPropertyList.Assign(TMAudioInput::PropID::AUDIO_IN_SAMPLE_CHANNELS, sampleChannels);
    mCurrentPropertyList.Assign(TMAudioInput::PropID::AUDIO_IN_DATA_TYPE, pcmDataType);

    TMEDIA_PRINTF("Audio Input open: SampleRate:%d SampleBits:%d SampleChannels:%d PcmDataType:%d\n", sampleRate, sampleBits, sampleChannels, pcmDataType);
    
    AI_TALKVQE_CONFIG_S stVqeConfig;
    AI_TALKVQE_CONFIG_S *pstVqeConfig = &stVqeConfig;
    pstVqeConfig->para_client_config = 0;
    pstVqeConfig->u32OpenMask = AI_TALKVQE_MASK_AEC | AI_TALKVQE_MASK_AGC | AI_TALKVQE_MASK_ANR;
    pstVqeConfig->s32WorkSampleRate = SAMPLE_RATE;
    pstVqeConfig->s32RevMask = 0;
    pstVqeConfig->para_notch_freq = 0;
    /* AEC */
    pstVqeConfig->stAecCfg.para_aec_filter_len = 13;
    pstVqeConfig->stAecCfg.para_aes_std_thrd = 37;
    pstVqeConfig->stAecCfg.para_aes_supp_coeff = 60;
    /* ANR */
    pstVqeConfig->stAnrCfg.para_nr_snr_coeff = 15;
    pstVqeConfig->stAnrCfg.para_nr_init_sile_time = 0;

    /* AGC */
    pstVqeConfig->stAgcCfg.para_agc_max_gain = 0;
    pstVqeConfig->stAgcCfg.para_agc_target_high = 2;
    pstVqeConfig->stAgcCfg.para_agc_target_low = 72;
    pstVqeConfig->stAgcCfg.para_agc_vad_ena = 0;

    pstVqeConfig->stAecDelayCfg.para_aec_init_filter_len = 2;
    pstVqeConfig->stAecDelayCfg.para_dg_target = 1;
    pstVqeConfig->stAecDelayCfg.para_delay_sample = 1;
    audioAlgoHandle = CviAud_Algo_Init(pstVqeConfig->u32OpenMask, pstVqeConfig);
    if (!audioAlgoHandle) {
        printf("ssp init fail\n");
    }
    return TMResult::TM_OK;
}

int TMAudioInputCvi::Close()                                                  
{
    TMEDIA_PRINTF("Audio Input close\n");
    CviAud_Algo_DeInit(audioAlgoHandle);
    return TMResult::TM_OK;
}

int TMAudioInputCvi::SetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMAudioInputCvi::GetConfig(TMPropertyList &propList)                      
{
    return TMResult::TM_OK;
}

int TMAudioInputCvi::Start()                                                  
{
    pthread_mutex_lock(&mMutex);
    TMEDIA_PRINTF("Audio Input start\n");

    unsigned int sampleRate = SAMPLE_RATE;
    aos_pcm_hw_params_t *capture_hw_params;
    aos_pcm_open (&capture_handle, "pcmC0", AOS_PCM_STREAM_CAPTURE, 0);
    aos_pcm_hw_params_alloca (&capture_hw_params);
    aos_pcm_hw_params_any (capture_handle, capture_hw_params);
    capture_hw_params->period_size = PERIOD_FRAMES_SIZE;
    capture_hw_params->buffer_size = PERIOD_FRAMES_SIZE;
    aos_pcm_hw_params_set_access (capture_handle, capture_hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED);// 设置音频数据参数为交错模式
    aos_pcm_hw_params_set_format (capture_handle, capture_hw_params, 16);//设置音频数据参数为小端16bit
    aos_pcm_hw_params_set_rate_near (capture_handle, capture_hw_params, &sampleRate, NULL);//设置音频数据参数采样率为16K
    aos_pcm_hw_params_set_channels (capture_handle, capture_hw_params, AUDIO_CHANNEL);//设置音频数据参数为2通道
    aos_pcm_hw_params (capture_handle, capture_hw_params);//设置硬件参数到具体硬件中
    mStart = 1;
    pthread_mutex_unlock(&mMutex);
    return TMResult::TM_OK;
}

int TMAudioInputCvi::Stop()                                                   
{
    pthread_mutex_lock(&mMutex);
    mStart = 0;
    TMEDIA_PRINTF("Audio Input stop\n");
    aos_pcm_close(capture_handle); //关闭设备
    pthread_mutex_unlock(&mMutex);
    return TMResult::TM_OK;
}

int TMAudioInputCvi::RecvFrame(TMAudioFrame &frame, int timeout)              
{
    pthread_mutex_lock(&mMutex);
    if(mStart == 0) {
        TMEDIA_PRINTF("Audio Input not start\n");
        pthread_mutex_unlock(&mMutex);
        return -1;
    }

    int RetframeLen;
    int sampleRate;		    
    AudioSampleBits_e       sampleBits;		
    AudioSampleChannels_e   sampleChannels;		
    AudioPcmDataType_e      pcmDataType;		
    mCurrentPropertyList.Get(TMAudioInput::PropID::AUDIO_IN_SAMPLE_RATE, (int *)&sampleRate);
    mCurrentPropertyList.Get(TMAudioInput::PropID::AUDIO_IN_SAMPLE_BITS, (int *)&sampleBits);		
    mCurrentPropertyList.Get(TMAudioInput::PropID::AUDIO_IN_SAMPLE_CHANNELS, (int *)&sampleChannels);		
    mCurrentPropertyList.Get(TMAudioInput::PropID::AUDIO_IN_DATA_TYPE, (int *)&pcmDataType);

    frame.mSampleCount = AUDIO_PERIOD_TIME * sampleRate / 1000;
    frame.mSampleBits = sampleBits;
    frame.mSampleChannels = sampleChannels;
    frame.mPcmDataType = pcmDataType;

    struct timespec     ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    TMClock_t pts;
    pts.timestamp = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    pts.time_base = 1000000;
    frame.mPTS.Set(pts);
    
    if(frame.mData[0] == NULL) {
        int ret = frame.PrepareBuffer(TMBUFFER_TYPE_USER_MALLOC, 0, 0, 0);
        if(ret != TMResult::TM_OK) {
            TMEDIA_PRINTF("Audio prepare buffer err:%d\n", ret);
            pthread_mutex_unlock(&mMutex);
            return -1;
        }
    }

    int16_t audio_data_recv[AUDIO_PERIOD_TIME * (SAMPLE_RATE/1000) * AUDIO_CHANNEL  * 2 / 2];
    int ret;

    // FIXME: workaround for cvitek bug, aos_pcm_readi would return 0 when no enough frames
    do {
        ret = aos_pcm_readi(capture_handle, audio_data_recv, PERIOD_FRAMES_SIZE);  //输出为 双通道 16bits 交织数据 
        if (ret == 0) {
            usleep(1000);
        }
    } while (ret == 0);

    for(int i = 0; i< AUDIO_PERIOD_TIME/10; i++) {  //每次处理10ms
        for (int j = 0; j < CVIAUDIO_AEC_LENGTH; j++) {
            micDataIn[j] = audio_data_recv[j * 2 + CVIAUDIO_AEC_LENGTH*i*2];
            refDataIn[j] = audio_data_recv[j * 2 + 1 + CVIAUDIO_AEC_LENGTH*i*2];
        }
        RetframeLen = CviAud_Algo_Process(audioAlgoHandle, micDataIn, refDataIn, algoDataOut+CVIAUDIO_AEC_LENGTH*i, CVIAUDIO_AEC_LENGTH);
        if (RetframeLen != CVIAUDIO_AEC_LENGTH) {
            printf("[aec] ssp process fail\n");
            return -1;
        }
    }
    int step = 1;
    if(sampleRate < SAMPLE_RATE) {
        step = SAMPLE_RATE/sampleRate;
        for(int i = 0; i < PERIOD_FRAMES_SIZE/step; i++) {
            algoDataOut[i] = algoDataOut[i * step];
        }
    }
    memcpy(frame.mData[0], algoDataOut, PERIOD_FRAMES_SIZE*2/step);
    pthread_mutex_unlock(&mMutex);
    return TMResult::TM_OK;
}

REGISTER_AUDIO_INPUT_CLASS(TMMediaInfo::DeviceID::MIC, TMAudioInputCvi)
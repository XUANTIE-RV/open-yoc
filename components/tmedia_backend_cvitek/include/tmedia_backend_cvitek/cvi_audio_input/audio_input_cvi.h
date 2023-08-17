/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_CVI_AUDIO_INPUT_H
#define TM_CVI_AUDIO_INPUT_H

#include <tmedia_core/entity/format/format_io.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"

class  TMAudioInputCvi: public TMAudioInput 
{
public:

    TMAudioInputCvi();
    virtual ~TMAudioInputCvi();

    // TMSrcEntity interface
    TMSrcPad  *GetSrcPad(int padID = 0) final override;

    // TMAudioInput interface
    int Open(string deviceName, TMPropertyList *propList = NULL) final override;
    int Close()                                                  final override;
    int SetConfig(TMPropertyList &propList)                      final override;
    int GetConfig(TMPropertyList &propList)                      final override;
    int Start()                                                  final override;
    int Stop()                                                   final override;
    int RecvFrame(TMAudioFrame &frame, int timeout)              final override;

private:
    const static uint16_t AUDIO_CHANNEL = 2;
    const static uint16_t AUDIO_PERIOD_TIME = 40;
    const static uint16_t PERIOD_FRAMES_SIZE = AUDIO_PERIOD_TIME*16;
    const static uint16_t SAMPLE_RATE = 16000;
    const static uint16_t CVIAUDIO_AEC_LENGTH = 160;
    int mStart;
    pthread_mutex_t mMutex;
    aos_pcm_t *capture_handle;
    TMPropertyList mCurrentPropertyList;
    short *micDataIn;
    short *refDataIn;
    short *algoDataOut;
    void *audioAlgoHandle;
};


#endif
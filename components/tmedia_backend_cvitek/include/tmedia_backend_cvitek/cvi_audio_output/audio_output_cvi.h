/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_CVI_AUDIO_OUTPUT_H
#define TM_CVI_AUDIO_OUTPUT_H

#include <tmedia_core/entity/format/format_io.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include <tmedia_core/util/util_inc.h>

class  TMAudioOutputCvi: public TMAudioOutput 
{
public:

    TMAudioOutputCvi();
    virtual ~TMAudioOutputCvi();

    // TMSinkEntity interface
    TMSinkPad *GetSinkPad(int padID = 0) final override;

    // TMAudioOutput interface
    int Open(string deviceName, TMPropertyList *propList = NULL) final override;
    int Close()                                                  final override;
    int SetConfig(TMPropertyList &propList)                      final override;
    int GetConfig(TMPropertyList &propList)                      final override;
    int Start()                                                  final override;
    int Stop()                                                   final override;
    int SendFrame(TMAudioFrame &frame, int timeout)              final override;

private:
    const static uint16_t AUDIO_PERIOD_SIZE = 320;
    TMPropertyList mCurrentPropertyList;
    pthread_mutex_t mMutex;
    int mStart;
    aos_pcm_t *playback_handle;
    static void     *Task(void *args);
    TMUtilRingBuffer *mRingBuffer;
};


#endif

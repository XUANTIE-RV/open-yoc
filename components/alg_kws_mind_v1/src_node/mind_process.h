/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __MIND_PROCESS_H__
#define __MIND_PROCESS_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VOICE_VADSTAT_SILENCE = 0,
    VOICE_VADSTAT_VOICE,
    VOICE_VADSTAT_ASR,
    VOICE_VADSTAT_ASR_FINI,
    VOICE_VAD_PCM_DATA,
} voice_vad_state;

#ifdef __cplusplus
}
#endif

#endif
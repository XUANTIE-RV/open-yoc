/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _VOICE_THEAD_H_
#define _VOICE_THEAD_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VOICE_STATE_IDLE,
    VOICE_STATE_BUSY
} voice_state_t;


typedef struct voice {
    mic_t           *mic;
    mic_event_t      event_cb;
    voice_state_t   state;
    int             task_running;
    aos_task_t      plugin_task;
    aos_sem_t       pcm_sem;

    aos_task_t      kws_data_task;
    aos_sem_t       kws_sem;
} voice_t;

#ifdef __cplusplus
}
#endif

#endif
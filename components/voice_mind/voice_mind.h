/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _VOICE_MIND_H_
#define _VOICE_MIND_H_

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct voice {
        mic_t      *mic;
        mic_event_t event_cb;
        int         task_running;
        int         task_exit;
        int         task_start;
        aos_task_t  plugin_task;
        aos_sem_t   pcm_sem;
        aos_sem_t   start_sem;
        int         pcm_output_en;

        char *kws_data;
        int   kws_data_len;
    } voice_t;

#ifdef __cplusplus
}
#endif

#endif
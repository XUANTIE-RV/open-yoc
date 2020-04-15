/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __YV_AP__
#define __YV_AP__

#include <yoc/mic.h>
#include "yv_defs.h"

/* 目前在实现中与mic_event_id_t 一一对应 
若mic API发生变化，该ID需要做转换 */
typedef enum {
    YV_PCM_EVT,
    YV_ASR_EVT, /*事件参数 type 0:强制唤醒 其他值预留*/
    YV_SILENCE_EVT,
    YV_VAD_EVT,
    YV_VAD_DAT_EVT,
    YV_KWS_EVT,

    YV_EVT_ID_END
} yv_evt_id_t;

typedef struct mic_pcm_param yv_pcm_param_t;
typedef struct _yv_ap_ yv_t;

//init
yv_t *yv_init(mic_event_t cb, void *priv);
yv_t *yv_get_handler();
int yv_deinit(yv_t *yv);

//control
int yv_get_dsp_version(yv_t *yv, uint8_t **dsp_version);
int yv_power_enable(yv_t *yv, int flag);
int yv_kws_enable(yv_t *yv, int flag);
int yv_pcm_enable(yv_t *yv, int flag);
int yv_wake_trigger(yv_t *yv, int flag);
int yv_dbg_set_memrec_addr(yv_t *yv, int addr);

//pcm param
int yv_pcm_param_set(yv_t *yv, yv_pcm_param_t *param);
int yv_pcm_param_get(yv_t *yv, yv_pcm_param_t *param);

//audio equalizer
int yv_eq_init(yv_t *yv, yv_eq_settings_t *eq_settings);
int yv_eq_config(yv_t *yv, int channel_num, int sample_rate, int type);
int yv_eq_proc(yv_t *yv, void *data, int len);
int yv_init_kw_map(yv_t *yv, void **map);
int yv_config_share_memory(yv_t *yv, uint32_t memory_id, void *addr, size_t size);
void *yv_get_share_memory(uint32_t memory_id, size_t *size);

//debug
void yv_debug_hook(mic_event_t hook, int dbg_level);
#endif

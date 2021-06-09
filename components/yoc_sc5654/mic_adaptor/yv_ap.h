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

typedef struct yv_pcm_param {
    int   sample_bits;      /* 采样精度 默认16bit*/
    int   channels;         /* 采样通道数 默认1*/
    int   rate;             /* 采样率 默认16K*/
    int   nsmode;           /* 去噪等级参数  0~3 非线性处理等级逐步加强，其他值无非线性处理 */
    int   aecmode;          /* 回音消除等级  0~3 非线性处理等级逐步加强，其他值无非线性处理 */
    int   vadmode;          /* VAD等级 0~3 等级逐步加强 */
    int   sentence_time_ms; /* 有语音断句时间 */
    int   noack_time_ms;    /* 无语音超时时间 */
    int   max_time_ms;      /* 唤醒后总超时时间 */
    void *ext_param1;       /* 预留 */
    void *ext_param2;       /* 预留 */
    int   vadswitch;        /* 0 关闭VAD，1 打开, 2 打开起点关闭尾点 */
    int   vadfilter;        /* VAD过滤器类型， 0 关闭过滤器， 1~3 不同过滤器类型 */
} yv_pcm_param_t;

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

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __MIC_PORT_H__
#define __MIC_PORT_H__

#include <aos/aos.h>
#include <ulog/ulog.h>
#include <yoc/mic.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct __mic mic_t;

/**
 * MIC 事件回调处理
 * @param priv 用于传递私有数据
 * @param event_id 麦克风事件
 * @param data 事件参数数据指针
 * @param size 数据字节数
 */
typedef void (*mic_event_t)(mic_t *priv, mic_event_id_t evt_id, void *data, int size);

typedef struct mic_ops {
    int (*init)(mic_t *mic, mic_event_t mic_event); /* 模块初始化 */
    int (*deinit)(mic_t *mic);                      /* 去初始化 */
    int (*start)(mic_t *mic);                       /* 启动模块 */
    int (*stop)(mic_t *mic);                        /* 停止模块 */

    int (*pcm_data_control)(mic_t *mic, int enable); /* 麦克风控制命令 */
    int (*set_push2talk)(mic_t *mic, int mode);                        /* push2talk */
    int (*notify_play_status)(mic_t *mic, int play_status, int delay); /* 播放状态通知算法 */
    int (*set_wakeup_level)(mic_t *mic, char *wakeup_word, int level); /* 配置唤醒等级 */
    int (*start_doa)(mic_t *mic);                                      /* 启动声源定位 */
    int (*enable_linear_aec_data)(mic_t *mic, int enable);             /* 使能线性AEC数据的输出 */
    int (*enable_asr)(mic_t *mic, int enable);      /* 本地ASR的使能控制 */
} mic_ops_t;

int mic_ops_register(mic_ops_t *ops);

#ifdef __cplusplus
}
#endif

#endif

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __YOC_MIC_H__
#define __YOC_MIC_H__

#include <aos/aos.h>
#include <aos/list.h>
#include <uservice/uservice.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/*************************************************
 * 麦克风服务
 *************************************************/
typedef enum {
    MIC_CTRL_START_PCM,      /* 启动音频数据传输 */
    MIC_CTRL_STOP_PCM,       /* 停止音频数据传输 */
    MIC_CTRL_START_SESSION,  /* 强制进入对话模式(P2T) */
    MIC_CTRL_VOICE_MUTE,     /* 进入闭(开)麦模式 */
    MIC_CTRL_NOTIFY_PLAYER_STATUS /* 通知算法播放状态，用于唤醒提示音的处理(若算法支持) */
} mic_ctrl_cmd_t;

typedef enum {
    MIC_EVENT_INVALID,
    MIC_EVENT_PCM_DATA,      /* 有音频数据 */
    MIC_EVENT_SESSION_START, /* 开始对话 */
    MIC_EVENT_SESSION_STOP,  /* 停止对话 */
    MIC_EVENT_KWS_DATA,      /* 唤醒词数据 */
} mic_event_id_t;


typedef struct mic_kws{
    int id;
    int score;
    char word[32];
} mic_kws_t;

typedef struct mic_pcm_vad_data {
    int vad_tag;
    int len;
    char data[0];
} mic_pcm_vad_data_t;

#define KWS_ID_WWV_MASK     0x10000
#define KWS_ID_P2T_MASK     0x20000
#define KWS_ID_MASK         0x0FFFF

/**
 * 麦克风事件回调
 * @param source 预留，数据来源
 * @param evt_id 参见mic_event_id_t枚举的说明
 * @param data 事件参数
 *        若是MIC_EVENT_PCM_DATA事件，data为音频数据
 *        若是MIC_EVENT_SESSION_START事件，data指针指向一个整型，值为0表示主关键字唤醒，0~255 唤醒词索引，>=256 预留非语音唤醒
 * @param size 参数的字节数
 */
typedef void (*aui_mic_evt_t)(int source, mic_event_id_t evt_id, void *data, int size);

int aui_mic_init(utask_t *task, aui_mic_evt_t evt_cb);
int aui_mic_deinit(void);

/**
 * 启动麦克风服务
 * @return 0:成功
 */
int aui_mic_start(void);

/**
 * 停止麦克风服务
 * @return 0:成功
 */
int aui_mic_stop(void);

/**
 * 麦克风控制命令
 * @param cmd 控制命令
 * @return 0:成功
 */
int aui_mic_control(mic_ctrl_cmd_t cmd, ...);

/**
 * 发送唤醒事件，用于二次唤醒确认后进入唤醒流程
 */
void aui_mic_send_wakeup_event(void);

/**
 * 麦克风设备注册
 */
void aui_mic_register(void);

#ifdef __cplusplus
}
#endif

#endif

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __YOC_MIC_H__
#define __YOC_MIC_H__

#include <aos/list.h>
#include <yoc/uservice.h>

#ifdef __cplusplus
extern "C" {
#endif


/*************************************************
 * 麦克风服务
 *************************************************/
typedef enum {
    MIC_CTRL_START_PCM,      /* 启动音频数据传输 */
    MIC_CTRL_STOP_PCM,       /* 停止音频数据传输 */
    MIC_CTRL_START_VAD,      /* 预留 */
    MIC_CTRL_STOP_VAD,       /* 预留 */
    MIC_CTRL_VAD_TIMEOUT,    /* 预留 */
    MIC_CTRL_START_SESSION,  /* 强制进入对话模式 */
    MIC_CTRL_ENABLE_WWV,     /* 使能唤醒二次确认功能*/
    MIC_CTRL_DISABLE_WWV,    /* 禁止唤醒二次确认功能*/
    MIC_CTRL_DEBUG_LEVEL_1,  /* 设置调试模式1*/
    MIC_CTRL_DEBUG_LEVEL_2,  /* 设置调试模式2*/
    MIC_CTRL_DEBUG_LEVEL_3,  /* 设置调试模式3*/
    MIC_CTRL_DEBUG_CLOSE,    /* 退出调试模式*/
    MIC_END
} mic_ctrl_cmd_t;

typedef enum {
    MIC_STATE_IDLE,    /* 空闲 */
    MIC_STATE_SESSION, /* 对话状态 */
    MIC_STATE_PCM,     /* 正在传输音频数据 */

    MIC_STATE_END
} mic_state_t;

typedef enum {
    MIC_EVENT_PCM_DATA,      /* 有音频数据 */
    MIC_EVENT_SESSION_START, /* 开始对话 */
    MIC_EVENT_SESSION_STOP,  /* 停止对话 */
    MIC_EVENT_VAD,
    MIC_EVENT_VAD_DATA,      /* 唤醒后命令词数据 */
    MIC_EVENT_KWS_DATA,      /* 唤醒词数据 */
} mic_event_id_t;

typedef enum {
    MIC_REC_START,  /* 开始录制*/
    MIC_REC_STOP    /* 停止录制*/
} mic_rec_cmd_t;

typedef struct mic_pcm_param {
    int   sample_bits;      /* 采样精度 默认16bit*/
    int   channels;         /* 预留  */
    int   rate;             /* 采样率 默认16K*/
    int   nsmode;           /* 去噪等级参数  0~3 非线性处理等级逐步加强，其他值无非线性处理 */
    int   acemode;          /* 回音消除等级  0~3 非线性处理等级逐步加强，其他值无非线性处理 */
    int   vadmode;          /* VAD等级 -1 关闭VAD,  0~3 等级逐步加强 */
    int   sentence_time_ms; /* 有语音断句时间 */
    int   noack_time_ms;    /* 无语音超时时间 */
    int   max_time_ms;      /* 唤醒后总超时时间 */
    void *ext_param1;       /* 预留 */
    void *ext_param2;       /* 预留 */
} mic_pcm_param_t;

#define KWS_ID_WWV_MASK     0x10000

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
/**
 * 启动麦克风服务
 * @param task 传入服务绑定的任务
 * @return 0:成功
 */
int aui_mic_start(utask_t *task, aui_mic_evt_t evt_cb);

/**
 * 停止麦克风服务
 * @return 0:成功
 */
int aui_mic_stop(void);

/**
 * 设置麦克风服务参数
 * @pram param 传入参数
 * @return 0:成功
 */
int aui_mic_set_param(mic_pcm_param_t *param);

/**
 * 设置麦克风唤醒使能
 * @pram en 0:开启 1:关闭
 * @return 0:成功
 */
int aui_mic_set_wake_enable(int en);

/**
 * 设置麦克风音频来源
 * @param source 音频来源
 * @return 0:成功
 */
int aui_mic_set_active(int source);

/**
 * 麦克风控制命令
 * @param cmd 控制命令
 * @return 0:成功
 */
int aui_mic_control(mic_ctrl_cmd_t cmd);

/**
 * 获取唤醒二次确认数据
 * @param len 数据长度
 * @return 数据指针， NULL: 失败
 */
void *aui_mic_get_wwv_data(size_t *len);

/**
 * 获取麦克风状态
 * @param state 输出状态值
 * @return 0:成功
 */
int aui_mic_get_state(mic_state_t *state);

/**
 * 启动mic网络
 * @param url  网络服务器地址
 * @param save_name 保存到服务端的文件名字
 * @return 0:成功
 */
int aui_mic_rec_start(const char *url, const char *save_name);

/**
 * 结束mic网络录制
 * @return 0:成功
 */
int aui_mic_rec_stop(void);

/*************************************************
 * 麦克风适配接口
 *************************************************/
typedef struct __mic mic_t;

typedef struct mic_hw_param {
    int sample_bits;  /* 音频采样位 16bit*/
    int channels;     /* 通道个数 1/2 */
    int rate;         /* 音频采样率 16000*/
    int encode;       /* 编码格式 0:PCM */
    int source;       /* 预留，用于控制使能哪些麦克风 */
} mic_hw_param_t;

typedef struct mic_sw_param {
    int buffer_size;      /* 麦克风驱动每次返回的最大数据量 */
    int buffer_num;       /* 预留，用于控制麦克风驱动使用的缓存个数 */
    int start_threshold;  /* 预留 */
    int sentence_time_ms; /* 断句时间 */
} mic_sw_param_t;

/**
 * MIC 事件回调处理
 * @param priv 用于传递私有数据
 * @param event_id 麦克风事件
 * @param data 事件参数数据指针
 * @param size 数据字节数
 */
typedef void (*mic_event_t)(void *priv, mic_event_id_t evt_id, void *data, int size);

typedef struct mic_ops {
    int (*init)(mic_t *mic, mic_event_t mic_event);
    int (*deinit)(mic_t *mic);
    int (*kws_control)(mic_t *mic, int flag);
    int (*kws_wake)(mic_t *mic, int flag);
    int (*wwv_enable)(mic_t *mic, int flag);
    int (*wwv_get_data)(mic_t *mic, void **param, size_t *size);
    int (*pcm_data_control)(mic_t *mic, int flag);
    int (*pcm_aec_control)(mic_t *mic, int flag);
    int (*debug_control)(mic_t *mic, int flag);
    int (*vad_control)(mic_t *mic, int flag);
    int (*vad_timeout)(mic_t *mic, int timeout_ms);
    int (*pcm_set_param)(mic_t *mic, void *param);
    int (*pcm_get_param)(mic_t *mic, void *param);
    void (*mic_rec_start)(mic_t *mic, const char *url, const char *save_name);
    void (*mic_rec_stop)(mic_t *mic);
} mic_ops_t;

int mic_set_privdata(void *priv);
void *mic_get_privdata(void);
int mic_ops_register(mic_ops_t *ops);

/**
 * 麦克风设备注册
 */
void yunvoice_mic_register(void);
void pangu_mic_register(void);
void voice_mic_register(void);
void mic_thead_v1_register(void);

#ifdef __cplusplus
}
#endif

#endif

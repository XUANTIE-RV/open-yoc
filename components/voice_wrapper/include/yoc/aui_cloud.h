/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _AUI_CLOUD_H_
#define _AUI_CLOUD_H_

#include <aos/aos.h>
#include <aos/list.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum aui_tts_state {
    AUI_TTS_INIT       = 1,
    AUI_TTS_PLAYING    = 2,
    AUI_TTS_FINISH     = 3,
    AUI_TTS_ERROR      = 4,
} aui_tts_state_e;

typedef void (*aui_nlp_cb_t)(void *data, int len, void* nlp_priv);
typedef void (*aui_tts_cb_t)(void *data, int len, void* tts_priv);

typedef struct _aui aui_t;

#define aui_cmm_cls  \
    void *priv; \
    int (*init)(aui_t *aui); \
    int (*deinit)(aui_t *aui); \
    int (*start)(aui_t *aui); \
    int (*stop)(aui_t *aui);

typedef struct aui_nlp_cls {
    aui_cmm_cls;
    int (*push_data)(aui_t *aui, void *data, size_t size);
    int (*stop_push_data)(aui_t *aui);
    int (*push_text)(aui_t *aui, char *text);
} aui_nlp_cls_t;

typedef struct aui_tts_cls {
    aui_cmm_cls;
    int (*req_tts)(aui_t *aui, const char *text);
} aui_tts_cls_t;

typedef struct aui_ops {
    aui_nlp_cls_t *nlp;
    aui_tts_cls_t *tts;
} aui_ops_t;

typedef struct aui_cb  {
    aui_nlp_cb_t nlp_cb;
    aui_tts_cb_t tts_cb;
    void *nlp_priv;
    void *tts_priv;
} aui_cb_t;

typedef enum _aui_audio_req_type {
    AUI_AUDIO_REQ_TYPE_NLP = 0,  /* 请求语义识别 */
    AUI_AUDIO_REQ_TYPE_WWV       /* 请求二次唤醒确认 */
} aui_audio_req_type_e;

struct _aui {
    aui_ops_t ops;
    aui_cb_t cb;
    aui_audio_req_type_e audio_req_type; /*请求类型*/
};

typedef struct aui_kws {
    aui_audio_req_type_e data_type;
    char  *data;
    size_t len;
} aui_audio_req_ack_info_t;

/*************************************************
 * AUI 应用接口
 *************************************************/
/**
 * AUI系统初始化
 * 
 * @param aui aui对象指针
 * @param config aui配置
 * @return 0:成功
 */
aui_t *aui_cloud_init(void);

/**
 * AUI系统去初始化
 * 
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_deinit(aui_t *aui);

/**
 * AUI事件回调注册接口
 * 
 * @param aui  aui对象指针
 * @param cb   tts、nlp的事件回调
 * @param priv tts、nlp的用户参数
 * @return void
 */
void aui_nlp_register(aui_t *aui, aui_nlp_cb_t cb, void *priv);
void aui_tts_register(aui_t *aui, aui_tts_cb_t cb, void *priv);

/**
 * 停止上云流程
 * 
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_stop(aui_t *aui);

/**
 * 启动语音数据交互
 *
 * @param aui aui对象指针
 * @param type 请求类型
 * @return 0:成功
 */
int aui_cloud_start_audio(aui_t *aui, aui_audio_req_type_e type);

/**
 * 向AUI系统输入语音数据
 *
 * @param aui aui对象指针
 * @param data 数据指针
 * @param size 数据大小
 * @return 0:成功
 */
int aui_cloud_push_audio(aui_t *aui, void *data, size_t size);

/**
 * 结束语音数据输入，AUI系统开始处理
 * 
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_stop_audio(aui_t *aui);

/**
 * 向AUI系统输入文本数据，AUI进行处理并回调NLP处理回调函数
 * 
 * @param aui aui对象指针
 * @param text 文本字符串
 * @return 0:成功
 */
int aui_cloud_push_text(aui_t *aui, char *text);

/**
 * 启动TTS请求，连接云端
 *
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_start_tts(aui_t *aui);

/**
 * 向AUI系统输入文本数据，要求返回文本的TTS转换后的语音结果
 *
 * @param aui aui对象指针
 * @param text 需要转换的文本
 * @return 0:成功
 */
int aui_cloud_req_tts(aui_t *aui, const char *text);

/**
 * 停止tts系统，断开云端连接
 *
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_stop_tts(aui_t *aui);

/*************************************************
 * NLP执行错误定义
 *************************************************/
#define AUI_CMD_PROC_NOMATCH -1        /* 无法匹配到可处理的技能 */
#define AUI_CMD_PROC_MATCH_NOACTION -2 /* 匹配到技能执行器，但意图不支持或解析有错误 */
#define AUI_CMD_PROC_NET_ABNORMAL -3   /* 网络异常 */
#define AUI_CMD_PROC_ERROR -10         /* 其他错误 */

/*************************************************
 * AUI 云端适配接口
 *************************************************/
/**
 * AUI系统注册适配接口
 * 
 * @param aui  aui对象指针
 * @param ops  nlp、tts的实现
 * @param cb   nlp、tts的事件回调
 * @param priv nlp、tts的用户参数
 * @return 0:成功
 */
int aui_cloud_nlp_register(aui_t *aui, aui_nlp_cls_t *ops, aui_nlp_cb_t cb, void *priv);
int aui_cloud_tts_register(aui_t *aui, aui_tts_cls_t *ops, aui_tts_cb_t cb, void *priv);
int aui_cloud_nlp_unregister(aui_t *aui);
int aui_cloud_tts_unregister(aui_t *aui);

#ifdef __cplusplus
}
#endif
#endif

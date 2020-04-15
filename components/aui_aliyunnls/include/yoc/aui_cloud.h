/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _AUI_CLOUD_H_
#define _AUI_CLOUD_H_

#include <aos/kernel.h>
#include <aos/list.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************
 * AUI CLOUD系统API
 * 语音解析(云端)、NLP Json解析及处理
 *************************************************/

typedef enum aui_asr_type {
    CLOUD_DEFAULT = 0,
    CLOUD_RASR_XFYUN = 1,
    CLOUD_ASR_XFYUN = 2,
    CLOUD_ASR_BAIDU = 3,
    CLOUD_ASR_PROXY_WS = 4
} aui_asr_type_e;

typedef struct aui_config {
    char *per;                      // 发音人选择
    int   vol;                      // 音量，取值0-100，50标准音量
    int   pit;                      // 音调n
    int   spd;                      // 语速，-500~500，默认0标准语速
    int   fmt;                      // 编码格式，1：PCM 2：MP3
    int   srate;                    // 采样率，16000
    int   cloud_vad;                // 云端VAD功能使能， 0：关闭；1：打开
    char  *tts_cache_path;	        // TTS内部缓存路径，NULL：关闭缓存功能
    void (*nlp_cb)(const char *json_text);  // NLP处理回调
} aui_config_t;

typedef enum aui_tts_state {
    AUI_TTS_INIT    = 1,
    AUI_TTS_CONTINUE = 2,
    AUI_TTS_FINISH  = 3,
    AUI_TTS_ERROR   = 4,
} aui_tts_state_e;

typedef enum aui_wwv_resut {
    AUI_WWV_REJECT = 1,
    AUI_WWV_CONFIRM
} aui_wwv_resut_e;

typedef struct _aui {
    aui_config_t    config;
    void            *context;
    aos_sem_t       sem;
    aui_asr_type_e  asr_type;
} aui_t;

typedef void (* aui_wwv_cb_t)(aui_wwv_resut_e wwv_result);
typedef void (* aui_tts_cb_t)(aui_tts_state_e tts_state);

/**
 * 设置AUI系统账号信息，在aui_cloud_init之前调用
 * 
 * @param json_account_info json字符串格式的aui系统账号信息
 * @return 0:成功
 */
int aui_cloud_set_account(const char *json_account_info);

/**
 * AUI系统初始化
 * 
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_init(aui_t *aui);

/**
 * 设置云端会话ID
 *
 * @param session_id 会话ID字符串，相同ID认为是同一轮会话
 * @return 0:成功
 */
int aui_cloud_set_session_id(const char *session_id);

/**
 * 启动语音数据交互，在aui_cloud_start_pcm前调用
 *
 * @param aui aui对象指针
 * @param enable 本次唤醒是否做云端二次确认
 * @return 0:成功
 */
int aui_cloud_enable_wwv(aui_t *aui, int enable);

/**
 * 启动语音数据交互
 *
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_start_pcm(aui_t *aui);

/**
 * 向AUI系统输入语音数据
 *
 * @param aui aui对象指针
 * @param data 数据指针
 * @param size 数据大小
 * @return 0:成功
 */
int aui_cloud_push_pcm(aui_t *aui, void *data, size_t size);

/**
 * 结束语音数据输入，AUI系统开始处理
 * 
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_stop_pcm(aui_t *aui);

/**
 * 强制停止上云流程
 * 
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_force_stop(aui_t *aui);

/**
 * 使能云端唤醒二次确认流程
 * 
 * @param aui aui对象指针
 * @param aui_wwv_cb 二次唤醒结果回调
 * @return 0:成功
 */
int aui_cloud_init_wwv(aui_t *aui, aui_wwv_cb_t cb);

/**
 * 推送唤醒二次确认音频数据到云端
 * 
 * @param aui aui对象指针
 * @param aui_wwv_cb 二次唤醒结果回调
 * @return 0:成功
 */
int aui_cloud_push_wwv_data(aui_t *aui, void *data, size_t size);

/**
 * 向AUI系统输入文本数据，AUI进行处理并回调NLP处理回调函数
 * 
 * @param aui aui对象指针
 * @param test 文本字符串
 * @return 0:成功
 */
int aui_cloud_push_text(aui_t *aui, char *text);

/**
 * 启动tts系统，连接云端
 *
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_start_tts(aui_t *aui);

/**
 * 向AUI系统输入文本数据，要求返回文本的TTS转换后的语音结果
 *
 * @param aui aui对象指针
 * @param player_fifo_name 例如"fifo://tts1",播放接口可使用该字符串来播放音频
 * @param text 需要转换的文本
 * @return 0:成功
 */
int aui_cloud_req_tts(aui_t *aui, const char *player_fifo_name, const char *text, aui_tts_cb_t stat_cb);

/**
 * 停止tts系统，断开云端连接
 *
 * @param aui aui对象指针
 * @return 0:成功
 */
int aui_cloud_stop_tts(aui_t *aui);

/**
 * 向AI引擎输入文本数据，要求返回文本的TTS转换后的语音结果
 * 
 * @param max_text_len 匹配字符串的长度
 * @param delim 匹配文本的分隔符
 */
void aui_textcmd_matchinit(int max_text_len, const char* delim);

/**
 *  控制命令设备端预解析
 *  AUI系统的本地加速处理，主要对短文本进行字符匹配，处理完成后会调用NLP处理回调
 *  @param cmd 如果匹配，返回NLP的控制命令字符串
 *  @param text 需要匹配的文本
 * 
 */
void aui_textcmd_matchadd(const char *cmd, const char *text);

/**
 *  输入字符串，返回对应的控制字符串
 *  @param text 需要匹配的文本
 * 
 *  返回通过aui_textcmd_matchadd的cmd字符串
 */
const char *aui_textcmd_find(const char *text);

/**
 *  预处理命令与AUI NLP的绑定
 *  把语音通过ASR转化为文本后，再调用该函数，把预处理的控制命令统一通过AUI的NLP处理回调进行处理
 *  @param aui aui对象指针
 *  @param text 命令文本
 *  @return 0:匹配到关键字 <0:没有匹配
 *
 */
int aui_textcmd_matchnlp(aui_t *aui, const char *text);

/*************************************************
 * 命令执行器
 *
 *    AUI云端处理后返回NLP结果，调用命令执行器进行具体的操作
 *    如：音乐播放、或继续调用AUI进行其他功能解析
 *
 *************************************************/

/**
 * NLP处理回调
 *
 * @param js 传入的Json对象
 * @param json_text 传入的Json文本
 * @return 0:解析到请求并做了处理,处理循环退出,不继续检查其他处理
 *        -1:没有解析到请求,处理循环继续匹配其他回调
 *        -2:解析到请求但请求的格式或内容有误,处理循环退出,不继续检查其他处理
 */
typedef int (*aui_nlp_proc_cb_t)(cJSON *js, const char *json_text);

typedef struct aui_cmd {
    aui_nlp_proc_cb_t cmd;  // 命令执行处理
    slist_t next;
} aui_cmd_t;

typedef struct aui_cmd_set {
    slist_t cmds;
    aui_nlp_proc_cb_t pre_check_cmd;
} aui_cmd_set_t;

/**
 * 添加NLP处理回调
 *
 * @param set 命令执行器的集合对象
 * @param cmd NLP处理回调函数
 */
void aui_nlp_process_add(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);

/**
 * 删除NLP处理回调
 *
 * @param set 命令执行器的集合对象
 * @param cmd NLP处理回调函数
 */
void aui_nlp_process_remove(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);

/**
 * 设置命令执行器的预处理回调
 * 预处理回调函数不是必须的，一般对NLP返回的格式进行与检查是否合法
 * 预处理回调函数 cmd 的实现中，如果返回0，说明格式正确，会进入后续
 * 具体的处理回调中
 * 
 * @param set 命令执行器的集合对象
 * @param cmd 预处理回调函数
 */
void aui_nlp_process_set_pre_check(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);


#define AUI_CMD_PROC_NOMATCH -1        /* 无法匹配到可处理的技能 */
#define AUI_CMD_PROC_MATCH_NOACTION -2 /* 匹配到技能执行器，但意图不支持或解析有错误 */
#define AUI_CMD_PROC_NET_ABNORMAL -3   /* 网络异常 */
#define AUI_CMD_PROC_ERROR -10         /* 其他错误 */

/**
 * 命令执行器的执行
 * 按照添加顺序逆序执行aui_cmd_set_t 中每个注册的命令处理回调函数
 *
 * @param set 命令执行器的集合对象
 * @param json_text 需要进行分析处理的NLP处理文本
 * @return 0:执行成功
 *         AUI_CMD_PROC_NOMATCH
 *         AUI_CMD_PROC_MATCH_NOACTION
 *         AUI_CMD_PROC_ERROR
 */
int aui_nlp_process_run(aui_cmd_set_t *set, const char *json_text);

#ifdef __cplusplus
}
#endif
#endif

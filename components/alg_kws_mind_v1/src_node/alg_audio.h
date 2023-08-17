/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _ALG_AUDIO_H_
#define _ALG_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_CHIP_D1)
#undef CONFIG_FEOUTAEC
#elif defined(CONFIG_CHIP_BL606P)
//#define CONFIG_FEOUTAEC 1   //默认关闭
#endif

#define AUDIO_WAKEUP_TYPE_NONE 0
#define AUDIO_WAKEUP_TYPE_KEY  1
#define AUDIO_WAKEUP_TYPE_CMD  2
#define AUDIO_WAKEUP_TYPE_P2T  3
typedef struct _s_wakeup_info {
    int wakeup_type;  /* 0:没有唤醒， 1:唤醒词 2:命令词 3:P2T*/
    char *kws_word;   /* 唤醒词拼音 */
    int   kws_score;  /* 唤醒分数 0~100 */
    int   kws_id;     /* 唤醒词的ID， 如果支持 */
    int   vad_st;     /* 0:无 1:起点 2:尾点 3:静音超时 4:语音超时*/
} audio_wakeup_info_t;

/**
 * 初始化 
 */
void audio_alginit(void);

/**
 * @brief 语音识别处理
 * @param data_int 输入3路数据，2MIC+1REF
 * @param sample_count 单路的采样点数
 * @return  唤醒信息
 */
audio_wakeup_info_t audio_process(short *data_in, int sample_count, short **out, size_t *out_len);

/**
 * @brief 通知算法播放状态
 * @param stat 0 - 停止播放，1 - 正在播放
 * @param buf_delay 当前ringbuffer中参数音频时间，单位ms
 * @param hw_delay 播放器播放延时，单位ms
 */
void audio_wakeup_voice_stat(int stat, int buf_delay, int hw_delay);

/**
 * @brief 配置唤醒词的唤醒等级
 * @param wakeup_word 唤醒词
 * @param level 唤醒等级 0~100
 */
void audio_set_wakeup_level(const char *wakeup_word, int level);

/* 获取算法状态 0:就绪(会话停止) 1:唤醒 2:等待会话结束 3:结束 */
#define AU_STATUS_READY 0
#define AU_STATUS_WAKE 1
#define AU_STATUS_ASR  2
#define AU_STATUS_FINI 3


/* 获取状态 */
int audio_get_status();

/* 设置状态 */
int audio_set_status(int status);

/**
 * @brief 获取vad状态
 * 
 * @return -1：安静 0：语音起点 1：语音结束 2：超时
 */
int audio_get_vad();

/**
 * @brief 获取带VAD标签的识别数据
 * @param vad_data 识别数据指针
 * @param data_len 识别数据长度
 * @param finished VAD结束标志
 * @return 0: 成功 -1: 失败 
 */
int audio_get_vad_data(char **vad_data, int *data_len, int *finished);

/**
 * @brief 获取带VAD标签的识别数据
 * @param vad_data 识别数据指针
 * @param data_len 识别数据长度
 * @param frame_tag vad标签数组
 * @param finished VAD结束标志
 * @return 0: 成功 -1: 失败 
 */
int audio_get_tagged_vad_data(char **vad_data, int *data_len, short **frame_tag, int *finished);

/**
 * @brief 获取kws数据
 * @param data_len kws数据长度
 * @return !=NULL: 数据指针 NULL: 获取失败 
 */
char *audio_get_kws_data(int *data_len);

/**
 * @brief 设置算法调试日志等级
 * @param mode 调试模式，0: 关闭
 */
int audio_set_debug_level(unsigned int mode);

/**
 * @brief 启动p2t模式
 * @param mode 0: 默认模式
 *             1：直接启动ASR数据，可语音唤醒
 *             2：直接启动ASR数据，无法语音唤醒
 *             3：预留
 *             4：VAD启动ASR数据，可语音唤醒
 *             5：VAD启动ASR数据，无法语音唤醒
 */
int audio_set_p2t_mode(int mode);

/**
 * @brief 获取声源定位信息
 * @return 0~360, 其他值无效
 */
int audio_get_doa();

/**
 * @brief 获取ASR结果
 * @return 结果字符串制作
 *         NULL: 无ASR结果
 */
const char *audio_get_asr_result();

/**
 * @brief 清除ASR结果
 */
void audio_clear_asr_result();

/**
 * @brief 设置线性AEC的使能状态
 * @param status 要设置的状态(0|1)
 * @return 无
 */
void audio_set_linear_aec_en_status(int status);

/**
 * @brief 获取线性AEC的使能状态
 * @return 1：使能，0：未使能
 */
int audio_get_linear_aec_en_status(void);

#ifdef __cplusplus
}
#endif

#endif

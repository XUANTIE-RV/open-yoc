/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _BAORD_AUDIO_H_
#define _BAORD_AUDIO_H_

#include <soc.h>

/* 音频参数配置 */
#define AUIDO_IN_GAIN_MIC 27 /*db 该定义预留，未使用*/
#define AUIDO_IN_GAIN_REF 6  /*db 该定义预留，未使用*/

#define AUIDO_OUT_GAIN   -2  /*db 整数值*/
/*
    //硬件支持的dB列表,自动选择与配置最接近的值
    static float db_array[33] = {
        -70.0f, -24.6f, -18.3f, -14.7f, -12.2f, -10.2f, -8.6f, -7.3f, -6.1f, -5.1f, -4.1f,
        -3.3f,  -2.5f,  -1.8f,  -1.2f,  -0.6f,  0.0f,   0.5f,  1.0f,  1.5f,  1.9f,  2.3f,
        2.7f,   3.1f,   3.5f,   3.9f,   4.2f,   4.5f,   4.8f,  5.1f,  5.4f,  5.7f,  6.0f,
    };
*/

#define BOARD_AUDIO_SUPPORT_AMP 1

/**
 * 采集增益配置
 *
 * @param[in]  id  通道id,0为起始值
 * @return  0:执行成功，其他值为失败
 */
int board_audio_in_set_gain(int id, int gain);

/**
 * 获取采集增益配置
 *
 * @param[in]  id  通道id,0为起始值
 * @return  获取当前配置值
 */
int board_audio_in_get_gain(int id);

/**
 * 播放增益配置
 *
 * @param[in]  id  通道,0为起始值
 *                 播放一般只有一路，可忽略该参数，直接传递0
 * @return  0:执行成功，其他值为失败
 */

int board_audio_out_set_gain(int id, int gain);

/**
 * 获取播放增益配置
 *
 * @param[in]  id  通道id,0为起始值
 * @return  获取当前配置值
 */
int board_audio_out_get_gain(int id);

#ifdef CONFIG_SMART_AUDIO
#include <smart_audio.h>
/**
 * 获取音量曲线配置
 *
 * @return 返回音量曲线配置结构体指针
 */
audio_vol_config_t *board_audio_out_get_vol_config(void);

/*EQ参数配置*/
#define EQ_TYPE_NONE     0
#define EQ_TYPE_HARDWARE 1
#define EQ_TYPE_SOFTWARE 2
uint8_t *board_eq_get_param(size_t *byte);
int    board_eq_set_param(void *data, int byte);
int    board_eq_show_param(void);
int    board_eq_get_type(void);
int    board_drc_set_param(void *data, int byte);
#endif /*CONFIG_SMART_AUDIO*/

/**
 * 获取PA控制引脚号
 */
int board_audio_get_pa_mute_pin(void);

/**
 * 直接控制功放使能
 */
void board_audio_amplifier_onoff(int on);

#endif


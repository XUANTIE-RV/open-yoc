/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __SILAN_VOICE_ADC_H__
#define __SILAN_VOICE_ADC_H__

#include <stdint.h>
#include <stddef.h>

#define SRAM_TEXT __attribute__((section(".sram.text")))
#define SRAM_TEXT2 __attribute__((section(".sram.text2")))
#define SRAM_DATA __attribute__((section(".sram.data")))
#define SRAM_BSS __attribute__((section(".sram.bss")))

typedef struct {
    uint8_t *buffer;
    uint32_t length;
    uint32_t head;
    uint32_t tail;
} codec_adrb_t;

typedef struct {
    codec_adrb_t *rb_mic;
    codec_adrb_t *rb_ref;
    int wake_flag;
} voice_adc_data_t;

typedef int (*vad_alg_t)(void);

void sram_init(void);
void sram_mic_stop(void);
void voice_register_vadalg(vad_alg_t alg);
int voice_get_vadalg_ret(void);
void voice_mic_init(int boost_gain, int l_gain, int r_gain);
void voice_ref_init(int l_gain, int r_gain);
void voice_ref_stop(void);
void *voice_get_ref_rb(void);
void *voice_get_adc_data(void);

/**
 * 通知DSP,主控的状态
 *
 * @param wake_flag
 *        -1:通知DSP主控准备进入睡眠
 *         1:通知DSP已经唤醒
*/
void voice_set_wake_flag(int wake_flag);

/* 以下是SRAM函数，给vad1算法使用 */
void *voice_get_mic_rb_sram(void);
int codec_adrb_available_read_space(codec_adrb_t *codec_adrb);
int codec_adrb_read(codec_adrb_t *codec_adrb, uint8_t *target, uint32_t amount);

/* 
 * 宏开启进入低功耗CPU不进行变频和关闭SDRAM，使调试打印能够正常输出
 * 有以下三种配置
 * PRINT_DEBUG_EN=1 SDRAM_ACCESS_EN=1
 * PRINT_DEBUG_EN=0 SDRAM_ACCESS_EN=0/1
*/
#define PRINT_DEBUG_EN 0
#define SDRAM_ACCESS_EN 0

/* 
 * 获取当前timer的计数值
 * SRAM_TIMER_EN 定义为1，功能有效
 * 修改配置需要重新编译SDK生效
*/
#define SRAM_TIMER_EN 0
uint32_t sram_timer_get_us(void);

#define SRAM_MIC_ADEV_LLI_NUM 100

#endif

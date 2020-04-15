/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <pin_name.h>

/* 开发板定义 */
#ifndef BOARD_CB5654
#define BOARD_CB5654 
#endif

/* 系统 */
//1:1.2V  2:1.0V  3:1.4V
// #define SOC_DSP_LDO_LEVEL 3

#define CONSOLE_ID 1

/* LED */
#define PIN_LED_R  LED_PIN_NOT_SET
#define PIN_LED_G  PD4
#define PIN_LED_B  LED_PIN_NOT_SET
#define LED_FLIP_FLAG  1  /* 低电平亮 */

/* RTC */
#define CONFIG_RTC_EN   1

/* 音频 */
#define PIN_PA_EN  PD0
#define CONFIG_VOL_MAX    (88)
#define CONFIG_LEFT_GAIN  (-1) /* 左声道固定 -> PA */
#define CONFIG_RIGHT_GAIN (88) /* 右声道可调 -> REF */
#define CONFIG_MIC_GAIN   (18) /* MIC初始20dB, 该值在20dB基础上增加的dB数 */

/* WiFi */
#define CONFIG_WIFI_RTL8723DS 1
#define WLAN_ENABLE_PIN   PC8
//#define PIN_BT_EN     PB8
#define WLAN_POWER_PIN    PC0  /* 等于0xffffffff表示不支持 */

#define PIN_WL_WAKE_HOST PA6
#define PIN_WL_WAKE_HOST_GROUP LPM_DEV_MASK_GENERAL_GPIO2

/* LPM */
#define MAX_LPM_WAKESRC_COUNT 5

/* ADC按键引脚 */
#define PIN_ADC_KEY    PA1

/* ADC按键配置 */
#define KEY_ADC_VAL1 1751
#define KEY_ADC_VAL2 2311
#define KEY_ADC_VAL3 3051
#define KEY_ADC_VAL4 1376
#define KEY_ADC_VAL5 3587
#define KEY_AD_VAL_OFFSET 100    /* 按键值误差允许 */

#define VAD_ADC_VAL_MAX   KEY_ADC_VAL5 + KEY_AD_VAL_OFFSET /* 最大值 + 误差值 */
#define VAD_ADC_VAL_MIN   KEY_ADC_VAL4 - KEY_AD_VAL_OFFSET /* 最小值 - 误差值 */

/* 模拟电源按键 */
#define SIMULATED_POWER_KEY 1
#define POWER_KEY_ADC   /* 配置则读取模拟电源键adc值，不配置则读取模拟电源键gpio值 */
#define POWER_KEY_PIN       PA1
#define POWER_KEY_VAL_MAX   1600
#define POWER_KEY_VAL_MIN   1300

/* 可用GPIO列表 */
#define USER_GPIO_LIST_STR \
"ID  Name\n \
42  PD4(LED)\n \
46  PD0(PA MUTE)\n \
45  PD1\n \
38  PD7\n \
39  PD8\n \
29  PC12\n \
30  PC13\n \
 2  PA2\n \
59  PB7\n \
60  PB6\n \
61  PB5\n \
62  PB4\n \
"

#endif /* __BOARD_CONFIG_H__ */

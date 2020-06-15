/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <pin_name.h>

/* 开发板定义 */
#ifndef BOARD_MIT_V2
#define BOARD_MIT_V2
#endif

/* 系统 */
//1:1.2V  2:1.0V  3:1.4V
// #define SOC_DSP_LDO_LEVEL 3

#define CONSOLE_ID 1

/* LED */
#define PIN_LED_R  -1
#define PIN_LED_G  -1
#define PIN_LED_B  -1
#define LED_FLIP_FLAG  0  /* 高电平亮 */

#define AW9523_ENABLED
#define AW9523_RST_PIN  PD1

#define AW2013_ENABLED

/* RTC */
#define CONFIG_RTC_EN   1

/* 音频 */
#define PIN_PA_EN  PD7
#define CONFIG_VOL_MAX    (90)
#define CONFIG_LEFT_GAIN  (-1) /* 88 左声道固定 -> REF */
#define CONFIG_RIGHT_GAIN (-1) /* 右声道可调 -> PA */
#define CONFIG_MIC_GAIN   (18) /* MIC初始20dB, 该值在20dB基础上增加的dB数 */

/* WiFi */
#define CONFIG_WIFI_RTL8723DS 1
#define WLAN_ENABLE_PIN       PC8
//#define PIN_BT_EN     PC7
#define WLAN_POWER_PIN    PC0 //0xffffffff  /* 不支持 */

#define PIN_WL_WAKE_HOST PA6
#define PIN_WL_WAKE_HOST_GROUP LPM_DEV_MASK_GENERAL_GPIO2

/* LPM */
#define MAX_LPM_WAKESRC_COUNT 5

/* 按键 */
#define PIN_ADC_KEY     PA1
#define PIN_ADC_KEY2    PA3
#define KEY_ADC_VAL1    887
#define KEY_ADC_VAL2    563
#define KEY_ADC_VAL3    1595
#define KEY_ADC_VAL4    1960
#define KEY_ADC_VAL5    1595
#define KEY_ADC_VAL6    563
#define KEY_ADC_VAL7    887
#define KEY_ADC_VAL8    1960
#define KEY_AD_VAL_OFFSET 100

#define VAD_ADC_VAL_MAX   KEY_ADC_VAL1 + KEY_AD_VAL_OFFSET /* 最大值 + 误差值 */
#define VAD_ADC_VAL_MIN   KEY_ADC_VAL6 - KEY_AD_VAL_OFFSET /* 最小值 - 误差值 */

#define CONFIG_ADC_BUTTON_WAKE 0 /* 0:关闭按钮唤醒 */

// #define CONFIG_BATTERY_ADC 1
// #define PIN_BATTERY_ADC PA2
// #define PIN_BATTERY_CHARGE PD0
// #define PIN_BATTERY_ADC_CON PD3

#define PIN_GPIO_WAKEUP_KEY PA1
#define PIN_GPIO_WAKEUP_KEY2  PA3

#define PIN_TOUCH_PAD          PB8

/* 模拟电源按键 */
#define SIMULATED_POWER_KEY 1
#define POWER_KEY_ADC   /* 配置则读取模拟电源键adc值，不配置则读取模拟电源键gpio值 */
#define POWER_KEY_PIN       PA3
#define POWER_KEY_VAL_MAX   650
#define POWER_KEY_VAL_MIN   450

#endif /* __BOARD_CONFIG_H__ */

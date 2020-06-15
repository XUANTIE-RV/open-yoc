/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__
#include <pin_name.h>

/* You can add user configuration items here. */

#define CONFIG_WIFI_SMARTLIVING 1 /* 飞燕配网 */
#define CONFIG_SMARTLIVING_MQTT 0 /* 飞燕mqtt发布及订阅 */

#define APP_FOTA_EN 1  /* 升级 */

#if (defined(BOARD_MIT_V2) || defined(BOARD_MIT_V3)) 
#define CONFIG_MIT_LPMVAD 1  
#endif

#if defined(BOARD_CB5654)
#define CONFIG_TEST_LPMVAD 1  
#endif

#define APP_LPM_CHECK_TIME 5  /* 秒 进入低功耗的检查时间 */
#define APP_LPM_WL_WAKE_HOST_EN 1 /* WIFI 唤醒主控 */
#define APP_LPM_IDLE_CHECK_TIME 30 /* 秒 */
#define APP_LPM_VAD_PIN PA1         /* VAD低功耗唤醒引脚 */
#define APP_LPM_DEEP_WAKEUP_PIN PA1 /* 深度睡眠唤醒引脚 */

#define CONFIG_BATTERY_ADC 1   /* 电池检测 */
#define PIN_BATTERY_ADC -1     /* 电池电量检测，-1：变量模拟一个电压值 */
#define PIN_BATTERY_CHARGE_PIN -1 /* 充电状态检测, -1不检查 */

#define BATTERY_AVG_ROUND   10    /* 电池电量检查的平均次数 */
#define BATTERY_VOLT_LOW    3800  /* 小于该值控制音量输出 */
#define BATTERY_VOLT_MIN    3700  /* 电量小于该值关机 */
#define BATTERY_VOLT_DIFF   100   /* 平均时候删除异常点的阈值 */

#if defined(BOARD_CB5654)
#define APP_EQ_EN 1 /* 软件EQ使能配置 */
#else
#define APP_EQ_EN 0 /* 软件EQ使能配置 */
#endif

#if (defined(BOARD_MIT_V2) || defined(BOARD_MIT_V3))
#define INIT_VOL 0
#else
#define INIT_VOL 20
#endif

#endif

/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _BAORD_BUTTON_H_
#define _BAORD_BUTTON_H_

#include <yoc/button.h>

/* 物理按键ID定义 */
#define BUTTON_ID_NULL        0
#define BUTTON_ID_VOL_UP      1
#define BUTTON_ID_VOL_DONW    2
#define BUTTON_ID_MUTE        3
#define BUTTON_ID_PLAY        4
#define BUTTON_ID_USER        5
//#define BUTTON_ID_VOL_UP_DOWN 6
//#define BUTTON_ID_VOL_UP_MUTE 7

/* 按键配置 */
#define BUTTON_ADC_CHANNEL              8    /* GPIO16 */

#define BUTTON_ADC_VALUE_VOL_UP         6052 /*K5*/
#define BUTTON_ADC_VALUE_VOL_UP_RANGE   500

#define BUTTON_ADC_VALUE_VOL_DONW       4220 /*K6*/
#define BUTTON_ADC_VALUE_VOL_DONW_RANGE 500

#define BUTTON_ADC_VALUE_MUTE           8729 /*K7*/
#define BUTTON_ADC_VALUE_MUTE_RANGE     500

#define BUTTON_ADC_VALUE_PLAY           300  /*K4*/
#define BUTTON_ADC_VALUE_PLAY_RANGE     300

#define BUTTON_ADC_VALUE_USER           1892 /*K3*/
#define BUTTON_ADC_VALUE_USER_RANGE     500

//#define BUTTON_ADC_VALUE_VOL_UP_DOWN       1 /*预留组合键定义*/
//#define BUTTON_ADC_VALUE_VOL_UP_DOWN_RANGE 1

//#define BUTTON_ADC_VALUE_VOL_UP_MUTE       1 /*预留组合键定义*/
//#define BUTTON_ADC_VALUE_VOL_UP_MUTE_RANGE 1

/* 按键事件 */
#define KEY_MSG_VOL_UP      0
#define KEY_MSG_VOL_DONW    1
#define KEY_MSG_MUTE        2
#define KEY_MSG_PLAY        3
#define KEY_MSG_USER        4
#define KEY_MSG_WIFI_PROV   5
#define KEY_MSG_BT          6
#define KEY_MSG_POWER       7
#define KEY_MSG_UPLOG       8
#define KEY_MSG_FACTORY     9
#define KEY_MSG_VOL_UP_LD   10
#define KEY_MSG_VOL_DONW_LD 11
#define KEY_MSG_COUNT       12

/**
 * 按键初始化
 */
void board_button_init(button_evt_cb_t keymsg_cb);

#endif
/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _BAORD_BUTTON_H_
#define _BAORD_BUTTON_H_

#include <yoc/button.h>

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
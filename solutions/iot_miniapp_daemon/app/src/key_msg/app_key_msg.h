/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _APP_KEY_MSG_H_
#define _APP_KEY_MSG_H_

/* 按键事件 */
#define KEY_MSG_VOL_UP      0
#define KEY_MSG_VOL_DOWN    1
#define KEY_MSG_MUTE        2
#define KEY_MSG_PLAY        3
#define KEY_MSG_USER        4
#define KEY_MSG_WIFI_PROV   5
#define KEY_MSG_BT          6
#define KEY_MSG_POWER       7
#define KEY_MSG_UPLOG       8
#define KEY_MSG_FACTORY     9
#define KEY_MSG_VOL_UP_LD   10
#define KEY_MSG_VOL_DOWN_LD 11
#define KEY_MSG_COUNT       12

/**
 * @brief 按键消息处理初始化
 */
void app_key_msg_init(void);

/**
 * @brief 发送按键消息
 * 该函数由按键处理回调调用，button时间转化到具体的按键消息
 * 该函数声明需要和button_evt_cb_t保持一致
 * @param[in] keymsg_id 按键消息ID，见KEY_MSG开头的宏
 * @return void
 */
void app_key_msg_send(int keymsg_id, void *priv);

#endif

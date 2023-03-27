/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _APP_DISP_SHOW_H_
#define _APP_DISP_SHOW_H_

#include "app_disp_ops.h"

typedef enum
{
    DISP_SHOW_NOTHING = 0,
    DISP_SHOW_BOOT,           /*启动效果*/
    DISP_SHOW_WIFI_PROV,      /*进入配网模式*/
    DISP_SHOW_WIFI_CONN_FAIL, /*网络连接失败*/
    DISP_SHOW_WAKEUP,         /*唤醒等待语音输入*/
    DISP_SHOW_MUTE,           /*静音*/
    DISP_SHOW_PLAY,           /*播放音乐*/
    DISP_SHOW_KEYPRESS,       /*按键按下*/
    DISP_SHOW_COUNT
} display_show_t;

int app_display_show(display_show_t show);
int app_display_init(void);
void app_display_update(void);

#endif

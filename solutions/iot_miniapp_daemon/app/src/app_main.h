/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <uservice/eventid.h>

/*************
 * USER EVENT LIST
 ************/
/* 网络处理 */
#define EVENT_NTP_RETRY_TIMER            (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER            (EVENT_USER + 2)
#define EVENT_NET_LPM_RECONNECT          (EVENT_USER + 4)
#define EVENT_NET_RECONNECT              (EVENT_USER + 5)

/* 其他事件 */
#define EVENT_PA_CHECK       (EVENT_USER + 14)

void board_yoc_init(void);
void app_cli_init(void);

#define MAX_DEVICES_META_NUM CONFIG_GW_MAX_DEVICES_META_NUM

#endif
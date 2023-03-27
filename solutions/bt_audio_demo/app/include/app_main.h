/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include <uservice/eventid.h>

void board_yoc_init(void);
void app_cli_init(void);

/* 事件定义 */
#define EVENT_CPUUSAGE_CHECK (EVENT_USER + 13)
#define EVENT_PA_CHECK       (EVENT_USER + 14)

#endif

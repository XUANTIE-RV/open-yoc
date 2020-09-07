/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_
#include <stdint.h>
#include <yoc/uservice.h>
#include <yoc/netmgr_service.h>

extern netmgr_hdl_t app_netmgr_hdl;

void board_yoc_init(void);

void board_cli_init(utask_t *task);

/* exception */
void app_exception_init(void);
void app_exception_event(uint32_t event_id);

/* user define event */
#define EVENT_DATA_REPORT  (EVENT_USER + 1)
#define EVENT_CHANNEL_CHECK (EVENT_USER + 2)
#define EVENT_CHANNEL_CLOSE (EVENT_USER + 3)
#define EVENT_GNSS_GET (EVENT_USER + 4)

#endif

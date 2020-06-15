/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <yoc/uservice.h>
#include <yoc/netmgr_service.h>

void board_cli_init(utask_t *task);

#include <stdint.h>
extern netmgr_hdl_t app_netmgr_hdl;

void get_sensor_value(char *name, int *temp, int *humi);

void app_exception_event(uint32_t event_id);
void board_yoc_init(void);

/* user define event */
#define EVENT_DATA_REPORT  (EVENT_USER + 1)
#define EVENT_CHANNEL_CHECK (EVENT_USER + 2)
#define EVENT_CHANNEL_CLOSE (EVENT_USER + 3)

#endif
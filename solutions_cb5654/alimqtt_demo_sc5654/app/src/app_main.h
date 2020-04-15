/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_MAIN_H_
#define  _APP_MAIN_H_

#include <hw_config.h>

#include <stdbool.h>
#include <stdint.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <aos/cli.h>
#include <aos/cli_cmd.h>
#include <yoc/netmgr.h>
#include <yoc/eventid.h>

/* app init */
void yoc_base_init(void);

void get_sensor_value(char *name, int *temp, int *humi);

/* user define event */
#define EVENT_DATA_REPORT  (EVENT_USER + 1)
#define EVENT_CHANNEL_CHECK (EVENT_USER + 2)
#define EVENT_CHANNEL_CLOSE (EVENT_USER + 3)
#endif

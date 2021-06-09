/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <uservice/uservice.h>
#include <yoc/netmgr_service.h>
#include <stdint.h>
#include <yoc/partition.h>
#include <aos/kv.h>
#include <yoc/init.h>

void board_cli_init();
void app_exception_event(uint32_t event_id);
void board_yoc_init(void);

#endif
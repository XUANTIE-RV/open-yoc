/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _APP_NET_H_
#define _APP_NET_H_

//#include <app_config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/kv.h>

#include <yoc/yoc.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>

/* wifi & net */
typedef enum {
    MODE_WIFI_TEST = -2,
    MODE_WIFI_CLOSE = -1,
    MODE_WIFI_NORMAL = 0,
    MODE_WIFI_PAIRING = 1
} wifi_mode_e;

wifi_mode_e app_network_init(void);




/* event id define */
#define EVENT_NTP_RETRY_TIMER       (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER       (EVENT_USER + 2)
#define EVENT_NET_NTP_SUCCESS       (EVENT_USER + 3)
#define EVENT_NET_LPM_RECONNECT     (EVENT_USER + 4)



#endif

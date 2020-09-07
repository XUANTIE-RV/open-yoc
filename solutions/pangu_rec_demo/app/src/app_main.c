/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include <stdio.h>

#include <yoc/uservice.h>
#include <yoc/eventid.h>

#include "app_init.h"
#include <pin_name.h>
#include <devices/devicelist.h>
#include <devices/rtl8723ds.h>
#include "app_main.h"
#include "app_init.h"
#include <pin.h>
#include <yoc/netmgr_service.h>
#include <posix_init.h>

#define TAG "app"

extern void netmgr_service_init(utask_t *task);

/* network event callback */
void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_NETMGR_GOT_IP) {
        LOGD(TAG, "Net up");

    } else if (event_id == EVENT_NETMGR_NET_DISCON) {
        if ((int)param == NET_DISCON_REASON_DHCP_ERROR) {
            LOGD(TAG, "Net down");
            netmgr_reset(netmgr_get_handle("wifi"), 30);
        }
    }
    else {
        ;
    }

}

static netmgr_hdl_t _network_init()
{

    /* kv config check */
    aos_kv_setint("wifi_en", 1);

    netmgr_hdl_t netmgr_hdl = netmgr_dev_wifi_init();

    netmgr_service_init(NULL);
    if (netmgr_hdl)
        netmgr_start(netmgr_hdl);

    return netmgr_hdl;
}

void main()
{
    board_yoc_init();
    posix_init();

     /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };

    wifi_rtl8723ds_register(&pin);
    netmgr_get_dev(_network_init());
      /* 系统事件订阅 */
    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);
}


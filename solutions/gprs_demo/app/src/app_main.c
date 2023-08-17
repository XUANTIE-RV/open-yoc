/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>

#include <uservice/eventid.h>
#include <uservice/event.h>

#include "app_main.h"

#define TAG "main"

#include "usbh_core.h"
#include "usbh_rndis.h"
#include "usbh_serial.h"

#include "drv_usbh_class.h"
#include "ntp.h"

netmgr_hdl_t app_netmgr_hdl;

static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
        case EVENT_NETMGR_GOT_IP: {
            LOGI(TAG, "Got IP");
            event_publish(EVENT_NTP_RETRY_TIMER, NULL);
        } break;
        case EVENT_NETMGR_NET_DISCON: {
            LOGI(TAG, "Net down");
        } break;
        case EVENT_NTP_RETRY_TIMER: {
            LOGI(TAG, "NTP Start");
            if (ntp_sync_time(NULL) == 0) {
                event_publish(EVENT_NET_NTP_SUCCESS, NULL);
            } else {
                event_publish_delay(EVENT_NTP_RETRY_TIMER, NULL, 6000);
            }
         } break;
         case EVENT_NET_NTP_SUCCESS: {
            LOGI(TAG, "NTP Success");
         } break;
    }
}

#if defined(CONFIG_RNDIS_DEVICE_ETH) && CONFIG_RNDIS_DEVICE_ETH
void usbd_configure_done_callback()
{
    aos_debug_printf("%s", __func__);
}
#endif

int main(int argc, char *argv[])
{
    board_yoc_init();

    LOGD(TAG, "build time: %s, %s\r\n", __DATE__, __TIME__);

#if defined(CONFIG_RNDIS_DEVICE_ETH) && CONFIG_RNDIS_DEVICE_ETH
extern void drv_rndis_device_eth_register(void);
    drv_rndis_device_eth_register();

    app_netmgr_hdl = netmgr_dev_eth_init();
#else
    USBH_REGISTER_RNDIS_CLASS();
    USBH_REGISTER_USB_SERIAL_CLASS();
    usbh_initialize();

    drv_ec200a_serial_register(0);

    drv_ec200a_rndis_register();

    app_netmgr_hdl = netmgr_dev_gprs_init();
#endif

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 10 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
    }

    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);

    event_subscribe(EVENT_NTP_RETRY_TIMER, user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_NTP_SUCCESS, user_local_event_cb, NULL);
}

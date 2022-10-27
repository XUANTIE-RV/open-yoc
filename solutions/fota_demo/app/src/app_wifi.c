#include <stdbool.h>
#include "board.h"
#include "app_main.h"
#include <aos/yloop.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <uservice/eventid.h>
#include <uservice/event.h>
#include <ntp.h>
#ifdef CONFIG_WIFI_XR829
#include <devices/xr829.h>
#elif defined(CONFIG_WIFI_DRIVER_RTL8723)
#include <devices/rtl8723ds.h>
#elif defined(CONFIG_WIFI_DRIVER_BL606P)
#include <devices/bl606p_wifi.h>
#elif defined(CONFIG_WIFI_DRIVER_W800)
#include <devices/w800.h>
#elif defined(CONFIG_WIFI_DRIVER_HI3861L)
#include <hi3861l_devops.h>
#else
#error "No WiFi driver found."
#endif


#define TAG "init"

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
            app_fota_start();
         } break;
    }
    /*do exception process */
    app_exception_event(event_id);
}

void app_network_init(void)
{
    /* init wifi driver and network */
#ifdef CONFIG_WIFI_XR829
    wifi_xr829_register(NULL);
#elif defined(CONFIG_WIFI_DRIVER_RTL8723)
    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);
#elif defined(CONFIG_WIFI_DRIVER_BL606P)
    aos_loop_init();
    wifi_bl606p_register(NULL);
#elif defined(CONFIG_WIFI_DRIVER_W800)
    w800_wifi_param_t w800_param;
    /* init wifi driver and network */
    w800_param.reset_pin      = PA21;
    w800_param.baud           = 1*1000000;
    w800_param.cs_pin         = PA15;
    w800_param.wakeup_pin     = PA25;
    w800_param.int_pin        = PA22;
    w800_param.channel_id     = 0;
    w800_param.buffer_size    = 4*1024;
    wifi_w800_register(NULL, &w800_param);
#elif defined(CONFIG_WIFI_DRIVER_HI3861L)
    wifi_hi3861l_register(NULL);
#else
#error "No WiFi driver found."
#endif

    app_netmgr_hdl = netmgr_dev_wifi_init();

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 10 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
    }

    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);
    /* 使用系统事件的定时器 */
    event_subscribe(EVENT_NTP_RETRY_TIMER, user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_NTP_SUCCESS, user_local_event_cb, NULL);
}


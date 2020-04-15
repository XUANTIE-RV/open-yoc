#include <yoc_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <aos/debug.h>
#include <aos/log.h>
#include <yoc/netmgr.h>
#include <yoc/eventid.h>

#include "app_main.h"

#define TAG "AppExp"

#define IOT_RECCONNECT_DELAY_TIME   5 /*second*/
#define NET_RESET_DELAY_TIME        15 /*second*/

#define MAX_IOT_PUSH_ERR_TIMES      5
#define MAX_IOT_CONNECT_ERR_TIMES   6
#define MAX_NET_RESET_ERR_TIMES     3

static int iot_push_err_times =    0;
static int iot_connect_err_times = 0;
static int net_reset_err_times =   0;
static int net_do_reset_times =    0;

static void app_except_process(int errno, const char *file, int line, const char *func_name, void *caller)
{
    LOGE(TAG, "Except! errno is %s, function: %s at %s:%d, caller: 0x%x\n", strerror(errno), func_name, file, line, caller);

#ifdef CONFIG_DEBUG
    while(1);
#else
    aos_reboot();
#endif
}

void app_exception_event(uint32_t event_id)
{
    switch(event_id) {
    case EVENT_NETMGR_NET_DISCON:
        LOGD(TAG, "EVENT_NETMGR_NET_DISCON");
        net_reset_err_times++;
        if (net_reset_err_times >= MAX_NET_RESET_ERR_TIMES) {
            LOGD(TAG, "Net Reset times %d, reboot", net_reset_err_times);
            //do reboot
            aos_reboot();
        } else {
            LOGD(TAG, "Net Reset after %d second", NET_RESET_DELAY_TIME);
            netmgr_reset(app_netmgr_hdl, NET_RESET_DELAY_TIME);
        }
        break;
    case EVENT_NETMGR_GOT_IP:
        net_reset_err_times = 0;
        break;

    case EVENT_IOT_CONNECT_FAILED:
    case EVENT_IOT_DISCONNECTED:
        if(net_reset_err_times == 0) {
            LOGD(TAG, "IOT DISCONNECTED/FAILED");
            if (net_do_reset_times >= MAX_NET_RESET_ERR_TIMES) {
                LOGD(TAG, "Net Reset times %d, reboot", net_do_reset_times);
                //do reboot
                aos_reboot();
                break;
            }

            iot_connect_err_times++;
            if (iot_connect_err_times >= MAX_IOT_CONNECT_ERR_TIMES) {
                LOGD(TAG, "IoT Reconnect times %d, Reset net", iot_connect_err_times);
                //do net reset
                iot_connect_err_times = 0;
                netmgr_reset(app_netmgr_hdl, 0);
                net_do_reset_times ++;
            } else {
                LOGD(TAG, "IoT Reconnect after %d second", IOT_RECCONNECT_DELAY_TIME);
                event_publish_delay(EVENT_CHANNEL_CHECK, NULL, IOT_RECCONNECT_DELAY_TIME * 1000);
            }
        } else {
            iot_connect_err_times = 0;
            net_do_reset_times = 0;
        }

        break;
    case EVENT_IOT_CONNECT_SUCCESS:
        iot_connect_err_times = 0;
        net_do_reset_times = 0;
        break;

    case EVENT_IOT_PUSH_FAILED:
        LOGD(TAG, "PUSH_FAILED");
        iot_push_err_times++;
        if (iot_push_err_times >= MAX_IOT_PUSH_ERR_TIMES) {
            LOGD(TAG, "IoT Push err times=%d, Reconnect", iot_push_err_times);
            //do reconnect
            iot_push_err_times = 0;
            event_publish(EVENT_CHANNEL_CLOSE, NULL);
        }
        break;
    case EVENT_IOT_PUSH_SUCCESS:
        iot_push_err_times = 0;
        break;
    }
}

void app_exception_init(void)
{
    /* Register system except process */
    aos_set_except_callback(app_except_process);

    /* Set utask default wdt timeout */
#ifdef CONFIG_DEBUG
    utask_set_softwdt_timeout(0);
#else
    utask_set_softwdt_timeout(60000);
#endif
}

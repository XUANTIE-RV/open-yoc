
#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include "board.h"
#include "app_init.h"
#include "drv/pin.h"
#include <yoc/at_port.h>
#include <devices/w800.h>
#include <yoc/netmgr_service.h>

const char *TAG = "INIT";

#ifndef CONSOLE_UART_IDX
#define CONSOLE_UART_IDX 0
#endif

netmgr_hdl_t app_netmgr_hdl;

static void network_event(uint32_t event_id, const void *param, void *context)
{
    switch(event_id) {
    case EVENT_NETMGR_GOT_IP: {
        LOGD(TAG, "EVENT_NETMGR_GOT_IP");
    }
        break;
    case EVENT_NETMGR_NET_DISCON:
        LOGD(TAG, "EVENT_NETMGR_NET_DISCON");
        break;
    }

    /*do exception process */
    // app_exception_event(event_id);
}

static void network_init()
{
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
    app_netmgr_hdl = netmgr_dev_wifi_init();

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        //netmgr_config_wifi(app_netmgr_hdl, "TEST", 4, "TEST1234", 10);
        netmgr_start(app_netmgr_hdl);
        event_subscribe(EVENT_NETMGR_GOT_IP, network_event, NULL);
        event_subscribe(EVENT_NETMGR_NET_DISCON, network_event, NULL);
    }
}

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init()
{
    board_init();
    stduart_init();
    event_service_init(NULL);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    
    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    network_init();
    board_cli_init();
}

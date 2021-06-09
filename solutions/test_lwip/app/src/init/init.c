/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "board.h"
#include "app_config.h"
#include <devices/rtl8723ds.h>  //对应的wifi chip驱动，网络初始化需要，可根据实际情况替换

#define TAG "init"

netmgr_hdl_t app_netmgr_hdl;

static void network_init()
{
    //对应的wifi驱动初始化，可根据实际情况替换
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);

    app_netmgr_hdl = netmgr_dev_wifi_init();

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
    }

}

void board_yoc_init(void)
{
    board_init();
    console_init(CONSOLE_UART_IDX, 115200, 512);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    /* load partition */
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");

    event_service_init(NULL);

    board_cli_init();
    network_init();
}

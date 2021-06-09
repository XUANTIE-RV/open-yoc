/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>

#ifdef CONFIG_CSI_V2
#include <soc.h>
#include <drv/gpio_pin.h>
#else
#include <pinmux.h>
#include "pin_name.h"
#endif

#include "board_config.h"

const char *TAG = "INIT";

void board_yoc_init(void)
{
    board_init();

    console_init(CONSOLE_UART_IDX, 115200, 512);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");
    event_service_init(NULL);

    board_cli_init();
    LOGI(TAG, "Welcome to CLI");
}

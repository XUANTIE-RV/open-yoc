#include <yoc_config.h>

#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <devices/devicelist.h>

#include "pin_name.h"
#include "app_init.h"
#include "pin.h"

const char *TAG = "INIT";

#ifndef CONSOLE_IDX
#define CONSOLE_IDX 0
#endif

static void board_pinmux_config(void)
{
    drv_pinmux_config(CONSOLE_TXD, 0);
    drv_pinmux_config(CONSOLE_RXD, 0);
    drv_pinmux_config(CONSOLE_TXD_FUNC, 0);
    drv_pinmux_config(CONSOLE_RXD_FUNC, 0);
}

void board_init(void)
{
    board_pinmux_config();
}

void board_yoc_init()
{
    uart_csky_register(CONSOLE_IDX);
    console_init(CONSOLE_IDX, 115200, 128);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);
    /* load partition */
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    /* uService init */
    utask_t *task = utask_new("at&cli", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    board_cli_init(task);
}

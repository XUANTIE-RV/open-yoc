
#include <stdbool.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include <aos/hal/pwm.h>
#include <board.h>
#include "app_init.h"
#include "app_main.h"
#include "drv_light.h"

const char *TAG = "INIT";

void board_yoc_init()
{
    int ret;

    board_init();

#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT
    board_bt_init();
#endif

#if defined(CONFIG_BOARD_LED) && CONFIG_BOARD_LED
    board_led_init();
#endif

#if defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON
    board_button_init();
#endif

    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, 128);
    
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
    board_cli_init();
#endif

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    /* load partition */
    ret = partition_init();

    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    /* kvfs init */
    aos_kv_init("kv");

#ifdef MESH_NODE_REPEAT_RESET
    extern void reset_by_repeat_init(void);
    reset_by_repeat_init();
#endif

    LOGI(TAG, "Build:%s,%s", __DATE__, __TIME__);
}

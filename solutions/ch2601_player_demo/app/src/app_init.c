#include <aos/debug.h>
#include <aos/cli.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include "app_init.h"
#include "board.h"
#include "drv/pin.h"
#include "drv/gpio_pin.h"
#include <devices/drv_snd_alkaid.h>
#include <aos/kv.h>

#define TAG "INIT"

static void board_cli_init()
{
    aos_cli_init();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    extern void cli_reg_cmd_kvtool(void);
    cli_reg_cmd_kvtool();

}

void board_yoc_init()
{
    board_init();
    console_init(CONSOLE_UART_IDX, 115200, 128);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    LOGI(TAG, "[YoC]Build:%s,%s",__DATE__, __TIME__);

    /* load partition */
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");
    event_service_init(NULL);
    snd_card_alkaid_register(NULL);
    board_cli_init();
}

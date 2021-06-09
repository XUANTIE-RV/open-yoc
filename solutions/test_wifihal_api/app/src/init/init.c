#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <aos/cli.h>
#include <aos/kv.h>
#include <yoc/partition.h>
#include <aos/yloop.h>
#include <uservice/uservice.h>

#include "board_config.h"
#include <w800_devops.h>
#include "app_init.h"


const char *TAG = "INIT";

extern void cli_reg_cmds(void);

void board_yoc_init()
{
    board_base_init();
    board_init();
	event_service_init(NULL);
	aos_loop_init();

    console_init(CONSOLE_UART_IDX, 115200, 128);
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
	aos_cli_init();
    aos_kv_init("kv");
    wifi_w800_register(NULL);

    cli_reg_cmds();
    aos_loop_run();
}

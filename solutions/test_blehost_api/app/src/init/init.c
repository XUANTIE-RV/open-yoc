
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <aos/cli.h>
#include <aos/kv.h>
#include <yoc/partition.h>

#include "app_init.h"
#include <board_config.h>


const char *TAG = "INIT";

void board_yoc_init()
{
    int ret;

    board_init();
    console_init(CONSOLE_UART_IDX, 115200, 512);
    aos_cli_init();

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    
    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    /* load partition */
    ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    /* kvfs init */
    aos_kv_init("kv");

    extern void cli_cmd_reg(void);
    /* cli init */
    cli_cmd_reg();

}
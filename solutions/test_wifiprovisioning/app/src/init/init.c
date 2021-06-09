#include <aos/aos.h>
#include <yoc/yoc.h>
#include <aos/cli.h>
#include <aos/kv.h>
#include <yoc/partition.h>

#include "app_init.h"

#include "board_config.h"
#include <w800_devops.h>

#define CONSOLE_IDX 0

const char *TAG = "INIT";

extern void cli_reg_cmds(void);

void board_yoc_init(void)
{
    board_base_init();
    board_init();

    console_init(CONSOLE_IDX, 115200, 512);
    aos_cli_init();

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    aos_kv_init("kv");
    wifi_w800_register(NULL);

    cli_reg_cmds();
}

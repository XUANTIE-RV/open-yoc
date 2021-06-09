
#include <stdbool.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include <aos/kv.h>
#include <devices/devicelist.h>

#include "hci_hal_h4.h"
#include "pin_name.h"
#include "app_init.h"
#include "pin.h"

const char *TAG = "INIT";

#ifndef CONSOLE_IDX
#define CONSOLE_IDX 0
#endif

void board_yoc_init()
{
    uart_csky_register(CONSOLE_IDX);
    flash_csky_register(0);

    console_init(CONSOLE_IDX, 115200, 128);

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

    bt_w800_register();
    extern int hci_h4_driver_init();
    hci_h4_driver_init();
    board_cli_init();
}

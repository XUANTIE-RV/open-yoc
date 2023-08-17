#include <stdbool.h>
#include <board.h>
#include <aos/kv.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <uservice/uservice.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#endif
#include "app_main.h"

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    board_cli_init();
    printf("###YoC###[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
    aos_debug_init();
#endif
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
        if (aos_kv_init("kv")) {
            LOGE(TAG, "kv init failed.");
        }
    }
    app_network_init();
    app_wifi_init();
}

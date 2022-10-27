#include <stdbool.h>
#include <board.h>
#include <aos/kv.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <uservice/uservice.h>
#ifdef AOS_COMP_DEBUG
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
    printf("\n###Welcom to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    event_service_init(NULL);
#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
        aos_assert((ret > 0));
    } else {
        LOGI(TAG, "find %d partitions", ret);
        ret = aos_kv_init("kv");
        if (ret != 0) {
            LOGE(TAG, "kv init failed.");
            aos_assert((ret == 0));
        }
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
        extern int bootab_init(void);
        if (bootab_init() < 0) {
            LOGE(TAG, "bootab init failed.");
        }
#endif
        extern uint32_t km_init(void);
        km_init();
    }
    app_network_init();
}

#include <stdbool.h>
#include <aos/kv.h>
#include <aos/cli.h>
#include <debug/dbg.h>
#include <uservice/uservice.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include "board.h"

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
    aos_cli_init();

    printf("###YoC###[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));

    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    board_display_init();

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

#if defined(CONFIG_BOARD_VENDOR) && CONFIG_BOARD_VENDOR
    board_vendor_init();
#endif
}

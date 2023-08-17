#include <stdbool.h>
#include <uservice/uservice.h>
#include <yoc/init.h>
#include <aos/cli.h>
#include <aos/console_uart.h>

#ifdef AOS_COMP_DEBUG
#include <debug/dbg.h>
#endif

#include "board.h"
#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
#include <log_ipc.h>
#endif
#define TAG "init"

static void  mhint_dump(void)
{
    uint32_t mhint;

    __ASM volatile("csrr %0, mhint" : "=r"(mhint));
    LOGI("c906", "mhint 0x%08x\r\n", mhint);
}

static void stduart_init(void)
{
#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
    log_ipc_uart_register(CONSOLE_UART_IDX, log_ipc_rx_read, log_ipc_tx_write);
    log_ipc_cp_init(1);
#else
    rvm_uart_drv_register(CONSOLE_UART_IDX);
#endif
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    aos_cli_init();
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

    mhint_dump();
}

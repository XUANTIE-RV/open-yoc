/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/kv.h>
#include <uservice/uservice.h>
#include <yoc/init.h>
#include <ulog/ulog.h>
#include "board.h"
#include "app_main.h"
#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
#include <log_ipc.h>
#endif

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
    log_ipc_uart_register(CONSOLE_UART_IDX, log_ipc_rx_read, log_ipc_tx_write);
    log_ipc_cp_init(1);
#endif
}

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    printf("\n###Welcom to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    board_cli_init();
    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_INFO);
}

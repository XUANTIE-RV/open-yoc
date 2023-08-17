/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#include <yoc/init.h>
#include <yoc/partition.h>
#include <uservice/uservice.h>
#include <aos/kv.h>
#include <board.h>
#include <drv/uart.h>
#include <log_ipc.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#endif

#define TAG "init"
#define CACHE_ALIGN_MASK (__riscv_xlen-1)
#define CACHE_ALIGN_UP(a)          (((a) + CACHE_ALIGN_MASK) & ~CACHE_ALIGN_MASK)
#define CACHE_ALIGN_DOWN(a)        ((a) & ~CACHE_ALIGN_MASK)

#define ALG_CPUID 0
static void log_ipc_init(void)
{
    uint8_t cpu_id[] = { ALG_CPUID };

    log_ipc_ap_init(cpu_id, (int)sizeof(cpu_id));
}

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

extern void board_cli_init();

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    board_cli_init();
    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
    aos_debug_init();
#endif

    int ret = partition_init();
    if (ret >= 0) {
        printf("partition_init success partition num %d\r\n", ret);
    }

#ifdef CONFIG_ENABLE_BL606P_C906
    partition_t partition;
    unsigned long run_address;
    const char *name = "prim";

    partition = partition_open(name);
    if (partition_split_and_get(partition, 1, NULL, NULL, &run_address)) {
        printf("get [%s] run_address failed.\n", name);
    } else {
        board_load_amp_fw(0, (void*)run_address, NULL, 0);
    }
    partition_close(partition);

#endif

    aos_kv_init("kv");
}

#include <stdbool.h>
#include <uservice/uservice.h>
#include <yoc/init.h>
#include <yoc/partition.h>
#include <aos/kv.h>
#include "board.h"
#include <soc.h>
#include <blyoc_flash.h>
#include <yoc/partition.h>
#include <aos/kv.h>
#include <drv/uart.h>
#include <log_ipc.h>
#ifdef AOS_COMP_DEBUG
#include <debug/dbg.h>
#endif
#ifdef CONFIG_ENABLE_C906
#include "c906_fw.h"
#endif

#define TAG "init"
#define CACHE_ALIGN_MASK (__riscv_xlen-1)
#define CACHE_ALIGN_UP(a)          (((a) + CACHE_ALIGN_MASK) & ~CACHE_ALIGN_MASK)
#define CACHE_ALIGN_DOWN(a)        ((a) & ~CACHE_ALIGN_MASK)

#define ALG_CPUID 0
static void log_ipc_init(void)
{
    uint8_t cpu_id[] = { ALG_CPUID };

    ipc_log_ap_init(cpu_id, (int)sizeof(cpu_id));
}

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

extern void board_cli_init();

#ifdef CONFIG_ENABLE_C906
void boot_c906(int boot_addr);

static csi_uart_t uart_c906;
void c906_uart_init(int id)
{
    csi_uart_init(&uart_c906, id);
}

static void c906_init(void)
{
    c906_uart_init(2);
    log_ipc_init();
    memcpy((uint32_t *)g_c906_fw_addr, g_c906_fw, sizeof(g_c906_fw));
    csi_dcache_clean_invalid_range((uint32_t *)CACHE_ALIGN_DOWN(g_c906_fw_addr), CACHE_ALIGN_UP(sizeof(g_c906_fw)));
    boot_c906(g_c906_fw_addr);
}
#endif

void board_yoc_init(void)
{
    board_init();
#ifdef CONFIG_ENABLE_C906
    c906_init();
#endif
    stduart_init();
    board_cli_init();
    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif
    
    int ret = partition_init();
    if (ret >= 0) {
        printf("partition_init success partition num %d\r\n", ret);
    }
    aos_kv_init("kv");
}

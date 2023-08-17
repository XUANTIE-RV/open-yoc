#include <stdbool.h>
#include <board.h>
#include <board_config.h>
#include <aos/kv.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <aos/cli.h>
#include "fatfs_vfs.h"
#include "vfs.h"
#include "disk_sd.h"
#include <uservice/uservice.h>
#include <drv/dma.h>
#ifdef AOS_COMP_DEBUG
#include <debug/dbg.h>
#endif
#include "app_main.h"

#define TAG "init"
#define CONFIG_CONSOLE_UART_BUFSIZE (512)

// static csi_dma_t dma;

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

void board_yoc_init(void)
{
    int ret;

    board_init();
    stduart_init();
    extern void  cxx_system_init(void);
    cxx_system_init();
    
    printf("\n###Welcome to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    // csi_dma_init(&dma, 0); move to board component

    ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition_init failed(%d).\n", ret);
        aos_assert(false);
        return;
    }

#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    extern int bootab_init(void);
    if (bootab_init() < 0) {
        LOGE(TAG, "bootab init failed.");
        aos_assert(false);
    }
#endif

    /* load weiht for fast startup */
    extern int app_cx_hw_init();
    app_cx_hw_init();

    ret = aos_vfs_init();
    if (ret != 0) {
        LOGE(TAG, "aos_vfs_init failed(%d).\n", ret);
        return;
    }

    ret = app_sd_detect_check();
    LOGE(TAG, ret ? "fatfs enable.\n" : "fatfs disable.\n");

    if (ret == 1) {
        ret = vfs_fatfs_register();
        if (ret != 0)
            LOGE("app", "fatfs register failed(%d).\n", ret);
    }

    ret = aos_cli_init();
    if (ret == 0) {
        board_cli_init();
    }
#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif
    return;
}

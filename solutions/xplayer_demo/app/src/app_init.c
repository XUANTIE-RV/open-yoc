#include <stdbool.h>
#include <aos/kv.h>
#include <debug/dbg.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <uservice/event.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <vfs.h>
#include <vfs_cli.h>
#include <littlefs_vfs.h>
#include "board.h"
#include "app_main.h"

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

static void fs_init(void)
{
    int ret;

    aos_vfs_init();
    ret = vfs_lfs_register("lfs");
    if (ret != 0) {
        LOGE(TAG, "littlefs register failed(%d)", ret);
        return;
    }
    LOGI(TAG, "filesystem init ok.");

    cli_reg_cmd_ls();
    cli_reg_cmd_rm();
    cli_reg_cmd_cat();
    cli_reg_cmd_mkdir();
    cli_reg_cmd_mv();
}

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    board_cli_init();
    printf("\n###Welcom to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    event_service_init(NULL);
    aos_debug_init();
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
    board_audio_init();

    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
        if (aos_kv_init("kv")) {
            LOGE(TAG, "kv init failed.");
        }
        fs_init();
    }
    app_network_init();
}

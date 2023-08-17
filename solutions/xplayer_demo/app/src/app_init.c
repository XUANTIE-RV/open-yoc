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
#include "board.h"
#include "app_main.h"

#if !defined(CONFIG_FS_EXT4) && !defined(CONFIG_FS_FAT) && !defined(CONFIG_FS_LFS)
#error "Please define a filesystem type."
#endif

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

static void fs_init(void)
{
    int ret;
    int init_ok = 0;

    aos_vfs_init();
#ifdef CONFIG_FS_EXT4
    extern int vfs_ext4_register(void);
    ret = vfs_ext4_register();
    if (ret != 0) {
        LOGE(TAG, "ext4 register failed(%d).", ret);
    } else {
        init_ok = 1;
        LOGD(TAG, "ext4 register ok.");
    }
#endif
#ifdef CONFIG_FS_FAT
    extern int vfs_fatfs_register(void);
    ret = vfs_fatfs_register();
    if (ret != 0) {
        LOGE(TAG, "fatfs register failed(%d).", ret);
    } else {
        init_ok = 1;
        LOGD(TAG, "fatfs register ok.");
    }
#endif
#ifdef CONFIG_FS_LFS
    extern int32_t vfs_lfs_register(char *partition_desc);
    ret = vfs_lfs_register("lfs");
    if (ret != 0) {
        LOGE(TAG, "littlefs register failed(%d)", ret);
    } else {
        init_ok = 1;
        LOGD(TAG, "littlefs register ok.");
    }
#endif
    if (init_ok) {
        LOGI(TAG, "filesystem init ok.");
    } else {
        LOGE(TAG, "filesystem init failed.");
    }
}

void board_yoc_init(void)
{
    board_init();
    stduart_init();
    printf("\n###Welcome to YoC###\n[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    board_cli_init();
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

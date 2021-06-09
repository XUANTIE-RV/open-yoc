/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/cli.h>
#include "app_main.h"
#include "littlefs_vfs.h"
#include "fatfs_vfs.h"

#define TAG "init"

int g_fct_mode = 0;

void cli_reg_cmd_ping(void);
void cli_reg_cmd_ifconfig_wifi(void);
void cli_reg_cmd_kvtool(void);
void cli_reg_cmd_ntp(void);
void cli_reg_cmd_ps(void);
void cli_reg_cmd_free(void);
void cli_reg_cmd_sysinfo(void);
void cli_reg_cmd_factory(void);
void cli_reg_cmd_ls(void);
void cli_reg_cmd_rm(void);
void cli_reg_cmd_cat(void);
void cli_reg_cmd_mkdir(void);
void cli_reg_cmd_mv(void);

static void cli_init()
{
    aos_cli_init();

    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig_wifi();
    cli_reg_cmd_kvtool();
    cli_reg_cmd_ntp();
    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_sysinfo();
    cli_reg_cmd_factory();
}

static void fs_init()
{
    int ret;
    int fatfs_en = 0;

    vfs_init();

    fatfs_en = app_sd_detect_check();
    LOGD(TAG, fatfs_en ? "fatfs enable" : "fatfs disable");

    if (fatfs_en == 1) {
        ret = vfs_fatfs_register();
        if (ret != 0) {
            /* fatfs may be mount fail sometimes, the reason is poor contact of sd */
            LOGE(TAG, "fatfs register failed(%d)", ret);
        }
    }

    ret = vfs_lfs_register("lfs");
    if (ret != 0) {
        LOGE(TAG, "littlefs register failed(%d)", ret);
    }

    if (!g_fct_mode) {
        /* fs cmd */
        cli_reg_cmd_ls();
        cli_reg_cmd_rm();
        cli_reg_cmd_cat();
        cli_reg_cmd_mkdir();
        cli_reg_cmd_mv();
    }
}

void yoc_base_init(void)
{
    int ret;

    board_init();
    console_init(CONSOLE_UART_IDX, 115200, 512);

    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);

    LOGI(TAG, "Build:%s,%s",__DATE__, __TIME__);

    /* load partition */
    ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    /* kvfs init */
    aos_kv_init("kv");

    /* event service init */
    event_service_init(NULL);

    int fct_code;
    aos_kv_getint("factory_test_mode", &fct_code);
    if (fct_code == 9721) {
        g_fct_mode = 1;
    }

    if (!g_fct_mode) {
        /* cli init */
        cli_init();
    }

    /* file system init */
    fs_init();
}

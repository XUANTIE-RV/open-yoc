/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"

#define TAG "init"

int g_fct_mode = 0;

static void cli_init(utask_t *task)
{
    if (task == NULL)
        return;

    cli_service_init(task);

    cli_reg_cmd_help();

    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig_wifi();
    cli_reg_cmd_ntp();

    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_sysinfo();

    cli_reg_cmd_factory();
    cli_reg_cmd_kvtool();

}

static void fs_init()
{
    int ret;
    int fatfs_en = 0;

    aos_kv_getint("fatfs_en", &fatfs_en);

    LOGD(TAG, fatfs_en ? "fatfs enable" : "fatfs disable");

    if (!fatfs_en) {
        return;
    }

    vfs_init();
    ret = vfs_fatfs_register();
    if (ret != 0) {
        LOGI(TAG, "fatfs register failed(%d)", ret);
        return;
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

    console_init(CONSOLE_ID, 115200, 512);

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
        utask_t *task = utask_new("cli", 3 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        cli_init(task);
    }

    /* file system init */
    fs_init();
}

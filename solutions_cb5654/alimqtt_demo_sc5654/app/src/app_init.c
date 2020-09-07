/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>

#include <devices/devicelist.h>
#include <pin_name.h>
#include "app_main.h"

#define TAG "init"

static void cli_init(utask_t *task)
{
    if (task == NULL)
        task = utask_new("cli" ,8 * 1024, QUEUE_MSG_COUNT, 32);

    if (task == NULL)
        return;

    cli_service_init(task);

    cli_reg_cmd_help();

    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_sysinfo();

    cli_reg_cmd_factory();
    cli_reg_cmd_kvtool();
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

    aos_kv_init("kv");

    event_service_init(NULL);

    /* uService init */
    utask_t *task = utask_new("at&cli", 3 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    cli_init(task);
}

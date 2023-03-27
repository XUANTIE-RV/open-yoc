/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <yoc/nvram.h>
#include <yoc/sysinfo.h>

#include <yoc/partition.h>

#include <vfs.h>
#include <vfs_cli.h>

#include <littlefs_vfs.h>

#include "app_sys.h"

#define TAG                     "AppFs"
#define DEVICE_NAME_FORMAT      "YOC-%s"

static void vfs_init(void)
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
    cli_reg_cmd_df();
    cli_reg_cmd_cp();
}

void app_sys_fs_init()
{
    int ret = partition_init(); 
    if ( ret <= 0) {
        LOGE(TAG, "partition init failed");
    } else {
        LOGI(TAG, "find %d partitions", ret);
    }

    if (aos_kv_init("kv")) {
        LOGE(TAG, "kv init failed.");
    }
#ifdef CONFIG_STANDALONE_NVRAM
    if (nvram_init("fct")) {
        LOGE(TAG, "nvram init failed.");
    }
#endif
    vfs_init();

#if defined(CONFIG_TB_KP) && CONFIG_TB_KP
    extern uint32_t km_init(void);
    km_init();
#endif
}



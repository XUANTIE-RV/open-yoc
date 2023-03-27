/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <yoc/partition.h>
#include <aos/kv.h>
#include <ulog/ulog.h>

#include "app_sys.h"

#define TAG "APP_SYS"

void app_sys_init()
{
    /* file system initialization */
    int ret = partition_init();
    if (ret >= 0) {
        LOGI(TAG, "partition_init success partition num %d\r\n", ret);
    }
    aos_kv_init("kv");

#ifdef CONFIG_DEBUG
    app_sys_except_init(APPEXP_MODE_DEV);
#else
    app_sys_except_init(APPEXP_MODE_RLS);
#endif

}

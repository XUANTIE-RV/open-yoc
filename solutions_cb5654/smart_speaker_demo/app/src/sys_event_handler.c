/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <app_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <aos/debug.h>

#include <yoc/netmgr_service.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

#define TAG "AppExp"

static void app_except_process(int errno, const char *file, int line, const char *func_name, void *caller)
{
    LOGE(TAG, "Except! errno is %s, function: %s at %s:%d, caller: 0x%x\n", strerror(errno), func_name, file, line, caller);

#ifdef CONFIG_DEBUG
    while(1);
#else
    aos_reboot();
#endif
}

void sys_event_init(void)
{
    /*
    * 注册系统异常处理,aos_check_return_xxx 相关函数
    * 若错误，会调用到回调函数，可以做异常处理
    */
    aos_set_except_callback(app_except_process);

    /* 默认设置为0，软狗不生效 */
    utask_set_softwdt_timeout(0);

    /* 若uService中的任务有超过指定时间没有退出，软狗系统会自动重启 */
    //utask_set_softwdt_timeout(60000);
}

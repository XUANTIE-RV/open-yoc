/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/debug.h>
#include <aos/wdt.h>
#include <debug/dbg.h>
#include <yoc/netmgr.h>
#include <uservice/uservice.h>
#include <csi_core.h>
#include "app_sys.h"

#define TAG "AppExp"

static void app_except_process(int errno, const char *file, int line, const char *func_name,
                               void *caller)
{
    printf("App Except! errno is %s, function: %s at %s:%d, caller: %p\n",
            strerror(errno) ? strerror(errno) : "NULL",
            func_name ? func_name : "null", file ? file : "null", line, caller ? caller : "null");
    ulog_flush();
    csi_irq_save();
    aos_reboot();
}

static void app_except_process_debug(int errno, const char *file, int line, const char *func_name,
                                     void *caller)
{
    printf("Debug Except! errno is %s, function: %s at %s:%d, caller: %p\n",
            strerror(errno) ? strerror(errno) : "NULL",
            func_name ? func_name : "null", file ? file : "null", line, caller ? caller : "null");
    ulog_flush();
    csi_irq_save();
    while (1);
}

void app_sys_except_init(int debug)
{
    /*
    * 注册系统异常处理,aos_check_return_xxx 相关函数
    * 若错误，会调用到回调函数，可以做异常处理
    */
    if (debug == APPEXP_MODE_FCT) {
        /* 强制Debug，主要用于产测 */
        LOGI(TAG, "Fct mode");
        aos_set_except_callback(app_except_process_debug);
        utask_set_softwdt_timeout(0);
    } else if (debug == APPEXP_MODE_DEV) {
        LOGI(TAG, "Debug mode");
        /* 开发调试，开启软狗，但超时不重启 */
        aos_set_except_callback(app_except_process_debug);
        utask_set_softwdt_timeout(25000);
    } else {
        //APPEXP_MODE_RLS
        LOGI(TAG, "Release mode");
        /* 关闭异常调试命令行 */
        aos_debug_panic_cli_enable(0);

        /* 发布模式，开启软狗和硬狗 */
        aos_wdt_debug(0);
        aos_set_except_callback(app_except_process);

        /* 若uService中的任务有超过指定时间没有退出，软狗系统会自动重启 */
        utask_set_softwdt_timeout(25000);

        /* 使能硬狗，硬狗以软狗为基础，需先使能软件狗*/
        aos_wdt_hw_enable(0, 30000);
    }
}


/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/ringbuffer.h>

#include <ulog/ulog.h>
#include <uservice/uservice.h>

#include "app_disp.h"

#define TAG "DISP"

static uservice_t *g_display_srv;
char *g_display_show_desc[DISP_SHOW_COUNT] = 
{
    "NOTHING",
    "BOOT",           /*启动效果*/
    "WIFI_PROV",      /*进入配网模式*/
    "WIFI_CONN_FAIL", /*网络连接失败*/
    "WAKEUP",         /*唤醒等待语音输入*/
    "MUTE",           /*静音*/
    "PLAY",           /*播放音乐*/
    "KEYPRESS",       /*按键按下*/
};

static int display_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, g_display_cmd_table);
}

int app_display_show(display_show_t show)
{
    int ret = -1;

    aos_check_return_einval(g_display_srv);
    char * desc = g_display_show_desc[show];

    ret = uservice_call_async(g_display_srv, show, &desc, sizeof(char*));
    return ret;
}

int app_display_init(void)
{
    utask_t *task_disp = utask_new("disp_task", 4 * 1024, 20, AOS_DEFAULT_APP_PRI);

    g_display_srv = uservice_new("disp_task", display_process_rpc, &g_display_srv);

    utask_add(task_disp, g_display_srv);

    return 0;
}

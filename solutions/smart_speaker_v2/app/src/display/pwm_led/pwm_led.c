/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <ulog/ulog.h>
#include <uservice/uservice.h>

#include "display/app_disp.h"

#define TAG "PWMLED"

static int disp_show_nothing(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_boot(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_wifi_prov(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_wifi_conn_fail(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_wakeup(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_mute(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_play(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}
static int disp_show_keypress(void *context, rpc_t *rpc)
{
    char *desc = rpc_get_point(rpc);

    LOGD(TAG, "%s", desc);

    return 0;
}

const rpc_process_t g_display_cmd_table[] = {
    { DISP_SHOW_NOTHING, (process_t)disp_show_nothing },               /*启动效果*/
    { DISP_SHOW_BOOT, (process_t)disp_show_boot },                     /*进入配网模式*/
    { DISP_SHOW_WIFI_PROV, (process_t)disp_show_wifi_prov },           /*网络连接失败*/
    { DISP_SHOW_WIFI_CONN_FAIL, (process_t)disp_show_wifi_conn_fail }, /*唤醒等待语音输入*/
    { DISP_SHOW_WAKEUP, (process_t)disp_show_wakeup },                 /*唤醒*/
    { DISP_SHOW_MUTE, (process_t)disp_show_mute },                     /*静音*/
    { DISP_SHOW_PLAY, (process_t)disp_show_play },                     /*播放音乐*/
    { DISP_SHOW_KEYPRESS, (process_t)disp_show_keypress },             /*按键按下*/
    { DISP_SHOW_COUNT, (process_t)NULL },
};

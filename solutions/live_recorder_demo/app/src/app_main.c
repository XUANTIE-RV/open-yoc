/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <aos/kv.h>
#include <aos/debug.h>
#ifdef AOS_COMP_DEBUG
#include <debug/dbg.h>
#endif
#include <cx/init.h>
#include <cx/record.h>
#include "media_video.h"
#include "media_audio.h"
#include "media_config.h"
// #include "custom_event.h"
#include "cvi_param.h"
#if CONFIG_PQTOOL_SUPPORT == 1
#include "cvi_ispd2.h"
#endif

#include "sys/app_sys.h"
#include "wifi/app_net.h"
#include "event_mgr/app_event.h"

#include <app_cx_faceai.h>
#include "app_cx_record.h"
#include "app_main.h"

#define TAG "app"

pthread_mutex_t sensor_toggle_lock;

int main(void)
{
    int ret = 0;

    board_yoc_init();

    //media video sys init, //Need move to board
    MEDIA_VIDEO_SysInit();

    //media_audio
    //Need move to board
    MEDIA_AUDIO_Init();

    #if CONFIG_PQTOOL_SUPPORT == 1
    usleep(1000);
    isp_daemon2_init(8888);
    #endif

    /* load weiht for fast startup */
    extern int app_cx_hw_init();
    ret = app_cx_hw_init();
    aos_assert((ret == 0));

    extern int app_cx_svr_init();
    ret = app_cx_svr_init();
    aos_assert((ret == 0));

    app_sys_init();

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#else
	MediaVideoInit();
#endif

    /*  have to initialze the dev0 sensor first to make dev1 work,
        so workaround here is to let faceai start the dual sensor when init */
    ret = app_faceai_start();
    if (ret < 0) {
        LOGE(TAG, "faceai start failed - %d", ret);
    } else {
        /* block until return result */
        ret = app_faceai_verify(-1);
    }

    ret = aos_cli_init();
    if (ret == 0) {
        board_cli_init();
    }
#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

    aos_msleep(200);
    app_network_init();
    app_event_init();
}

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
#include "fota/app_fota.h"
#include "wifi/app_net.h"
#include "event_mgr/app_event.h"

#include "app_cx_record.h"
#include <app_cx_faceai.h>
#include "app_gui.h"
#include "app_main.h"
#include <drv/tick.h>
#include <csi_core.h>
#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
#include <bengine/bengine_dload.h>
#include <bengine/bestat.h>
#endif

#define TAG "app"

extern void do_pf_test();
pthread_mutex_t sensor_toggle_lock;

int main(void)
{
    int ret = 0;
    board_yoc_init();
    LOGI(TAG, "=====cpu work on %s mode!=====", __get_CPU_WORK_MODE() == 1 ? "Supervisor" : "Machine");
#ifdef CONFIF_NETWORK_PRE
    ret = aos_kv_init("kv");
    if (ret != 0) {
        LOGE(TAG, "aos_kv_init failed(%d)", ret);
        return -1;
    } else {
        LOGD(TAG, "aos_kv_init succeed.");
    }
    app_network_init();
#endif


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

#ifdef CONFIG_FACEAI_IMAGE_DUMP
    app_sys_init();
    extern void set_faceai_dump_switch(int sw);
    set_faceai_dump_switch(1);
#endif
    pthread_mutex_init(&sensor_toggle_lock, 0);
    /* first face recognize */
    SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#else
    MediaVideoInit();
#endif

    ret = app_faceai_start();
    uint32_t time_recog = 0;
    if (ret < 0) {
        LOGE(TAG, "faceai start failed - %d", ret);
    } else {
        /* block until return result */
        ret = app_faceai_verify(-1);
        // time_recog = csi_tick_get_ms();
        time_recog = aos_now_ms();
        LOGE(TAG, "FaceAI app cx_face_verify result: %d", ret);
    }
    printf("====time = %u\n", time_recog);
#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
    extern void bengine_dload_disp();
    bengine_dload_disp();
#endif

#if defined(CONFIG_IR_GUI_EN) && CONFIG_IR_GUI_EN
    app_preview_init(PREVIEW_MODE_RGB);
    GUI_Init();

    char show_test[128];
    if(ret >= 0) {
        snprintf(show_test, sizeof(show_test), "Recognition Success: %dms", time_recog);
        GUI_Show_Text(show_test);
    } else {
        snprintf(show_test, sizeof(show_test), "Recognition Failed: %d", ret);
        GUI_Show_Text(show_test);
    }
    GUI_Show_Text(NULL);
#endif

#ifndef CONFIG_FACEAI_IMAGE_DUMP
    app_sys_init();
#endif

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRDeInit();
    MediaVideoRGBInit();
#endif

    SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
    /* first face recognize finish*/

    ret = aos_cli_init();
    if (ret == 0) {
        board_cli_init();
    }
#ifdef AOS_COMP_DEBUG
    aos_debug_init();
#endif

#ifndef CONFIF_NETWORK_PRE
    app_network_init();
#endif
    app_event_init();

    aos_msleep(3000);  // FIME: huashanpi evb board wifi problem
    app_fota_init();
#if defined(CONFIG_BENGINE_ENABLE) && CONFIG_BENGINE_ENABLE
    LOGI(TAG, "......before calling do_pf_test......");
    do_pf_test();
    LOGI(TAG, "......after calling do_pf_test......");
    aos_msleep(3000);
    bengine_dload_disp();
    LOGI(TAG, "......prepare load the post sections after 10s......");
    /* load the post sections after 10s */
    bengine_dload_post(10*1000);
    aos_msleep(50000);
    bengine_dload_disp();
#endif
}

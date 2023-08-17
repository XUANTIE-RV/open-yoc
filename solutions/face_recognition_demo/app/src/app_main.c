/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include "media_video.h"
#include "media_audio.h"
// #include "custom_event.h"
#include "cvi_param.h"
#include "wifi_if.h"
#include "ethernet_init.h"
#if CONFIG_PQTOOL_SUPPORT == 1
#include "cvi_ispd2.h"
#endif
#include <cx/init.h>
#include <cx/faceai/face_ai_service.h>
#include "app_main.h"
#include "media_config.h"

#define TAG "app"

extern int app_cx_svr_init();

cx_srv_hdl_t g_face_ai_handler;
int main(void)
{
	int ret = -1;
	board_yoc_init();

    //media video sys init, //Need move to board
    MEDIA_VIDEO_SysInit();

    //media_audio
    //Need move to board
    MEDIA_AUDIO_Init();

#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#else
	MediaVideoInit();
#endif

	#if CONFIG_PQTOOL_SUPPORT == 1
	usleep(1000);
	isp_daemon2_init(8888);
	#endif
	LOGI(TAG, "app start........\n");

    ret = app_cx_svr_init();
    aos_assert((ret == 0));


	while (1) {
		aos_msleep(3000);
	};
}

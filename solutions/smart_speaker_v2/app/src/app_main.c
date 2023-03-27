/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#include <stdio.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <sys_clk.h>
#include <aos/kv.h>
#include <alg_kws.h>

#include "sys/app_sys.h"
#include "aui_cloud/app_aui_cloud.h"
#include "player/app_player.h"
#include "voice/app_voice.h"
#include "wifi/app_net.h"
#include "key_msg/app_key_msg.h"
#include "event_mgr/app_event.h"
#include "display/app_disp.h"
#include "bt/app_bt.h"
#include "alarms/app_alarms.h"
#include "at_cmd/app_at_cmd.h"
#include "button/app_button.h"

#include "app_main.h"

#define TAG "main"

extern void cxx_system_init(void);
int main(int argc, char *argv[])
{
    long long enter_main_ms = aos_now_ms();
    cxx_system_init();
    board_yoc_init();

    printf("main %lldms\r\n", enter_main_ms);
    LOGI("main", "build time: %s,%s, clock=%uHz\r\n", __DATE__, __TIME__, soc_get_cur_cpu_freq());

    /* 文件系统 */
    app_sys_fs_init();

    /* 播放器 */
    LOGI("main", "audio init");
#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO
    board_audio_init();
#endif

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO
    app_speaker_init();

    app_player_init();
#endif

    LOGI("main", "event init");
    /* 应用事件 */
    app_sys_init();
    app_display_init();
    app_event_init();
    app_key_msg_init();
    app_cli_init();

    /* 网络蓝牙 */
    LOGI("main", "wifi init");
#if defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI
    board_wifi_init();
#endif

#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT
    board_bt_init();
#endif

    LOGI("main", "network init");
#if (defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI) \
 || (defined(CONFIG_BOARD_ETH) && CONFIG_BOARD_ETH)
    app_network_init();
#endif

#if defined(CONFIG_BT_BREDR) && (CONFIG_BT_BREDR == 1)
    app_bt_init();
#endif

    LOGI("main", "voice init");
    /* 语音交互 */
    alg_kws_init();
    app_mic_init();
    app_aui_cloud_init();

    /* 其他外设 */
#if defined(CONFIG_BOARD_BUTTON) && BOARD_BUTTON_NUM > 0
    app_button_init();
#endif

    //app_alrams_init();

#if defined(CONFIG_SMART_SPEAKER_AT) && CONFIG_SMART_SPEAKER_AT
    app_at_cmd_init();
#endif

    //app_linear_aec_init();

    // while(1) {
    //     aos_msleep(1000);
    //     LOGD(TAG, "loop");
    // }

    return 0;
}

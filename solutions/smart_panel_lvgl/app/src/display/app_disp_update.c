/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <ulog/ulog.h>

#include <smart_audio.h>
#include "app_net.h"
#include "voice/app_voice.h"

#include "app_disp.h"

#define TAG "DISP"

static int g_boot_display = 1;
void app_display_update(void)
{
    if (g_boot_display == 1) {
        app_display_show(DISP_SHOW_BOOT);
        g_boot_display = 0;
        return;
    }

    smtaudio_state_t state = smtaudio_get_state();

    if (wifi_is_pairing()) {
        app_display_show(DISP_SHOW_WIFI_PROV);
    } else if (!app_network_internet_is_connected()) {
        app_display_show(DISP_SHOW_WIFI_CONN_FAIL);
    } else if (app_mic_is_wakeup()) {
        app_display_show(DISP_SHOW_WAKEUP);
    } else if (state == SMTAUDIO_STATE_MUTE) {
        app_display_show(DISP_SHOW_MUTE);
    } else if (state == SMTAUDIO_STATE_PLAYING) {
        app_display_show(DISP_SHOW_PLAY);
    } else {
        app_display_show(DISP_SHOW_NOTHING);
    }
}

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include <stdio.h>

#include <yoc/uservice.h>
#include <yoc/eventid.h>

#include "app_init.h"
#include <pin_name.h>
#include <devices/devicelist.h>
#include <devices/rtl8723ds.h>
#include "app_main.h"
#include "app_init.h"
#include <pin.h>
#include <yoc/netmgr_service.h>
#include <player.h>
#include <alsa/mixer.h>

#include "audio/starting.mp3.c"
#include "audio/ok.mp3.c"
#include "audio/net_succ.mp3.c"
#include "audio/net_fail.mp3.c"
#include "audio/hello.mp3.c"
// #include "hello_ac2_raw.h"

#define TAG "ap-app"

extern void netmgr_service_init(utask_t *task);

//yv_t *yv;
/* network event callback */
void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    char url[128];
    if (event_id == EVENT_NETMGR_GOT_IP) {
        LOGD(TAG, "Net up");
        snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_net_succ, sizeof(local_audio_net_succ));
        _player(url);
    } else if (event_id == EVENT_NETMGR_NET_DISCON) {

        snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_net_fail, sizeof(local_audio_net_fail));
        _player(url);
        if ((int)param == NET_DISCON_REASON_DHCP_ERROR) {
            LOGD(TAG, "Net down");
            netmgr_reset(netmgr_get_handle("wifi"), 30);
        }
    } else {
        ;
    }

}

static netmgr_hdl_t _network_init()
{

    /* kv config check */
    aos_kv_setint("wifi_en", 1);

    netmgr_hdl_t netmgr_hdl = netmgr_dev_wifi_init();

    netmgr_service_init(NULL);
    if (netmgr_hdl)
        netmgr_start(netmgr_hdl);

    return netmgr_hdl;
}

static struct {
    aos_mixer_t      *mixer;
    aos_mixer_elem_t *elem;
} g_media;

static int mixer_init(void)
{
    int rc = 0;

    rc = aos_mixer_open(&g_media.mixer, 0);
    if (rc != 0) {
        printf("aos_mixer_open failed\n");
        return -1;
    }
    rc = aos_mixer_attach(g_media.mixer, "card0");
    if (rc != 0) {
        printf("aos_mixer_attach failed\n");
        return -1;
    }
    rc = aos_mixer_load(g_media.mixer);
    if (rc != 0) {
        printf("aos_mixer_load failed\n");
        return -1;
    }
    g_media.elem = aos_mixer_first_elem(g_media.mixer);

    if (g_media.elem == NULL) {
        rc = -1;
        printf("elem find NULL\n");
    }

    return 0;
}

#define KV_VOL_KEY "volumeK"
static int _set_kv_volume(int i8Volume)
{
    int rc = 0;

    rc = aos_kv_setint(KV_VOL_KEY, i8Volume);
    LOGI(TAG, "vol kv set %d, rc: %d", i8Volume, rc);
    return rc;
}

void app_set_volume(int vol)
{
    aos_mixer_selem_set_playback_volume_all(g_media.elem, vol);
    _set_kv_volume(vol);
}

int g_pcm_data[0x80000/4];
int g_pcm_len;

#if 0
static void voice_event(void *priv, voice_evt_id_t evt_id, void *data, int len)
{
    char *p = data;
    int data_len = len;

    if (evt_id == VOICE_ASR_EVT) {
        LOGD(TAG, "asr ok...\n");
        char url[128];
        snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_hello, sizeof(local_audio_hello));
        _player(url);
    } else if (evt_id == VOICE_DATA_EVT) {
        voice_data_t *vdata = (voice_data_t *)data;

        p = vdata->data;
        data_len = vdata->len;

        if (vdata->type == VOICE_MIC_DATA) {

            return;
        } else if (vdata->type == VOICE_REF_DATA) {

            return;
        } else if (vdata->type == VOICE_AEC_DATA) {

        }
    } else if (evt_id == VOICE_SILENCE_EVT) {

    }

}

static int app_voice_init(void)
{
    voice_pcm_param_t pcm_p;
    voice_param_t v_p;
    voice_t *v;

    v = voice_init(voice_event, NULL);

    pcm_p.access = 0;
    pcm_p.channles = 1;
    pcm_p.channles_sum = 1;
    pcm_p.rate = 16000;
    pcm_p.sample_bits = 16;
    pcm_p.pcm_name = "pcmC0";
    voice_add_mic(v, &pcm_p);
    pcm_p.pcm_name = "pcmC2";
    voice_add_ref(v, &pcm_p);

    v_p.cts_ms = 80;
    v_p.ipc_mode = 0;
    voice_config(v, &v_p);

    voice_start(v);

    return 0;
}
#endif

extern void speaker_init();
void main()
{
    extern int posix_init(void);
    extern void cxx_system_init(void);

    // must call this func when use c++
    posix_init();
    // must call this func when use c++
    cxx_system_init();

    board_yoc_init();
    player_init();
    speaker_init();

    mixer_init();

    //app_voice_init();

    app_set_volume(60);

    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };

    wifi_rtl8723ds_register(&pin);
    netmgr_get_dev(_network_init());

    extern void cli_reg_cmd_player(void);
    cli_reg_cmd_player();
    char url[128];
    snprintf(url, 128, "mem://addr=%u&size=%u", (uint32_t)&local_audio_starting, sizeof(local_audio_starting));
    _player(url);

    /* 系统事件订阅 */
    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);
}


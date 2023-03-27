/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#if defined(CONFIG_BOARD_AUDIO) && CONFIG_BOARD_AUDIO > 0

#include <stdio.h>
#include <time.h>
#include <aos/debug.h>

#include <drv_amp.h>

#include "app_player.h"
#include "event_mgr/app_event.h"

#include "app_main.h"

#define TAG "app_player"

#define MINI_VOLUME 20

#ifndef CONFIG_RESAMPLE_RATE
#define CONFIG_RESAMPLE_RATE 48000
#endif

static void pa_check_event(uint32_t event_id, const void *data, void *context);

static aos_mutex_t g_pa_lock;

/*************************************************
 * 本地音播放
 *************************************************/
static int _audio_play_(const char *name, int resume)
{
    char local_url[64];

    snprintf(local_url, sizeof(local_url), "file:///mnt/%s", name);

    return smtaudio_start(MEDIA_SYSTEM, local_url, 0, resume);
}

int local_audio_play(const char *name)
{
    return _audio_play_(name, 1);
}

int local_wakeup_audio_play(const char *name)
{
    return _audio_play_(name, 0);
}

/*************************************************
 * 播放器初始化
 *************************************************/
static void media_evt(int type, smtaudio_player_evtid_t evt_id)
{
    // LOGD(TAG, "media_evt type %d,evt_id %d", type, evt_id);
    if ((type == SMTAUDIO_ONLINE_MUSIC) || (type == SMTAUDIO_LOCAL_PLAY)) {
        switch (evt_id) {
            case SMTAUDIO_PLAYER_EVENT_START:
                app_event_update(type == SMTAUDIO_LOCAL_PLAY ? EVENT_MEDIA_SYSTEM_START : EVENT_MEDIA_START);
                break;

            case SMTAUDIO_PLAYER_EVENT_ERROR:
                app_event_update(type == SMTAUDIO_LOCAL_PLAY ? EVENT_MEDIA_SYSTEM_ERROR : EVENT_MEDIA_MUSIC_ERROR);
                if (SMTAUDIO_ONLINE_MUSIC == type) {
                    /* media_system doesn't need to play error audio */
                    LOGE(TAG, "SMTAUDIO_PLAYER_EVENT_ERROR");
                } else {
                    // app_tts_update_running(TTS_STATE_IDLE);
                }

                LOGI(TAG, "audio player exit %d", av_errno_get());
                break;
            case SMTAUDIO_PLAYER_EVENT_PAUSE:
            case SMTAUDIO_PLAYER_EVENT_STOP:
                app_event_update(type == SMTAUDIO_LOCAL_PLAY ? EVENT_MEDIA_SYSTEM_FINISH : EVENT_MEDIA_MUSIC_FINISH);
                if (SMTAUDIO_LOCAL_PLAY == type) {
                    // app_tts_update_running(TTS_STATE_IDLE);
                }

                LOGD(TAG, "audio player exit %d", SMTAUDIO_PLAYER_EVENT_STOP);
                break;
            case SMTAUDIO_PLAYER_EVENT_RESUME:
                // app_event_update(EVENT_MEDIA_START);
                LOGD(TAG, "audio player resumed %d", SMTAUDIO_PLAYER_EVENT_RESUME);
                break;
            case SMTAUDIO_PLAYER_EVENT_UNDER_RUN:
                LOGD(TAG, "audio player underrun");
                break;

            case SMTAUDIO_PLAYER_EVENT_OVER_RUN:
                LOGD(TAG, "audio player overrun");
                break;

            default:
                break;
        }
    }
}

int app_player_init(void)
{
    int                 eq_type    = board_eq_get_type();
    audio_vol_config_t *vol_config = board_audio_out_get_vol_config();

    aos_mutex_new(&g_pa_lock);

    smtaudio_init(media_evt);
    smtaudio_vol_config(vol_config);

    /* 根据EQ类型初始化播放器 */

    switch (eq_type) {
        case EQ_TYPE_NONE:
            smtaudio_register_local_play(MINI_VOLUME, NULL, 0, 1.0f, 0);
            smtaudio_register_online_music(MINI_VOLUME, NULL, 0, 1.0f, 0);
            break;
        case EQ_TYPE_HARDWARE:
            /* 设置默认参数 */
            board_eq_set_param(NULL, 0);
            board_drc_set_param(NULL, 0);

            smtaudio_register_local_play(MINI_VOLUME, NULL, 0, 1.0f, CONFIG_RESAMPLE_RATE);
            smtaudio_register_online_music(MINI_VOLUME, NULL, 0, 1.0f, CONFIG_RESAMPLE_RATE);
            break;
        case EQ_TYPE_SOFTWARE: {
            uint8_t *eqcfg      = NULL;
            size_t   eqcfg_size = 0;

            eqcfg = board_eq_get_param(&eqcfg_size);

            smtaudio_register_local_play(MINI_VOLUME, eqcfg, eqcfg_size, 1.0f, CONFIG_RESAMPLE_RATE);
            smtaudio_register_online_music(MINI_VOLUME, eqcfg, eqcfg_size, 1.0f, CONFIG_RESAMPLE_RATE);
        } break;
        default:
            aos_assert(0);
    }

    event_subscribe(EVENT_PA_CHECK, pa_check_event, NULL);
    event_publish_delay(EVENT_PA_CHECK, NULL, 1000);

    return 0;
}

/*************************************************
 * PA控制-延时关闭
 *************************************************/
static volatile long long g_pa_delay_mute = 0;

static void pa_check_event(uint32_t event_id, const void *data, void *context)
{
    // printf("pa check %d\n", g_pa_delay_mute);
    if (event_id == EVENT_PA_CHECK) {
        aos_mutex_lock(&g_pa_lock, AOS_WAIT_FOREVER);
        if (g_pa_delay_mute) {
#if 1
            long long now = aos_now_ms();
            if (now - g_pa_delay_mute > 3000) {
                LOGD(TAG, "PA delay mute");
                app_speaker_mute(1);
                g_pa_delay_mute = 0;
            }
#else
            LOGD(TAG, "PA delay mute");
            app_speaker_mute(1);
            g_pa_delay_mute = 0;
#endif
        }
        aos_mutex_unlock(&g_pa_lock);
    }
    event_publish_delay(EVENT_PA_CHECK, NULL, 1000);
}

void ao_event_hook(int ao_evt)
{
    if (!aos_mutex_is_valid(&g_pa_lock)) {
        return;
    }

    switch (ao_evt) {
        case 0: /* 结束播放 */
            /* 延时关闭 */
            aos_mutex_lock(&g_pa_lock, AOS_WAIT_FOREVER);
            if (g_pa_delay_mute == 0) {
                g_pa_delay_mute = aos_now_ms();
            }
            aos_mutex_unlock(&g_pa_lock);
            break;
        case 1: /* 开始播放,Codec初始化前*/
            LOGD(TAG, "PA mute");
            app_speaker_mute(1);
            break;
        case 2: /* 开始播放,Codec初始化后 */
            LOGD(TAG, "PA unmute");
            aos_mutex_lock(&g_pa_lock, AOS_WAIT_FOREVER);
            g_pa_delay_mute = 0;
            aos_mutex_unlock(&g_pa_lock);
            app_speaker_mute(0);
        default:;
    }
    return;
}

void app_speaker_init(void)
{
#if defined(CONFIG_BOARD_AUDIO_AMP) && CONFIG_BOARD_AUDIO_AMP
    board_audio_amplifier_onoff(0);
#else
    int pa_pin = board_audio_get_pa_mute_pin();

    LOGD(TAG, "PA init pinid %d", pa_pin);

    if (pa_pin >= 0 ) {
        amplifier_init(AMP_TYPE_GPIO, pa_pin, -1, AMP_MODE_DEF);
        amplifier_onoff(1);
    }
#endif
}

void app_speaker_mute(int mute)
{
    if (smtaudio_get_state() == SMTAUDIO_STATE_MUTE)
        mute = 1;

#if defined(CONFIG_BOARD_AUDIO_AMP) && CONFIG_BOARD_AUDIO_AMP
    board_audio_amplifier_onoff(mute ? 0 : 1);
#else
    amplifier_onoff(mute ? 0 : 1);
#endif
}
#endif /*CONFIG_BOARD_AUDIO*/

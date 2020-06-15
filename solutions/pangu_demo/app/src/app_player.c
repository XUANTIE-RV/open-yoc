/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/kv.h>
#include <media.h>
#include <soc.h>
#include "app_main.h"
#include "audio/audio_res.h"

static int ai_kv_get_vol(int type)
{
    char key[8];
    snprintf(key, sizeof(key), "vol%d", type);

    int value;
    int ret = aos_kv_getint(key, &value);
    return ret == 0?value:60;
}

static int ai_kv_set_vol(int type, int vol)
{
    char key[8];
    snprintf(key, sizeof(key), "vol%d", type);
    return aos_kv_setint(key, vol);
}

static int g_mute_flag = 0;
static int g_lpm_flag = 0;

#define LPM_RETURN() \
    if (g_lpm_flag) { \
        return; \
    }

#define LPM_RETURN_RET(ret) \
    if (g_lpm_flag) { \
        return ret; \
    }

/**
 * 从kv中读取初始化播放器音量
*/
void app_player_init(void)
{
    int vol;
    vol = ai_kv_get_vol(MEDIA_MUSIC);
    aui_player_vol_set(MEDIA_MUSIC, vol);

    vol = ai_kv_get_vol(MEDIA_SYSTEM);
    aui_player_vol_set(MEDIA_SYSTEM, vol);
}

/**
 * 音量变化时调用该函数进行保存
*/
static void auikv_player_vol_save(void)
{
    int vol;

    vol = aui_player_vol_get(MEDIA_MUSIC);
    ai_kv_set_vol(MEDIA_MUSIC, vol);

    vol = aui_player_vol_get(MEDIA_SYSTEM);
    ai_kv_set_vol(MEDIA_SYSTEM, vol);
}

static void app_volume_adjust(int vol)
{
    if (vol < 0) {
        aui_player_vol_adjust(MEDIA_ALL, vol);
    } else {
        int vol_m, vol_s;

        vol_m = aui_player_vol_get(MEDIA_MUSIC);
        vol_s = aui_player_vol_get(MEDIA_SYSTEM);

        if (vol_m < vol_s) {
            aui_player_vol_adjust(MEDIA_MUSIC, vol);
        } else {
            aui_player_vol_adjust(MEDIA_ALL, vol);
        }
    }
}

void app_volume_inc(int notify)
{
    LPM_RETURN();
    app_volume_adjust(10);
    auikv_player_vol_save();

    if(aui_player_get_state(MEDIA_MUSIC) == AUI_PLAYER_PLAYING) {
        aui_player_resume_music();
    } else {
        if (notify) {
            local_audio_play(LOCAL_AUDIO_OK);
        }
    }
}

void app_volume_dec(int notify)
{
    LPM_RETURN();
    app_volume_adjust(-10);
    auikv_player_vol_save();

    if(aui_player_get_state(MEDIA_MUSIC) == AUI_PLAYER_PLAYING) {
        aui_player_resume_music();
    } else {
        if (notify) {
            local_audio_play(LOCAL_AUDIO_OK);
        }
    }
}

void app_volume_set(int vol, int notify)
{
    LPM_RETURN();
    aui_player_vol_set(MEDIA_ALL, vol);
    auikv_player_vol_save();

    if(aui_player_get_state(MEDIA_MUSIC) == AUI_PLAYER_PLAYING) {
        aui_player_resume_music();
    } else {
        if (notify) {
            local_audio_play(LOCAL_AUDIO_OK);
        }
    }
}

void app_volume_mute(void)
{
    LPM_RETURN();
    if (g_mute_flag == 0) {
        aui_player_mute(MEDIA_ALL);
        g_mute_flag = 1;
    } else {
        if(aui_player_get_state(MEDIA_SYSTEM) == AUI_PLAYER_PLAYING) {
            aui_player_resume(MEDIA_SYSTEM);
        } else if (aui_player_get_state(MEDIA_MUSIC) == AUI_PLAYER_PLAYING) {
            aui_player_resume(MEDIA_MUSIC);
        }
        g_mute_flag = 0;
    }
}

int app_player_get_mute_state(void)
{
    return g_mute_flag;
}

void app_player_reverse(void)
{
    LPM_RETURN();

    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_SYSTEM);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_SYSTEM);
    }

    state = aui_player_get_state(MEDIA_MUSIC);

    switch (state)
    {
        case AUI_PLAYER_PLAYING:
            aui_player_pause(MEDIA_MUSIC);
            break;
        case AUI_PLAYER_PAUSED:
            aui_player_resume(MEDIA_MUSIC);
            break;
        default:
            break;
    }
}

void app_player_pause(void)
{
    LPM_RETURN();

    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_SYSTEM);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_SYSTEM);
    }

    state = aui_player_get_state(MEDIA_MUSIC);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_pause(MEDIA_MUSIC);
    }
}

void app_player_resume(void)
{
    LPM_RETURN();

    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_MUSIC);

    if (state == AUI_PLAYER_PAUSED) {
        aui_player_resume(MEDIA_MUSIC);
    }
}

void app_player_stop(void)
{
    LPM_RETURN();

    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_MUSIC);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_MUSIC);
    }

    state = aui_player_get_state(MEDIA_SYSTEM);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_SYSTEM);
    }
}

int app_player_play(int type, const char *url, int resume)
{
    LPM_RETURN_RET(-1);

    aui_player_play(type, url, resume);

    return 0;
}

int app_player_is_busy(void)
{
    aui_player_state_t play_st;
    play_st = aui_player_get_state(MEDIA_MUSIC);
    if (play_st == AUI_PLAYER_PLAYING) {
        return 1;
    }

    play_st = aui_player_get_state(MEDIA_SYSTEM);
    if (play_st == AUI_PLAYER_PLAYING) {
        return 1;
    }

    return 0;
}

int app_player_lpm(int state)
{
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();
    aos_kernel_sched_suspend();
    g_lpm_flag = state;
    aos_kernel_sched_resume();
    csi_irq_restore(irq_flag);

    return 0;
}

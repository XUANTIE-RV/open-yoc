/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include <media.h>
#include <avutil/misc.h>
#if defined(APP_EQ_EN) && APP_EQ_EN
#include <aef/eq_all.h>
#endif

#define TAG "appplayer"

static int g_mute_flag = 0;

static int ai_kv_get_vol(int type)
{
    char key[8];
    snprintf(key, sizeof(key), "vol%d", type);

    int value;
    int ret = aos_kv_getint(key, &value);
    return ret == 0?value:30;
}

static int ai_kv_set_vol(int type, int vol)
{
    char key[8];
    snprintf(key, sizeof(key), "vol%d", type);
    return aos_kv_setint(key, vol);
}

static void media_evt(int type, aui_player_evtid_t evt_id)
{
    //LOGD(TAG, "media_evt type %d,evt_id %d", type, evt_id);

    switch (evt_id) {
        case AUI_PLAYER_EVENT_START:
            event_publish(EVENT_MEDIA_START, NULL);
            break;

        case AUI_PLAYER_EVENT_ERROR:
            event_publish(type == MEDIA_SYSTEM ? EVENT_MEDIA_SYSTEM_ERROR : EVENT_MEDIA_MUSIC_ERROR, NULL);
            local_audio_play(LOCAL_AUDIO_PLAY_ERR);
            LOGD(TAG, "audio player exit %d", AUI_PLAYER_EVENT_ERROR);
            break;

        case AUI_PLAYER_EVENT_FINISH:
            event_publish(type == MEDIA_SYSTEM ? EVENT_MEDIA_SYSTEM_FINISH : EVENT_MEDIA_MUSIC_FINISH, NULL);
            LOGD(TAG, "audio player exit %d", AUI_PLAYER_EVENT_FINISH);
            break;
            
        default:
            break;
    }

    if (!(type == MEDIA_SYSTEM && evt_id == AUI_PLAYER_EVENT_START)) {
        app_status_update();
    }
}

int media_dec_key_cb(const void *in, size_t ilen, void *out, size_t *olen)
{
    int rc;

    CHECK_PARAM(in && ilen >= 3 && out && olen && *olen >= 16, -1);


    LOGD(TAG, "mp4 major brand is %s", in);
    int key_num = ((char *)in)[2] - '0';
    char cenc_decrypt_key[33] = {0};        // "6ABED02448B8FFD2224FF54619935526"

    if (strncmp(in, "KS", 2) == 0 && (key_num >= 1 && key_num <= 5)) {
        if (aos_kv_getstring(in, cenc_decrypt_key, 33) > 0) {
            rc = bytes_from_hex(cenc_decrypt_key , out, *olen);
            CHECK_RET_TAG_WITH_RET(rc == 0, -1);
            *olen = 16;
            return 0;
        }
    }
    
    LOGW(TAG, "major brand %s get key failed", in);
    return -1;
}

static int app_media_init(utask_t *task)
{
    int ret = -1;

    ret = aui_player_init(task, media_evt);

    if (ret == 0) {
        aui_player_config_t config = {0};

        config.resample_rate = 48000;
        //config.web_cache_size = 160 * 1024;  /* 修改网络传输的缓冲默认80K */
        aui_player_config(&config);
    }

    return ret;
}
/**
 * 从kv中读取初始化播放器音量
*/
void app_player_init(void)
{
    int vol;

    utask_t *task_media = utask_new("task_media", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    app_media_init(task_media);

    aui_player_key_config(media_dec_key_cb);

    vol = ai_kv_get_vol(MEDIA_MUSIC);
    aui_player_vol_set(MEDIA_MUSIC, vol);

    vol = ai_kv_get_vol(MEDIA_SYSTEM);
    aui_player_vol_set(MEDIA_SYSTEM, vol);

#if (defined(BOARD_MIT_V2) || defined(BOARD_MIT_V3))
    aui_player_set_minvol(MEDIA_ALL, 0);
#else
    aui_player_set_minvol(MEDIA_ALL, 20);
#endif

#if defined(APP_EQ_EN) && APP_EQ_EN
    /* EQ 示例 */
    eqx_register_silan();
/*
    //设置EQ默认配置
    eqfp_t bqfparams[4] = {
        {1, EQF_TYPE_PEAK,  -20.0, 1.0, 100},
        {1, EQF_TYPE_PEAK, -20.0, 1.0, 200 },
        {1, EQF_TYPE_PEAK, -20.0, 1.0, 400 },
        {1, EQF_TYPE_PEAK, 3.0, 1.0, 1000 }
    };
    aui_player_eq_config(bqfparams, 4);
*/
#endif

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
    g_mute_flag = 0;

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
    g_mute_flag = 0;

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
    g_mute_flag = 0;

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

    app_status_update();
}

void app_player_pause(void)
{
    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_SYSTEM);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_SYSTEM);
    }

    state = aui_player_get_state(MEDIA_MUSIC);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_pause(MEDIA_MUSIC);
    }

    app_status_update();
}

void app_player_resume(void)
{
    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_MUSIC);

    if (state == AUI_PLAYER_PAUSED) {
        aui_player_resume(MEDIA_MUSIC);
        app_status_update();
    }
}

void app_player_stop(void)
{
    aui_player_state_t state;
    state = aui_player_get_state(MEDIA_MUSIC);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_MUSIC);
    }

    state = aui_player_get_state(MEDIA_SYSTEM);

    if (state == AUI_PLAYER_PLAYING) {
        aui_player_stop(MEDIA_SYSTEM);
    }

    app_status_update();
}

int app_player_get_status()
{
    aui_player_state_t play_st;
    play_st = aui_player_get_state(MEDIA_MUSIC);

    return (play_st == AUI_PLAYER_PLAYING);
}

int app_player_is_busy()
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
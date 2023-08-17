/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/kv.h>

#include <av/avutil/misc.h>
#include <av/player.h>
#include <av/avformat/avparser_all.h>

#include "ulog/ulog.h"
#include "smart_audio.h"

#define TAG "LOCAL_PLAYER_LITE"

// static utask_t *aui_task_media = NULL;
// static aui_player_config_t media_config;
static audio_vol_config_t *g_vol_config = NULL;

static int local_play_init(void);
static int local_play_deinit(void);
static int local_play_start(const char *url, uint64_t seek_time, int resume);
static int local_play_pause(void);
static int local_play_stop(void);
static int local_play_resume(void);
static int local_play_vol_get(void);
static int local_play_vol_set(int vol);
static int local_play_vol_up(int vol);
static int local_play_vol_down(int vol);

static player_t *g_player = NULL;

static int local_player_init()
{
    static int inited = 0;

    if (!inited)
    {
        avparser_register_all();
        stream_register_all();
        demux_register_all();
        ad_register_all();
        ao_register_all();
        inited = 1;
    }

    return inited ? 0 : -1;
}

static smtaudio_ops_node_t ctrl_local_play = {
    .name = "simple_local_play",
    .url  = NULL,
    .prio = 1,
    .id = SMTAUDIO_LOCAL_PLAY,
    .status = SMTAUDIO_STATE_STOP,
    .init = local_play_init,
    .deinit = local_play_deinit,
    .start = local_play_start,
    .pause = local_play_pause,
    .stop = local_play_stop,
    .resume = local_play_resume,
    .vol_get = local_play_vol_get,
    .vol_set = local_play_vol_set,
    .vol_up = local_play_vol_up,
    .vol_down = local_play_vol_down,
};

static void _player_event(void *player, uint8_t type, const void *data, uint32_t len)
{
    LOGD(TAG, "=====%s, %d, type = %d", __FUNCTION__, __LINE__, type);

    switch (type)
    {
    case PLAYER_EVENT_ERROR:
        player_stop((player_t *)player);

        break;

    case PLAYER_EVENT_START:
    {
        media_info_t minfo;
        memset(&minfo, 0, sizeof(media_info_t));
        player_get_media_info((player_t *)player, &minfo);

        if (ctrl_local_play.callback)
        {
            ctrl_local_play.callback(SMTAUDIO_LOCAL_PLAY, SMTAUDIO_PLAYER_EVENT_START);
        }
        break;
    }

    case PLAYER_EVENT_FINISH:
        player_stop((player_t *)player);
        // aos_event_set(&evt_player, EVT_PLY_FINISH, AOS_EVENT_OR);
        break;

    default:
        break;
    }

    if (type == PLAYER_EVENT_FINISH || type == PLAYER_EVENT_ERROR)
    {
        // if (recording_paused) {
        //  recording_paused = 0;
        // warning "fix me"
        // mvoice_process_resume();
        //  }

        // set_unplaying();
    }
}

static int media_init(uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample)
{
    int ret = 0;
    ply_conf_t ply_cnf;

#if defined(CONFIG_AV_DBUS_SUPPORT) && CONFIG_AV_DBUS_SUPPORT
    return 0;
#endif
    local_player_init();

    ret = player_conf_init(&ply_cnf);
    if (ret)
    {
        LOGE(TAG, "Player conf init failed %d", ret);
        return ret;
    }
    ply_cnf.event_cb = _player_event;
    ply_cnf.aef_conf = aef_conf;
    ply_cnf.aef_conf_size = aef_conf_size;
    ply_cnf.speed = speed;
    ply_cnf.resample_rate = resample;

    g_player = player_new(&ply_cnf);
    if (!g_player)
    {
        LOGE(TAG, "New player create failed");
        return -EIO;
    }
    return 0;
}

static int local_play_init(void)
{
    return 0;
}

static int local_play_deinit(void)
{
    if (g_player)
    {
        return player_free(g_player);
    }
    return 0;
}

static int local_play_start(const char *url, uint64_t seek_time, int resume)
{
    // warning "seek resume"
    return player_play(g_player, url, 0);
}

static int local_play_pause(void)
{
    int ret = 0;
    ctrl_local_play.status = SMTAUDIO_STATE_PAUSE;

    ret = player_pause(g_player);
    if (ctrl_local_play.callback)
    {
        ctrl_local_play.callback(SMTAUDIO_LOCAL_PLAY, SMTAUDIO_PLAYER_EVENT_PAUSE);
    }

    return ret;
}

static int local_play_stop(void)
{
    int ret = 0;
    ctrl_local_play.status = SMTAUDIO_STATE_STOP;
    ret = player_stop(g_player);
    if (ctrl_local_play.callback)
    {
        ctrl_local_play.callback(SMTAUDIO_LOCAL_PLAY, SMTAUDIO_PLAYER_EVENT_STOP);
    }
    return ret;
}

static int local_play_resume(void)
{
    return player_resume(g_player);
}

static int local_play_vol_get(void)
{
    uint8_t vol = 0;
    player_get_vol(g_player, &vol);
    return vol;
}

static int local_play_vol_set(int vol)
{
    int ret;
    player_set_vol(g_player, vol);
    ret = aos_kv_setint(VOLUME_SAVE_KV_NAME, vol);
    if (0 != ret)
    {
        LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    }
    return ret;
}

static int local_play_vol_up(int vol)
{
    int ret;
    int cur_vol = local_play_vol_get();
    ret = player_set_vol(g_player, cur_vol + vol);
    if (0 != ret)
    {
        LOGE(TAG, "set kv volume failed! ret[%d]", ret);
        return ret;
    }

    ret = aos_kv_setint(VOLUME_SAVE_KV_NAME, cur_vol + vol);
    if (0 != ret)
    {
        LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    }

    return ret;
}

static int local_play_vol_down(int vol)
{
    int ret;

    int cur_vol = local_play_vol_get();
    ret = player_set_vol(g_player, cur_vol - vol);
    if (0 != ret)
    {
        LOGE(TAG, "set kv volume failed! ret[%d]", ret);
        return ret;
    }
    ret = aos_kv_setint(VOLUME_SAVE_KV_NAME, cur_vol - vol);
    if (0 != ret)
    {
        LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    }
    return ret;
}

int8_t smtaudio_vol_lite_config(audio_vol_config_t *vol_config)
{
    g_vol_config = vol_config;
    return 0;
}

int8_t smtaudio_register_local_play_lite(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed,
                                         int resample)
{
    int8_t ret;
    int vol;

    ret = smtaudio_ops_register(&ctrl_local_play);
    if (ret)
    {
        LOGE(TAG, "Ops register failed %d", ret);
        return ret;
    }

    ret = ctrl_local_play.init();
    if (ret)
    {
        LOGE(TAG, "ctrl local player init failed %d", ret);
        return ret;
    }

    ret = media_init(aef_conf, aef_conf_size, speed, resample);
    if (ret)
    {
        LOGE(TAG, "ctrl local media init failed %d", ret);
        return ret;
    }

    // warning "set min vol"
    // aui_player_set_minvol(MEDIA_ALL, (min_vol <= 0) ? 20 : min_vol);
    aos_kv_getint(VOLUME_SAVE_KV_NAME, &vol);
    ctrl_local_play.vol_set(((vol == 0) ? 20 : vol));

    return ret;
}
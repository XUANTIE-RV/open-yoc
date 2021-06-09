/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <smart_audio.h>
#include <avutil/misc.h>
#include <csi_core.h>

#define TAG "smart_audio"

typedef int8_t state_func_t();
static int8_t  do_state_play();
static int8_t  do_state_pause();
static int8_t  do_state_zero_volume();
static int8_t  do_state_mute();
static int8_t  do_state_stop();

static state_func_t *const func_table[SMTAUDIO_STATE_NUM] = {
    do_state_play, do_state_pause, do_state_zero_volume, do_state_mute, do_state_stop};
static aui_player_config_t media_config;

typedef struct smtaudio_type {
    utask_t *        task_media;
    smtaudio_state_t cur_state;
    smtaudio_state_t last_state;
    uint8_t          min_vol;
    uint8_t          lpm_flag; // 0: not in lpm state, 1: in lpm state
    media_evt_t      local_cb;
    aos_event_t      event_media_state;
    uint8_t          tts_resume;
    int8_t           cur_vol;
} smtaudio_type_t;

static smtaudio_type_t  smtaudio_ctx;
static smtaudio_state_t ori_state = SMTAUDIO_STATE_STOP;
#define EVENT_SYSTEM_FINISHED (1 << 0)
#define EVENT_MUSIC_STOPPED   (1 << 1) /* 停止队列中等待的MUSIC */
#define VOL_KEY               "volume"

#define LPM_RETURN()             \
    if (smtaudio_ctx.lpm_flag) { \
        return;                  \
    }
#define LPM_RETURN_RET(ret)      \
    if (smtaudio_ctx.lpm_flag) { \
        return ret;              \
    }

/***** internal APIs *****/
static void _aui_evt_cb_(int type, aui_player_evtid_t evt_id)
{
    switch (evt_id) {
    case AUI_PLAYER_EVENT_START:
        break;
    case AUI_PLAYER_EVENT_ERROR:
    case AUI_PLAYER_EVENT_FINISH:
        if (MEDIA_SYSTEM == type) {
            aos_event_set(&smtaudio_ctx.event_media_state, EVENT_SYSTEM_FINISHED, AOS_EVENT_OR);
            event_publish(EVENT_SYSTEM_FINISHED, NULL);
        } else if (MEDIA_MUSIC == type) {
            smtaudio_ctx.cur_state = SMTAUDIO_STATE_STOP;
        }
        break;
    default:
        break;
    }

    if (NULL != smtaudio_ctx.local_cb) {
        smtaudio_ctx.local_cb(type, evt_id);
    }
}

static void _smtaudio_evt_cb(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
    case EVENT_SYSTEM_FINISHED:
        if ((AUI_PLAYER_PAUSED == aui_player_get_state(MEDIA_MUSIC)) && smtaudio_ctx.tts_resume &&
            (SMTAUDIO_STATE_PLAYING == smtaudio_ctx.cur_state)) {
            /* 确认当前状态是被打断且可恢复状态 */
            aui_player_resume(MEDIA_MUSIC);
        }
        break;
    default:;
    }
}

/**
 * 音量变化时调用该函数将音量更新到kv
*/
static int kv_set_vol(void)
{
    int vol_music = 0, vol_sys = 0, ret = 0;

    vol_music = aui_player_vol_get(MEDIA_MUSIC);
    vol_sys   = aui_player_vol_get(MEDIA_SYSTEM);
    if (vol_music != vol_sys) {
        /* music and system should have same volumn */
        LOGW(TAG, "music volume(%d) is different with system volume(%d)", vol_music, vol_sys);
    }

    ret = aos_kv_setint(VOL_KEY, vol_music);
    if (0 != ret) {
        LOGE(TAG, "set kv volume failed! ret[%d]", ret);
    }
    return vol_music;
}

static int kv_get_vol(int type)
{
    int value;
    int ret = aos_kv_getint(VOL_KEY, &value);
    return ret == 0 ? value : 30;
}

static int media_dec_key_cb(const void *in, size_t ilen, void *out, size_t *olen)
{
    int rc;

    CHECK_PARAM(in && ilen >= 3 && out && olen && *olen >= 16, -1);

    LOGD(TAG, "mp4 major brand is %s", in);
    char cenc_decrypt_key[33] = {0}; // "6ABED02448B8FFD2224FF54619935526"

    if (aos_kv_getstring("mp4_key", cenc_decrypt_key, 33) > 0) {
        rc = bytes_from_hex(cenc_decrypt_key, out, *olen);
        CHECK_RET_TAG_WITH_RET(rc == 0, -1);
        *olen = 16;
        return 0;
    }

    LOGW(TAG, "major brand %s get key failed", in);
    return -1;
}

static int media_init(media_evt_t media_evt_cb, uint8_t *aef_conf, size_t aef_conf_size,
                      float speed, int resample)
{
    int ret = -1;

    ret                   = aui_player_init(smtaudio_ctx.task_media, _aui_evt_cb_);
    smtaudio_ctx.local_cb = media_evt_cb;

    if (ret == 0) {
        aui_player_config_init(&media_config);

        media_config.web_cache_size      = 256 * 1024;
        media_config.web_start_threshold = 30;
        media_config.resample_rate       = resample;
        if (aef_conf && aef_conf_size) {
            media_config.aef_conf      = aef_conf;
            media_config.aef_conf_size = aef_conf_size;
        }

        if (speed >= 0.5f && speed <= 2.0f) {
            media_config.speed = speed;
        } else {
            media_config.speed = 1;
        }

        aui_player_config(&media_config);
    }

    event_subscribe(EVENT_SYSTEM_FINISHED, _smtaudio_evt_cb, NULL);

    return ret;
}

static int volume_set(int vol)
{
    aui_player_vol_set(MEDIA_ALL, vol);
    smtaudio_ctx.cur_vol = vol;
    return kv_set_vol();
}

static void switch_state(smtaudio_state_t old_state, smtaudio_state_t new_state)
{
    if (old_state == new_state) {
        /* don't need to change state */
        return;
    }

    LOGD(TAG, "old state[%d] --> new state[%d]", old_state, new_state);

    smtaudio_ctx.last_state = old_state;
    smtaudio_ctx.cur_state  = new_state;
    func_table[smtaudio_ctx.cur_state]();
}

static int8_t do_state_play()
{
    if (AUI_PLAYER_PLAYING != aui_player_get_state(MEDIA_SYSTEM)) {
        /* 如果系统声音正在播放，系统声音播放结束后会恢复音乐 */
        aui_player_resume(MEDIA_MUSIC);
    }
    return 0;
}

static int8_t do_state_pause()
{
    aui_player_state_t play_state;
    play_state = aui_player_get_state(MEDIA_MUSIC);
    if (AUI_PLAYER_PLAYING == play_state) {
        aui_player_pause(MEDIA_MUSIC);
    }

    return 0;
}

static int8_t do_state_zero_volume()
{
    /* check current volume, which should be zero */
    int vol = aui_player_vol_get(MEDIA_MUSIC);
    if (vol != 0) {
        LOGE(TAG, "current volume[%d] is not zero!", vol);
        return -1;
    }
    aui_player_state_t play_state;
    play_state = aui_player_get_state(MEDIA_MUSIC);
    if (AUI_PLAYER_PLAYING == play_state) {
        aui_player_pause(MEDIA_MUSIC);
    }

    return 0;
}

static int8_t do_state_mute()
{
    int ret = 0;
    ret     = aui_player_pause(MEDIA_MUSIC);
    ret     = aui_player_stop(MEDIA_SYSTEM);

    return ret;
}

static int8_t do_state_stop()
{
    /* do nothing */
    return 0;
}

/****************************************/
/*          external APIs               */
/****************************************/
/**
 * 从kv中读取初始化播放器音量
*/
int8_t smtaudio_init(size_t stack_size, media_evt_t media_evt_cb, uint8_t min_vol,
                     uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);
    int ret = 0;
    int vol;

    if (NULL != smtaudio_ctx.task_media) {
        LOGW(TAG, "media task is already inited!");
        return 0;
    }

    memset(&smtaudio_ctx, 0, sizeof(smtaudio_type_t));
    smtaudio_ctx.task_media =
        utask_new("task_media", stack_size, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    if (NULL == smtaudio_ctx.task_media) {
        LOGE(TAG, "media task created failed!");
        return -EINVAL;
    }
    media_init(media_evt_cb, aef_conf, aef_conf_size, speed, resample);
    aos_event_new(&smtaudio_ctx.event_media_state, 0);

    aui_player_key_config(media_dec_key_cb);

    vol = kv_get_vol(MEDIA_ALL);
    smtaudio_vol_set(vol);

    smtaudio_ctx.min_vol = (min_vol <= 0) ? 20 : min_vol;
    aui_player_set_minvol(MEDIA_ALL, 0);

    smtaudio_ctx.cur_state = smtaudio_ctx.last_state = SMTAUDIO_STATE_STOP;

    return ret;
}

int8_t smtaudio_get_config(aui_player_config_t *config)
{
    aos_check_return_einval(config);

    *config = media_config;
    return 0;
}

int8_t smtaudio_set_config(aui_player_config_t *config)
{
    aos_check_return_einval(config);

    int ret = aui_player_config(config);
    if (ret == 0) {
        media_config = *config;
    }

    return ret == 0 ? 0 : -1;
}

int8_t smtaudio_vol_up(int16_t vol)
{
    int cur_vol = 0;
    int ret     = 0;

    cur_vol = aui_player_vol_get(MEDIA_MUSIC);
    ret     = smtaudio_vol_set(cur_vol + vol);
    return ret;
}

int8_t smtaudio_vol_down(int16_t vol)
{
    int cur_vol = 0;
    int ret     = 0;

    cur_vol = aui_player_vol_get(MEDIA_MUSIC);
    ret     = smtaudio_vol_set(cur_vol - vol);
    return ret;
}

int8_t smtaudio_vol_set(int16_t set_vol)
{
    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    int ret = 0;
    int vol;
    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_STATE_PLAYING:
    case SMTAUDIO_STATE_PAUSE:
    case SMTAUDIO_STATE_STOP:
        vol = volume_set(set_vol);
        if (0 == vol) {
            /*
            * 考虑如下状态切换：
            * PLAYING-(音量设为0)->ZERO_VOLUME-(按下禁麦键)->MUTE-(再次按下禁麦键)->ZERO_VOLUME-(增加音量)->PLAYING
            * PAUSE-(音量设为0)->ZERO_VOLUME-(按下禁麦键)->MUTE-(再次按下禁麦键)->ZERO_VOLUME-(增加音量)->PAUSE
            * STOP-(音量设为0)->ZERO_VOLUME-(按下禁麦键)->MUTE-(再次按下禁麦键)->ZERO_VOLUME-(增加音量)->STOP
            * 所以需要记录最原始状态
            */
            ori_state = smtaudio_ctx.cur_state;
            switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_ZERO_VOLUME);
        }
        break;
    case SMTAUDIO_STATE_ZERO_VOLUME:
        vol = volume_set(set_vol);
        if (vol > 0) {
            switch_state(SMTAUDIO_STATE_ZERO_VOLUME, smtaudio_ctx.last_state);
        }
        break;
    case SMTAUDIO_STATE_MUTE:
        /* do nothing */
        break;
    default:
        break;
    }
    return ret;
}

int8_t smtaudio_pause()
{
    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    int ret = 0;
    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_STATE_PLAYING:
    case SMTAUDIO_STATE_ZERO_VOLUME:
        switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_PAUSE);
        break;
    default:
        break;
    }
    return ret;
}

int8_t smtaudio_mute()
{
    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    int ret = 0;
    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_STATE_PLAYING:
    case SMTAUDIO_STATE_ZERO_VOLUME:
    case SMTAUDIO_STATE_PAUSE:
    case SMTAUDIO_STATE_STOP:
        switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_MUTE);
        break;
    case SMTAUDIO_STATE_MUTE:
        if (SMTAUDIO_STATE_ZERO_VOLUME == smtaudio_ctx.last_state) {
            /*
             * 参考smtaudio_vol_set函数内的注释
             * 设置ZERO_VOLUME退出后应该回到的状态
             */
            switch_state(ori_state, smtaudio_ctx.last_state);
        } else {
            switch_state(SMTAUDIO_STATE_MUTE, smtaudio_ctx.last_state);
        }
        break;
    default:
        break;
    }
    return ret;
}

int8_t smtaudio_resume()
{
    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    int ret = 0;
    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_STATE_PAUSE:
        switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_PLAYING);
        break;
    case SMTAUDIO_STATE_ZERO_VOLUME:
        if (AUI_PLAYER_PAUSED == aui_player_get_state(MEDIA_MUSIC)) {
            /* 零音量退出后，应该回到播放状态 */
            smtaudio_ctx.last_state = SMTAUDIO_STATE_PLAYING;
        }
        break;
    case SMTAUDIO_STATE_PLAYING:
    case SMTAUDIO_STATE_MUTE:
    case SMTAUDIO_STATE_STOP:
        /* do nothing */
        break;
    default:
        break;
    }
    return ret;
}

int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume)
{
    if (NULL == url) {
        return -1;
    }
    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    int8_t       ret       = 0;
    unsigned int flags     = 0;
    char *       local_url = NULL;

    if ((SMTAUDIO_STATE_MUTE != smtaudio_ctx.cur_state) && (MEDIA_SYSTEM == type)) {
        smtaudio_ctx.tts_resume = resume;
    }
    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_STATE_ZERO_VOLUME:
        volume_set(smtaudio_ctx.min_vol);
        if (MEDIA_SYSTEM == type) {
            if (SMTAUDIO_STATE_PLAYING == smtaudio_ctx.last_state) {
                /* 应该回到MUSIC播放状态，但是需要先等SYSTEM(TTS)播放完
                   所以现在只更新播控状态，具体恢复播放的动作，等SYSTEM结束
                   后再恢复 */
                smtaudio_ctx.cur_state = SMTAUDIO_STATE_PLAYING;
            } else {
                switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_PAUSE);
            }
        }
    case SMTAUDIO_STATE_PLAYING:
    case SMTAUDIO_STATE_PAUSE:
    case SMTAUDIO_STATE_STOP:
        if (MEDIA_SYSTEM == type) {
            /* 1. 首先清除等待播放的MUSIC，发送STOP EVENT */
            aos_event_set(&smtaudio_ctx.event_media_state, EVENT_MUSIC_STOPPED, AOS_EVENT_OR);
            /* 2. 播放SYSTEM */
            ret = aui_player_seek_play(type, url, seek_time, resume);
        } else {
            if (AUI_PLAYER_PLAYING == aui_player_get_state(MEDIA_SYSTEM)) {
                /* 1. 复制url到本地，防止url被改变 */
                int url_len = strlen(url);
                local_url   = aos_zalloc(url_len + 1);
                if (NULL == local_url) {
                    break;
                }
                strncpy(local_url, url, url_len);
                local_url[url_len] = '\0';
                /* 2. 等待下面条件之一满足：
                    a. SYSTEM播放完成；
                    b. 收到停止播放指令 */
                aos_event_get(&smtaudio_ctx.event_media_state,
                              EVENT_SYSTEM_FINISHED | EVENT_MUSIC_STOPPED, AOS_EVENT_OR_CLEAR,
                              &flags, AOS_WAIT_FOREVER);
                if (flags & EVENT_MUSIC_STOPPED) {
                    LOGD(TAG, "receive stop command! stats[%d]", flags);
                    break;
                }
                ret = aui_player_seek_play(type, local_url, seek_time, 0);
                if (local_url) {
                    aos_free(local_url);
                    local_url = NULL;
                }
            } else {
                ret = aui_player_seek_play(type, url, seek_time, 0);
            }
            switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_PLAYING);
        }
        break;
    case SMTAUDIO_STATE_MUTE:
        /* do nothing */
        break;
    default:
        break;
    }

    /* 清除MEDIA状态 */
    aos_event_set(&smtaudio_ctx.event_media_state, 0, AOS_EVENT_AND);
    return ret;
}

int8_t smtaudio_stop(int type)
{
    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    int ret = 0;
    if ((MEDIA_MUSIC == type) || (MEDIA_ALL == type)) {
        aos_event_set(&smtaudio_ctx.event_media_state, EVENT_MUSIC_STOPPED, AOS_EVENT_OR);
    }
    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_STATE_PLAYING:
    case SMTAUDIO_STATE_PAUSE:
    case SMTAUDIO_STATE_ZERO_VOLUME:
        if ((MEDIA_MUSIC == type) || (MEDIA_ALL == type)) {
            ret = aui_player_stop(MEDIA_MUSIC);
            switch_state(smtaudio_ctx.cur_state, SMTAUDIO_STATE_STOP);
        }
        if ((MEDIA_SYSTEM == type) || (MEDIA_ALL == type)) {
            ret = aui_player_stop(MEDIA_SYSTEM);
        }
        break;
    case SMTAUDIO_STATE_MUTE:
    case SMTAUDIO_STATE_STOP:
        /* MUSIC是STOP状态，但是SYSTEM有可能正在播放，依然需要停止 */
        if ((MEDIA_SYSTEM == type) || (MEDIA_ALL == type)) {
            ret = aui_player_stop(MEDIA_SYSTEM);
        }
        break;
    default:
        break;
    }

    return ret;
}

smtaudio_state_t smtaudio_get_state(void)
{
    return smtaudio_ctx.cur_state;
}

int8_t smtaudio_vol_get(void)
{
    return smtaudio_ctx.cur_vol;
}

int8_t smtaudio_lpm(uint8_t state)
{
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();
    aos_kernel_sched_suspend();
    smtaudio_ctx.lpm_flag = state;
    aos_kernel_sched_resume();
    csi_irq_restore(irq_flag);

    return 0;
}

int8_t smtaudio_deinit(void)
{
    int8_t ret = 0;
    aos_event_free(&smtaudio_ctx.event_media_state);
    if (NULL != smtaudio_ctx.task_media) {
        utask_destroy(smtaudio_ctx.task_media);
        smtaudio_ctx.task_media = NULL;
    }
    memset(&smtaudio_ctx, 0, sizeof(smtaudio_type_t));

    return ret;
}

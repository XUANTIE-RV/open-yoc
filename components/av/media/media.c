/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>
#include <media.h>
#include <yoc/eventid.h>
#include <devices/audio.h>
#include <alsa/mixer.h>
#include <player.h>

#define TAG "media"

/** 音频服务
 * type:
 *  0: 音乐播放
 *  1: 提示音播放
 *  2: 对话播放
 *      每一个 type 只有一个播放实例，新的播放会替换旧的播放实例，
 *      提示音的播放会先暂停音乐播放，播放完成之后继续音乐播放
 */
typedef struct {
    int type;
    int vol;
    int step_vol;
    int time;
} vol_param_t;

typedef struct {
    int         type;
    int         resume;
    char       *url;
} play_param_t;

typedef struct {
    int        type;
    uint64_t   seek_time;
} seek_param_t;

typedef enum {
    MEDIA_INIT_CMD,
    MEDIA_PLAY_CMD,
    MEDIA_PAUSE_CMD,
    MEDIA_RESUME_CMD,
    MEDIA_STOP_CMD,
    MEDIA_SEEK_CMD,
    MEDIA_GET_TIME_CMD,
    MEDIA_SET_VOL_CMD,
    MEDIA_SET_MIN_VOL_CMD,
    MEDIA_GET_VOL_CMD,
    MEDIA_ADD_VOL_CMD,
    MEDIA_STEP_VOL_CMD,
    MEDIA_EVT_ERR_CMD,
    MEDIA_EVT_RUN_CMD,
    MEDIA_EVT_END_CMD,
    MEDIA_CONTINUE_MUSIC_CMD,
    MEDIA_CONFIG_CMD,
    MEDIA_CONFIG_KEY_CMD,
    MEDIA_GET_STATE_CMD,
    MEDIA_MUTE_CMD,

    MEDIA_END_CMD
} MEDIA_CMD;

#define VOLUME_MAX (1000)
#define VOLUME_UNIT (10)
#define VOLUME_MIN (0)

#define MEDIA_MAX_NUM (2)

#define EVENT_STEP_VOL_ID (EVENT_USER + 100)

#define VOL_STEP_TIME (50)
#define VOL_STEP_MAX_TIME (5000)

typedef struct {
    int       type;
    player_t *hdl;
    int       speech_pause; //music 被其他打断标志
    int       state;
    int       play_vol;
    int       min_vol;
    int       cur_vol;
    int       exp_vol;
    int       step_vol;
    int       resume;
    aui_player_config_t config;
    media_key_cb_t key_cb;
} media_type_t;

typedef struct {
    uservice_t *      srv;
    aos_mixer_t *     mixer;
    aos_mixer_elem_t *elem;
    media_evt_t       cb;
} media_t;

static media_type_t         media_type[MEDIA_MAX_NUM];
static media_t              g_media = {NULL, NULL, NULL, NULL};
static eqfp_t * g_eq_segments = NULL;
static int g_eq_segment_count = 0;

static void          m_step_vol(media_type_t *m, int new_vol, int time);
static media_type_t *get_type(int type);
static int           m_vol_set(media_type_t *m, int vol);
#if 1
#define audio_set_vol(l, r)                                                                        \
    aos_mixer_selem_set_playback_volume_all(g_media.elem, (l + r) / VOLUME_UNIT / 2)
#endif
#define abs_to_per(abs_vol) (abs_vol / VOLUME_UNIT)
#define per_to_abs(per_vol) (per_vol * VOLUME_UNIT)
// FIXME:
extern int volicore_set(size_t vol);
#if 0
#define audio_set_vol(l, r) volicore_set(abs_to_per(l))
#else
// #define audio_set_vol(l, r) do {} while(0)
#endif

static void player_event(player_t *handle, uint8_t type, const void *data, uint32_t len)
{
    int i;

    for (i = 0; i < MEDIA_MAX_NUM; i++) {
        if (media_type[i].hdl == handle) {
            break;
        }
    }

    // media_type_t *m = get_type(i);
    LOGD(TAG, "media(%d) evt:%d\n", i, type);
    // aos_msleep(500);
    uservice_call_async(g_media.srv, MEDIA_EVT_ERR_CMD + type - 1, &i, 4);
}

static media_type_t *get_type(int type)
{
    return (&media_type[type]);
}

static inline void rpc_return_int(rpc_t *rpc, int ret)
{
    rpc_put_reset(rpc);
    rpc_put_int(rpc, ret);
}

static int media_evt_call(int type, int evt_id)
{
    if (g_media.cb) {
        g_media.cb(type, evt_id);
    }

    return 0;
}

static int m_stop(media_type_t *m)
{
    if (m->hdl && (m->state == AUI_PLAYER_PLAYING || m->state == AUI_PLAYER_PAUSED)) {
        player_stop(m->hdl);
        player_free(m->hdl);
        m->state = AUI_PLAYER_STOP;
        m->hdl   = 0;
    }

    return 0;
}

static int m_pause(media_type_t *m)
{
    if (m->hdl && m->state == AUI_PLAYER_PLAYING) {
        player_pause(m->hdl);
        m->state = AUI_PLAYER_PAUSED;
    }

    return 0;
}

static int m_vol_set(media_type_t *m, int vol)
{
    if ((vol > VOLUME_MAX && m->play_vol == VOLUME_MAX) ||
        (vol < VOLUME_MIN && m->play_vol == VOLUME_MIN)) {
        //LOGD(TAG, "set err->cur:%d  set:%d", abs_to_per(m->play_vol), abs_to_per(vol));
        return -1;
    }

    if (vol > VOLUME_MAX) {
        vol = VOLUME_MAX;
    } else if (vol < VOLUME_MIN) {
        vol = VOLUME_MIN;
    }

    m->play_vol = vol;

    LOGD(TAG, "set vol:%d type:%d", abs_to_per(m->play_vol), m->type);
    audio_set_vol(m->play_vol, m->play_vol);
    m->cur_vol = m->play_vol;

    return 0;
}

static int m_start(media_type_t *m, const char *url)
{
    if (m->state == AUI_PLAYER_STOP) {
        plyh_t plyh;
        int vol_ret = m_vol_set(m, m->play_vol);
        LOGD(TAG, "play%d vol:%d vol_ret:%d url:%s", m->type, abs_to_per(m->play_vol), vol_ret, url);
        // aos_msleep(120);  // FIXME:

        //TODO:
        memset(&plyh, 0, sizeof(plyh_t));
        plyh.ao_name       = "alsa";
        plyh.eq_segments   = g_eq_segment_count;
        plyh.resample_rate = m->config.resample_rate;
        plyh.cache_size    = m->config.web_cache_size;
        plyh.cache_percent = m->config.web_start_threshold;
        plyh.period_ms     = m->config.snd_period_ms;
        plyh.period_num    = m->config.snd_period_num;
        plyh.rcv_timeout   = 0;
        plyh.get_dec_cb    = m->key_cb;
        plyh.event_cb      = player_event;
        m->hdl = player_new(&plyh);

        /* EQ config */
        if(g_eq_segment_count > 0 && g_eq_segments != NULL) {
            int i;
            peq_seten_t eqen;
            peq_setpa_t eqpa;

            for (i = 0; i < plyh.eq_segments; i++) {
                eqpa.segid = i;
                eqpa.param = g_eq_segments[i];
                //memcpy(&eqpa.param, (void*)&bqfparams[i], sizeof(eqfp_t));
                if (g_eq_segments[i].type != EQF_TYPE_UNKNOWN) {
                    player_ioctl(m->hdl, PLAYER_CMD_EQ_SET_PARAM, &eqpa);
                }
            }

            eqen.enable = 1;
            player_ioctl(m->hdl, PLAYER_CMD_EQ_ENABLE, &eqen);
        }

        if (m->hdl == NULL) {
            LOGE(TAG, "play err(%s)", url);
        } else {
            player_play(m->hdl, url);
            m->state    = AUI_PLAYER_PLAYING;
        }
    }

    return m->hdl ? 0 : -1;
}

static int m_continue(media_type_t *m)
{
    if (m->hdl) {
        if (m->state == AUI_PLAYER_PAUSED) {
            audio_set_vol(m->cur_vol, m->cur_vol);
            player_resume(m->hdl);
            m->state = AUI_PLAYER_PLAYING;
        } else if (m->state == AUI_PLAYER_PLAYING && m->cur_vol == 0) {
            audio_set_vol(m->play_vol, m->play_vol);
            m->cur_vol = m->play_vol;
        }

        return 0;
    }

    return -1;
}

static void music_stash(void)
{
    media_type_t *music = get_type(MEDIA_MUSIC);

    if (music->state == AUI_PLAYER_PLAYING) {
        music->speech_pause = 1;
    }

    m_step_vol(music, per_to_abs(1), 100);

    m_pause(music);
}

static void music_stash_pop(void)
{
    media_type_t *m = get_type(MEDIA_MUSIC);

    if (m->speech_pause == 1) {
        m->speech_pause = 0;
        // audio_set_vol(m->cur_vol, m->cur_vol);
        m_continue(m);
        m_step_vol(m, m->play_vol, 1000);
    }
}

static void media_vol_init(int vol)
{
    audio_set_vol(vol, vol);

    for (int i = 0; i < MEDIA_MAX_NUM; i++) {
        media_type[i].type  = i;
        media_type[i].state = AUI_PLAYER_STOP;
        // media_type[i].cur_vol = vol;
        media_type[i].play_vol = vol;
        media_type[i].min_vol  = per_to_abs(10);
    }
}

static void mixer_init(void)
{
    aos_mixer_open(&g_media.mixer, 0);
    aos_mixer_attach(g_media.mixer, "card0");
    aos_mixer_load(g_media.mixer);
    g_media.elem = aos_mixer_first_elem(g_media.mixer);

    if (g_media.elem == NULL) {
        LOGI(TAG, "elem fine NULL");
    }
}

static int _init(media_t *media, rpc_t *rpc)
{
    g_media.cb = (media_evt_t) rpc_get_point(rpc);;
    player_init();
    mixer_init();
    media_vol_init(per_to_abs(60));
    return 0;
}

// 开始播放（停止前一个播放）  "fifo://tts"
static int _play(media_t *media, rpc_t *rpc)
{
    play_param_t *param   = (play_param_t *)rpc_get_buffer(rpc, NULL);
    media_type_t *m       = get_type(param->type);
    int           ret     = -1;
    int           start   = 1;

    LOGD(TAG, "state:%d type:%d\n", m->state, m->type);

    media_evt_call(m->type, AUI_PLAYER_EVENT_START);

    if (m->type == MEDIA_SYSTEM) {
        m->resume = param->resume;
        music_stash();
        m_stop(get_type(MEDIA_SYSTEM));
    } else {
        media_type_t *m_s = get_type(MEDIA_SYSTEM);

        if (m_s->state == AUI_PLAYER_PLAYING) {
            start = 0;
        } else {
            m_stop(get_type(MEDIA_MUSIC));
        }
    }

    if (start == 1) {
        if (m->play_vol < m->min_vol && m->cur_vol < m->min_vol) {
            m_vol_set(m, m->min_vol);
        }

        ret = m_start(m, param->url);

        if (ret < 0) {
            media_evt_call(m->type, AUI_PLAYER_EVENT_ERROR);
        }
    }

    aos_free(param->url);
    return 0;
}

// 暂停
static int _pause(media_t *media, rpc_t *rpc)
{
    int           type    = *(int *)rpc_get_point(rpc);
    int           rpc_ret = -1;

    if (type != MEDIA_ALL) {
        media_type_t *m = get_type(type);
        rpc_ret         = m_pause(m);
        m->speech_pause = 0;
    } else {
        for (int i = 0; i < MEDIA_MAX_NUM; i++) {
            media_type_t *m = get_type(i);
            rpc_ret         = m_pause(m);
            m->speech_pause = 0;
        }
    }

    rpc_return_int(rpc, rpc_ret);
    return 0;
}

// 继续
static int _continue(media_t *media, rpc_t *rpc)
{
    int           type    = *(int *)rpc_get_point(rpc);
    media_type_t *m       = get_type(type);
    int           rpc_ret = -1;

    if (m->play_vol < m->min_vol && m->cur_vol < m->min_vol) {
        m_vol_set(m, m->min_vol);
    }

    rpc_ret = m_continue(m);

    rpc_return_int(rpc, rpc_ret);

    return 0;
}

// 停止
static int _stop(media_t *media, rpc_t *rpc)
{
    int type    = *(int *)rpc_get_point(rpc);
    int rpc_ret = -1;

    LOGD(TAG, "media%d stop\n", type);

    if (type != MEDIA_ALL) {
        rpc_ret = m_stop(get_type(type));
    } else {
        for (int i = 0; i < MEDIA_MAX_NUM; i++) {
            rpc_ret = m_stop(get_type(i));
        }
    }

    rpc_return_int(rpc, rpc_ret);

    return 0;
}

static int _mute(media_t *media, rpc_t *rpc)
{
    int           type = rpc_get_int(rpc);
    media_type_t *m;

    LOGD(TAG, "media(%d) mute", type);
    if (type != MEDIA_ALL) {
        m          = get_type(type);
        m->cur_vol = 0;
    } else {
        for (int i = 0; i < MEDIA_MAX_NUM; i++) {
            m          = get_type(i);
            m->cur_vol = 0;
        }
    }

    audio_set_vol(0, 0);

    return 0;
}

static int _resume(media_t *media, rpc_t *rpc)
{
    int           type = rpc_get_int(rpc);
    media_type_t *m    = get_type(type);

    LOGD(TAG, "music resume");

    for (int i = 0; i < MEDIA_MAX_NUM; i++) {
        media_type_t *m_m = get_type(i);

        if (m != m_m && m_m->state == AUI_PLAYER_PLAYING) {
            LOGD(TAG, "music resume err(%d)(%d)", m_m->type, m_m->state);
            return 0;
        }
    }

    music_stash_pop();

    return 0;
}

static int _seek(media_t *media, rpc_t *rpc)
{
    seek_param_t *param = (seek_param_t *)rpc_get_point(rpc);
    media_type_t *m     = get_type(param->type);

    int rpc_ret = player_seek(m->hdl, param->seek_time);

    rpc_return_int(rpc, rpc_ret);

    return 0;
}

static int _get_time(media_t *media, rpc_t *rpc)
{
    int type            = *(int *)rpc_get_point(rpc);
    media_type_t *m     = get_type(type);
    int rpc_ret         = -1;

    play_time_t p = {
        .curtime = -1,
        .duration = -1,
    };

    rpc_ret = player_get_cur_ptime(m->hdl, &p);

    rpc_put_reset(rpc);
    rpc_put_buffer(rpc, &p, sizeof(p));

    return 0;
}

static int _config(media_t *media, rpc_t *rpc)
{
    aui_player_config_t *config = rpc_get_point(rpc);
    media_type_t *m;

    for (int i = 0; i < MEDIA_MAX_NUM; i++) {
        m = get_type(i);

        memcpy(&m->config, config, sizeof(aui_player_config_t));
    }

    return 0;
}

static int _key(media_t *media, rpc_t *rpc)
{
    media_key_cb_t *cb = rpc_get_point(rpc);
    media_type_t   *m;

    for (int i = 0; i < MEDIA_MAX_NUM; i++) {
        m = get_type(i);

        m->key_cb = *cb;
    }

    return 0;
}

// 增加增量设置， 正数加大，负数减小
static int _vol_add(media_t *media, rpc_t *rpc)
{
    vol_param_t * param = (vol_param_t *)rpc_get_point(rpc);
    media_type_t *m;
    int           volume;
    int           add_vol = per_to_abs(param->vol);
    int           rpc_ret = -1;

    if (param->type != MEDIA_ALL) {
        m       = get_type(param->type);
        volume  = m->play_vol + add_vol;
        rpc_ret = m_vol_set(m, volume);
    } else {
        for (int i = 0; i < MEDIA_MAX_NUM; i++) {
            m       = get_type(i);
            volume  = m->play_vol + add_vol;
            rpc_ret = m_vol_set(m, volume);
        }
    }

    rpc_return_int(rpc, rpc_ret);

    return 0;
}

// 设置绝对音量
static int _vol_set(media_t *media, rpc_t *rpc)
{
    vol_param_t * param   = (vol_param_t *)rpc_get_point(rpc);
    int           rpc_ret = -1;
    int           volume  = per_to_abs(param->vol);
    media_type_t *m;

    if (param->type != MEDIA_ALL) {
        m       = get_type(param->type);
        rpc_ret = m_vol_set(m, volume);
    } else {
        for (int i = 0; i < MEDIA_MAX_NUM; i++) {
            m = get_type(i);

            rpc_ret = m_vol_set(m, volume);
        }
    }

    rpc_return_int(rpc, rpc_ret);

    return 0;
}

static int _set_min_vol(media_t *media, rpc_t *rpc)
{
    vol_param_t * param  = (vol_param_t *)rpc_get_point(rpc);
    int           volume = per_to_abs(param->vol);
    media_type_t *m;

    if (param->type != MEDIA_ALL) {
        m          = get_type(param->type);
        m->min_vol = volume;

        if (m->play_vol < m->min_vol) {
            m->play_vol = m->min_vol;
        }
    } else {
        for (int i = 0; i < MEDIA_MAX_NUM; i++) {
            m = get_type(i);
            m->min_vol = volume;

            if (m->play_vol < m->min_vol) {
                m->play_vol = m->min_vol;
            }
        }
    }

    rpc_return_int(rpc, 0);

    return 0;
}

static int _vol_get(media_t *media, rpc_t *rpc)
{
    int           type = *(int *)rpc_get_point(rpc);
    media_type_t *m    = get_type(type);

    rpc_return_int(rpc, abs_to_per(m->play_vol));

    return 0;
}

static void _evt_hdl(rpc_t *rpc, int evt_id)
{
    int           type = rpc_get_int(rpc);
    media_type_t *m    = (media_type_t *)get_type(type);

    m_stop(m);
    if (type == MEDIA_SYSTEM && m->resume == 1) {
        music_stash_pop();
        m->resume = 0;
    }
    media_evt_call(m->type, evt_id);

    //LOGD(TAG, "media(%d) evt end\n", m->type);
}

static int _evt_err(media_t *media, rpc_t *rpc)
{
    //播放异常
    _evt_hdl(rpc, AUI_PLAYER_EVENT_ERROR);
    return 0;
}

static int _evt_ok(media_t *media, rpc_t *rpc)
{
    //播放完成
    _evt_hdl(rpc, AUI_PLAYER_EVENT_FINISH);
    return 0;
}

static int _evt_run(media_t *media, rpc_t *rpc)
{
    // int type = rpc_get_int(rpc);
    // media_type_t *m = (media_type_t*)get_type(type);

    // int vol_ret = m_vol_set(m, m->play_vol);
    // LOGD(TAG,"play%d vol:%d vol_ret:%d\n",m->type, abs_to_per(m->play_vol), vol_ret);

    return 0;
}

static void _step_vol_handle(int type)
{
    media_type_t *m = get_type(type);

    if (m->cur_vol == m->exp_vol) {
        return;
    }

    m->cur_vol -= m->step_vol;
    audio_set_vol(m->cur_vol, m->cur_vol);

    if ((m->cur_vol - m->exp_vol) / m->step_vol == 0) {
        m->step_vol = m->cur_vol - m->exp_vol;
    }
}

static int _get_step_vol(int old, int new, int ms)
{
    ms          = ms > VOL_STEP_MAX_TIME ? VOL_STEP_MAX_TIME : ms;
    int i       = ms / VOL_STEP_TIME;
    int vol_gap = old - new;
    int flag    = vol_gap > 0 ? 1 : -1; //+:1 -:0

    vol_gap = vol_gap > 0 ? vol_gap : -vol_gap;

    if (i <= 1 || vol_gap < i) {
        return vol_gap * flag;
    } else {
        return (flag * (vol_gap + i / 2) / i);
    }
}

static void _step_vol_sleep(int t1, int t2)
{
    if (t1 > t2) {
        if ((t1 - t2) < VOL_STEP_TIME) {
            aos_msleep(VOL_STEP_TIME - (t1 - t2));
        }
    } else {
        aos_msleep(VOL_STEP_TIME + (t2 - t1));
    }
}

static void m_step_vol(media_type_t *m, int new_vol, int time)
{
    if (m->hdl && m->state == AUI_PLAYER_PLAYING) {
        int       i, j;
        long long old_t, new_t, off_t;

        m->exp_vol  = new_vol;
        m->step_vol = _get_step_vol(m->cur_vol, new_vol, time);

        i = time / VOL_STEP_TIME;
        j = 0;
        LOGD(TAG, "media vol start(%d)(%d)(%d) :%d", m->cur_vol / 1, i, m->step_vol, aos_now_ms());
        old_t = aos_now_ms();
        off_t = 0;

        while (i--) {
            _step_vol_handle(0);
            //printf("---%d---\n", m->cur_vol);
            _step_vol_sleep(off_t, VOL_STEP_TIME * j);
            new_t = aos_now_ms();
            off_t = new_t - old_t;
            j++;
        }

        _step_vol_handle(0);
        LOGD(TAG, "media vol end(%d) :%d", m->cur_vol / 1, aos_now_ms());
    }
}
// 音量在指定的时间内逐渐调整到指定音量
static int _vol_gradual(media_t *media, rpc_t *rpc)
{
    vol_param_t * param = (vol_param_t *)rpc_get_buffer(rpc, NULL);
    media_type_t *m     = get_type(param->type);

    m_step_vol(m, param->vol, param->time);

    m->play_vol = param->vol;

    return 0;
}

static int _get_state(media_t *media, rpc_t *rpc)
{
    int           type = *(int *)rpc_get_point(rpc);
    media_type_t *m    = get_type(type);

    if (m->speech_pause == 0) {
        rpc_return_int(rpc, m->state);
    } else {
        rpc_return_int(rpc, (int)AUI_PLAYER_PLAYING);
    }

    return 0;
}

int aui_player_play(int type, const char *url, int resume)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM && url && strlen(url) > 0);
    int          ret = -1;
    play_param_t param;

    param.type = type;
    param.url  = strdup(url);
    param.resume = resume;

    ret = uservice_call_async(g_media.srv, MEDIA_PLAY_CMD, &param, sizeof(play_param_t));
    return ret;
}

int aui_player_pause(int type)
{
    aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);
    int ret = -1;
    int rpc_ret;

    ret =
        uservice_call_sync(g_media.srv, MEDIA_PAUSE_CMD, (void *)&type, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}

int aui_player_resume(int type)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM);
    int ret = -1;
    int rpc_ret;
    ret =
        uservice_call_sync(g_media.srv, MEDIA_RESUME_CMD, (void *)&type, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}

int aui_player_stop(int type)
{
    aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);
    int ret = -1;
    int rpc_ret;
    ret = uservice_call_sync(g_media.srv, MEDIA_STOP_CMD, (void *)&type, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}

int aui_player_mute(int type)
{
    aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);
    int ret = -1;

    ret = uservice_call_async(g_media.srv, MEDIA_MUTE_CMD, &type, sizeof(int));
    return ret;
}

int aui_player_seek(int type, uint64_t seek_time)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM);
    int ret = -1;
    int rpc_ret;
    seek_param_t param;

    param.type      = type;
    param.seek_time = seek_time;

    ret = uservice_call_sync(g_media.srv, MEDIA_SEEK_CMD, (void *)&param, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}


int aui_player_get_time(int type, aui_play_time_t *t)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM);
    int ret = -1;

    ret = uservice_call_sync(g_media.srv, MEDIA_GET_TIME_CMD, (void *)&type, t, sizeof(aui_play_time_t));
    return ret;
}
/*
   int media_event(int type, int state)
   {
   aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);
   int ret = -1;
// ret = uservice_call_sync(g_media.srv, MEDIA_, &type, NULL, 0);
return ret;
}
*/

int aui_player_vol_adjust(int type, int inc_volume)
{
    aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);
    int         ret = -1;
    vol_param_t param;
    int         rpc_ret;

    param.type = type;
    param.vol  = inc_volume;
    ret = uservice_call_sync(g_media.srv, MEDIA_ADD_VOL_CMD, &param, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}

int aui_player_vol_set(int type, int volume)
{
    aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);

    if (volume < VOLUME_MIN || volume > (VOLUME_MAX / VOLUME_UNIT)) {
        return -1;
    }

    int         ret = -1;
    vol_param_t param;
    int         rpc_ret;

    param.type = type;
    param.vol  = volume;

    ret = uservice_call_sync(g_media.srv, MEDIA_SET_VOL_CMD, &param, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}

int aui_player_set_minvol(int type, int volume)
{
    aos_check_return_einval((type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM) || type == MEDIA_ALL);

    if (volume < VOLUME_MIN || volume > (VOLUME_MAX / VOLUME_UNIT)) {
        return -1;
    }

    int         ret = -1;
    vol_param_t param;
    int         rpc_ret;

    param.type = type;
    param.vol  = volume;

    ret = uservice_call_sync(g_media.srv, MEDIA_SET_MIN_VOL_CMD, &param, &rpc_ret, sizeof(rpc_ret));
    return ret < 0 ? ret : rpc_ret;
}

int aui_player_vol_get(int type)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM);
    int ret = -1;
    int vol = -1;

    ret = uservice_call_sync(g_media.srv, MEDIA_GET_VOL_CMD, &type, &vol, sizeof(int));
    return ret < 0 ? ret : vol;
}

int aui_player_config(aui_player_config_t *config)
{
    aos_check_return_einval(config);
    int ret = -1;

    ret = uservice_call_sync(g_media.srv, MEDIA_CONFIG_CMD, config, NULL, 0);
    return ret;
}

int aui_player_key_config(media_key_cb_t cb)
{
    aos_check_return_einval(cb);
    int ret = -1;

    ret = uservice_call_sync(g_media.srv, MEDIA_CONFIG_KEY_CMD, &cb, NULL, 0);
    return ret;
}

int aui_player_vol_gradual(int type, int new_volume, int ms)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM);

    if (ms > 5000) {
        ms = 5000;
    }

    if (new_volume < VOLUME_MIN || new_volume > (VOLUME_MAX / VOLUME_UNIT)) {
        return -1;
    }

    int         ret = -1;
    vol_param_t param;

    param.type = type;
    param.vol  = per_to_abs(new_volume);
    // param.step_vol = step_vol;
    param.time = ms;

    ret = uservice_call_async(g_media.srv, MEDIA_STEP_VOL_CMD, &param, sizeof(vol_param_t));
    return ret;
}

aui_player_state_t aui_player_get_state(int type)
{
    aos_check_return_einval(type >= MEDIA_MUSIC && type <= MEDIA_SYSTEM);
    int ret   = -1;
    int state = AUI_PLAYER_UNKNOWN;

    if (g_media.srv == NULL) {
        return state;
    }

    if (media_type[0].state ==  AUI_PLAYER_UNKNOWN || media_type[1].state ==  AUI_PLAYER_UNKNOWN) {
        return state;
    }

    ret = uservice_call_sync(g_media.srv, MEDIA_GET_STATE_CMD, &type, &state, sizeof(int));
    return ret < 0 ? AUI_PLAYER_UNKNOWN : state;
}

int aui_player_resume_music(void)
{
    int type = MEDIA_MUSIC;
    int ret  = -1;

    ret = uservice_call_async(g_media.srv, MEDIA_CONTINUE_MUSIC_CMD, &type, sizeof(int));
    return ret;
}

int aui_player_eq_config(eqfp_t *eq_segments, int count)
{
    if (eq_segments == NULL || count <= 0) {
        aos_free(g_eq_segments);
        g_eq_segments = NULL;
        g_eq_segment_count = 0;
        return 0;
    }

    aos_free(g_eq_segments);
    g_eq_segments = aos_malloc(sizeof(eqfp_t) * count);
    g_eq_segment_count = count;

    memcpy(g_eq_segments, eq_segments, sizeof(eqfp_t) * count);

    return 0;
}

static const rpc_process_t c_media_cmd_cb_table[] = {
    {MEDIA_INIT_CMD,            (process_t)_init},
    {MEDIA_PLAY_CMD,            (process_t)_play},
    {MEDIA_PAUSE_CMD,           (process_t)_pause},
    {MEDIA_RESUME_CMD,          (process_t)_continue},
    {MEDIA_STOP_CMD,            (process_t)_stop},
    {MEDIA_SEEK_CMD,            (process_t)_seek},
    {MEDIA_SET_VOL_CMD,         (process_t)_vol_set},
    {MEDIA_SET_MIN_VOL_CMD,     (process_t)_set_min_vol},
    {MEDIA_GET_VOL_CMD,         (process_t)_vol_get},
    {MEDIA_ADD_VOL_CMD,         (process_t)_vol_add},
    {MEDIA_STEP_VOL_CMD,        (process_t)_vol_gradual},
    {MEDIA_EVT_ERR_CMD,         (process_t)_evt_err},
    {MEDIA_EVT_RUN_CMD,         (process_t)_evt_run},
    {MEDIA_EVT_END_CMD,         (process_t)_evt_ok},
    {MEDIA_GET_STATE_CMD,       (process_t)_get_state},
    {MEDIA_MUTE_CMD,            (process_t)_mute},
    {MEDIA_CONTINUE_MUSIC_CMD,  (process_t)_resume},
    {MEDIA_CONFIG_CMD,          (process_t)_config},
    {MEDIA_CONFIG_KEY_CMD,      (process_t)_key},
    {MEDIA_GET_TIME_CMD,        (process_t)_get_time},

    {MEDIA_END_CMD, (process_t)NULL},
};

int media_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, c_media_cmd_cb_table);
}

int aui_player_init(utask_t *task, media_evt_t evt_cb)
{
    aos_check_return_einval(task);

    if (g_media.srv != NULL) {
        return -1;
    }

    g_media.srv = uservice_new("media", media_process_rpc, &g_media);

    aos_check_return_enomem(g_media.srv);
    utask_add(task, g_media.srv);
    uservice_call_async(g_media.srv, MEDIA_INIT_CMD, &evt_cb, 4);

    return 0;
}

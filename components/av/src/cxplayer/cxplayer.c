/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include "av/cav/xplayer/cxplayer_cls.h"
#include "av/cav/xplayer/cxplayer.h"
#include "av/cav/xplayer/cxplayer_all.h"

#define TAG                    "cxplayer"

#define cxplayer_lock()         (aos_mutex_lock(&cxplayer->lock, AOS_WAIT_FOREVER))
#define cxplayer_unlock()       (aos_mutex_unlock(&cxplayer->lock))

/**
 * @brief  new one cxplayer obj by the name. create default player if name is NULL
 * @param  [in] name : player class by the name, may be NULL
 * @return NULL on error
 */
cxplayer_t* cxplayer_new(const char *name)
{
    int rc = -1;
    cxplayer_t *cxplayer;
    struct cxplayer_cls *cls;

    cls = _get_cxplayer_cls(name);
    if (!cls) {
        LOGE(TAG, "error. cxplayer cls had not regist yet!, name = %s", name ? name : "default");
        return NULL;
    }
    cxplayer = aos_zalloc(sizeof(cxplayer_t) + cls->priv_size);
    cxplayer->vol    = 180;
    cxplayer->speed  = 1.0;
    cxplayer->cls    = cls;
    cxplayer->status = CXPLAYER_STATUS_STOPED;

    rc = cxplayer->cls->ops->init(cxplayer);
    if (rc) {
        LOGE(TAG, "error. cxplayer init fail!");
        av_free(cxplayer);
        return NULL;
    }

    aos_mutex_new(&cxplayer->lock);

    return cxplayer;
}

/**
 * @brief  get config param from the cxplayer
 * @param  [in] cxplayer
 * @param  [in/out] conf
 * @return 0/-1
 */
int cxplayer_get_config(cxplayer_t *cxplayer, cxplayer_cnf_t *conf)
{
    int rc = -1;

    CHECK_PARAM(cxplayer && conf, -1);
    cxplayer_lock();
    //LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    memset(conf, 0, sizeof(cxplayer_cnf_t));
    rc = cxplayer->cls->ops->get_config ? cxplayer->cls->ops->get_config(cxplayer, conf) : rc;
    //LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  set config param for the cxplayer before play
 * @param  [in] cxplayer
 * @param  [in] conf
 * @return 0/-1
 */
int cxplayer_set_config(cxplayer_t *cxplayer, const cxplayer_cnf_t *conf)
{
    int rc = -1;
    CHECK_PARAM(cxplayer && conf, -1);
    cxplayer_lock();
    //LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_config ? cxplayer->cls->ops->set_config(cxplayer, conf) : rc;
    //LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  set event callback before play
 * @param  [in] cxplayer
 * @param  [in] event_cb : callback of the cxplayer event
 * @param  [in] user_data : user data for event_cb
 * @return 0/-1
 */
int cxplayer_set_callback(cxplayer_t *cxplayer, cxplayer_eventcb_t event_cb, const void *user_data)
{
    CHECK_PARAM(cxplayer && event_cb, -1);
    cxplayer_lock();
    //LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer->event_cb  = event_cb;
    cxplayer->user_data = (void*)user_data;
    //LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return 0;
}

/**
 * @brief  set play source url before play
 * @param  [in] cxplayer
 * @param  [in] url : example: http://ip:port/xx.mp3
 * @return 0/-1
 */
int cxplayer_set_url(cxplayer_t *cxplayer, const char *url)
{
    CHECK_PARAM(cxplayer && url, -1);
    cxplayer_lock();
    //LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    av_freep(&cxplayer->url);
    cxplayer->url = strdup(url);
    //LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return 0;
}

/**
 * @brief  cxplayer play interface
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_play(cxplayer_t *cxplayer)
{
    int rc;

    CHECK_PARAM(cxplayer && cxplayer->url, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->play(cxplayer);
    if (rc) {
        LOGE(TAG, "play error, rc = %d, url = %s", rc, cxplayer->url);
    } else {
        cxplayer->status = CXPLAYER_STATUS_PLAYING;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  pause the cxplayer
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_pause(cxplayer_t *cxplayer)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    if (cxplayer->status == CXPLAYER_STATUS_PLAYING) {
        rc = cxplayer->cls->ops->pause(cxplayer);
        cxplayer->status = rc == 0 ? CXPLAYER_STATUS_PAUSED : cxplayer->status;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  resume the cxplayer
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_resume(cxplayer_t *cxplayer)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    if (cxplayer->status == CXPLAYER_STATUS_PAUSED) {
        rc = cxplayer->cls->ops->resume(cxplayer);
        cxplayer->status = rc == 0 ? CXPLAYER_STATUS_PLAYING : cxplayer->status;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  stop the cxplayer
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_stop(cxplayer_t *cxplayer)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    if (cxplayer->status != CXPLAYER_STATUS_STOPED) {
        rc = cxplayer->cls->ops->stop(cxplayer);
        cxplayer->status = CXPLAYER_STATUS_STOPED;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  free/destroy the cxplayer obj
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_free(cxplayer_t *cxplayer)
{
    CHECK_PARAM(cxplayer, -1);
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_stop(cxplayer);
    cxplayer->cls->ops->uninit(cxplayer);
    aos_mutex_free(&cxplayer->lock);
    av_freep(&cxplayer->url);
    av_free(cxplayer);
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);

    return 0;
}

/**
 * @brief  seek to the time
 * @param  [in] cxplayer
 * @param  [in] timestamp : seek time
 * @return 0/-1
 */
int cxplayer_seek(cxplayer_t *cxplayer, uint64_t timestamp)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    if (cxplayer->status != CXPLAYER_STATUS_STOPED) {
        rc = cxplayer->cls->ops->seek ? cxplayer->cls->ops->seek(cxplayer, timestamp) : rc;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  set start play time before play
 * @param  [in] cxplayer
 * @param  [in] start_time : start play time
 * @return 0/-1
 */
int cxplayer_set_start_time(cxplayer_t *cxplayer, uint64_t start_time)
{
    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    //LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer->start_time = start_time;
    //LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return 0;
}

/**
 * @brief  get play time(current time & duration, ms)
 * @param  [in] cxplayer
 * @param  [in/out] time
 * @return 0/-1
 */
int cxplayer_get_time(cxplayer_t *cxplayer, xplay_time_t *time)
{
    int rc = -1;

    CHECK_PARAM(cxplayer && time, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->get_time ? cxplayer->cls->ops->get_time(cxplayer, time) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  get media info
 * @param  [in] cxplayer
 * @param  [in/out] minfo : need free by the caller(use media_info_uninit function)
 * @return 0/-1
 */
int cxplayer_get_media_info(cxplayer_t *cxplayer, xmedia_info_t *minfo)
{
    int rc = -1;

    CHECK_PARAM(cxplayer && minfo, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->get_media_info ? cxplayer->cls->ops->get_media_info(cxplayer, minfo) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  get soft vol index of the cxplayer
 * @param  [in] cxplayer
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int cxplayer_get_vol(cxplayer_t *cxplayer, uint8_t *vol)
{
    CHECK_PARAM(cxplayer && vol, -1);
    *vol = cxplayer->vol;
    return 0;
}

/**
 * @brief  set soft vol index of the cxplayer
 * @param  [in] cxplayer
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int cxplayer_set_vol(cxplayer_t *cxplayer, uint8_t vol)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_vol ? cxplayer->cls->ops->set_vol(cxplayer, vol) : rc;
    if (rc == 0) {
        cxplayer->vol = vol;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  get play speed of the cxplayer
 * @param  [in] cxplayer
 * @param  [out] speed
 * @return 0/-1
 */
int cxplayer_get_speed(cxplayer_t *cxplayer, float *speed)
{
    CHECK_PARAM(cxplayer && speed, -1);
    *speed = cxplayer->speed;
    return 0;
}

/**
 * @brief  set play speed of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] speed : 0.5 ~ 2.0 for audio, etc
 * @return 0/-1
 */
int cxplayer_set_speed(cxplayer_t *cxplayer, float speed)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_speed ? cxplayer->cls->ops->set_speed(cxplayer, speed) : rc;
    if (rc == 0) {
        cxplayer->speed = speed;
    }
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  mute/unmute the cxplayer
 * @param  [in] cxplayer
 * @param  [in] mute : mute flag
 * @return 0/-1
 */
int cxplayer_set_mute(cxplayer_t *cxplayer, uint8_t mute)
{
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_mute ? cxplayer->cls->ops->set_mute(cxplayer, mute) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
}

/**
 * @brief  get play source url
 * @param  [in] cxplayer
 * @return NULL on error
 */
const char* cxplayer_get_url(cxplayer_t *cxplayer)
{
    return cxplayer ? cxplayer->url : NULL;
}

/**
 * @brief  switch audio track of the player
 * @param  [in] cxplayer
 * @param  [in] idx : index of audios
 * @return 0/-1
 */
int cxplayer_switch_audio_track(cxplayer_t *cxplayer, uint8_t idx)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->switch_audio_track ? cxplayer->cls->ops->switch_audio_track(cxplayer, idx) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  switch subtitle track of the player
 * @param  [in] cxplayer
 * @param  [in] idx : index of subtitles
 * @return 0/-1
 */
int cxplayer_switch_subtitle_track(cxplayer_t *cxplayer, uint8_t idx)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->switch_subtitle_track ? cxplayer->cls->ops->switch_subtitle_track(cxplayer, idx) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  set external subtitle url of the player
 * @param  [in] cxplayer
 * @param  [in] url : url of the external subtitle
 * @return 0/-1
 */
int cxplayer_set_subtitle_url(cxplayer_t *cxplayer, const char *url)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer && url, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_subtitle_url ? cxplayer->cls->ops->set_subtitle_url(cxplayer, url) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  show/hide subtitle of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int cxplayer_set_subtitle_visible(cxplayer_t *cxplayer, uint8_t visible)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_subtitle_visible ? cxplayer->cls->ops->set_subtitle_visible(cxplayer, visible) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  show/hide video of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int cxplayer_set_video_visible(cxplayer_t *cxplayer, uint8_t visible)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_video_visible ? cxplayer->cls->ops->set_video_visible(cxplayer, visible) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  crop video of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] win : crop window
 * @return 0/-1
 */
int cxplayer_set_video_crop(cxplayer_t *cxplayer, const cxwindow_t *win)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer && win, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_video_crop ? cxplayer->cls->ops->set_video_crop(cxplayer, win) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  set display window of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] win : display window
 * @return 0/-1
 */
int cxplayer_set_display_window(cxplayer_t *cxplayer, const cxwindow_t *win)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer && win, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_display_window ? cxplayer->cls->ops->set_display_window(cxplayer, win) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  enable/disable fullscreen for the cxplayer
 * @param  [in] cxplayer
 * @param  [in] onoff
 * @return 0/-1
 */
int cxplayer_set_fullscreen(cxplayer_t *cxplayer, uint8_t onoff)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_fullscreen ? cxplayer->cls->ops->set_fullscreen(cxplayer, onoff) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  set display format for the cxplayer
 * @param  [in] cxplayer
 * @param  [in] format
 * @return 0/-1
 */
int cxplayer_set_display_format(cxplayer_t *cxplayer, cxdisplay_format_t format)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_display_format ? cxplayer->cls->ops->set_display_format(cxplayer, format) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}

/**
 * @brief  rotate video of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] type : rotate type
 * @return 0/-1
 */
int cxplayer_set_video_rotate(cxplayer_t *cxplayer, cxrotate_type_t type)
{
#if CONFIG_AV_AUDIO_ONLY_SUPPORT
    LOGD(TAG, "%s, %d. audio only support!", __FUNCTION__, __LINE__);
    return -1;
#else
    int rc = -1;

    CHECK_PARAM(cxplayer, -1);
    cxplayer_lock();
    LOGD(TAG, "%s, %d enter. player = %p", __FUNCTION__, __LINE__, cxplayer);
    rc = cxplayer->cls->ops->set_video_rotate ? cxplayer->cls->ops->set_video_rotate(cxplayer, type) : rc;
    LOGD(TAG, "%s, %d leave. player = %p", __FUNCTION__, __LINE__, cxplayer);
    cxplayer_unlock();

    return rc;
#endif
}



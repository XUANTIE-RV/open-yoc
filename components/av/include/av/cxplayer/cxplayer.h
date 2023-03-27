/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __CXPLAYER_H__
#define __CXPLAYER_H__

#include "av/avutil/av_config.h"
#include "av/cav/xplayer/cxplayer_typedef.h"

__BEGIN_DECLS__

typedef struct cxplayer_struct cxplayer_t;

/**
 * @brief  new one cxplayer obj by the name. create default player if name is NULL
 * @param  [in] name : player class by the name, may be NULL
 * @return NULL on error
 */
cxplayer_t* cxplayer_new(const char *name);

/**
 * @brief  get config param from the cxplayer
 * @param  [in] cxplayer
 * @param  [in/out] conf
 * @return 0/-1
 */
int cxplayer_get_config(cxplayer_t *cxplayer, cxplayer_cnf_t *conf);

/**
 * @brief  set config param for the cxplayer before play
 * @param  [in] cxplayer
 * @param  [in] conf
 * @return 0/-1
 */
int cxplayer_set_config(cxplayer_t *cxplayer, const cxplayer_cnf_t *conf);

/**
 * @brief  set event callback before play
 * @param  [in] cxplayer
 * @param  [in] event_cb : callback of the cxplayer event
 * @param  [in] user_data : user data for event_cb
 * @return 0/-1
 */
int cxplayer_set_callback(cxplayer_t *cxplayer, cxplayer_eventcb_t event_cb, const void *user_data);

/**
 * @brief  set play source url before play
 * @param  [in] cxplayer
 * @param  [in] url : example: http://ip:port/xx.mp3
 * @return 0/-1
 */
int cxplayer_set_url(cxplayer_t *cxplayer, const char *url);

/**
 * @brief  cxplayer play interface
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_play(cxplayer_t *cxplayer);

/**
 * @brief  pause the cxplayer
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_pause(cxplayer_t *cxplayer);

/**
 * @brief  resume the cxplayer
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_resume(cxplayer_t *cxplayer);

/**
 * @brief  stop the cxplayer
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_stop(cxplayer_t *cxplayer);

/**
 * @brief  free/destroy the cxplayer obj
 * @param  [in] cxplayer
 * @return 0/-1
 */
int cxplayer_free(cxplayer_t *cxplayer);

/**
 * @brief  seek to the time
 * @param  [in] cxplayer
 * @param  [in] timestamp : seek time
 * @return 0/-1
 */
int cxplayer_seek(cxplayer_t *cxplayer, uint64_t timestamp);

/**
 * @brief  set start play time before play
 * @param  [in] cxplayer
 * @param  [in] start_time : start play time
 * @return 0/-1
 */
int cxplayer_set_start_time(cxplayer_t *cxplayer, uint64_t start_time);

/**
 * @brief  get play time(current time & duration, ms)
 * @param  [in] cxplayer
 * @param  [in/out] time
 * @return 0/-1
 */
int cxplayer_get_time(cxplayer_t *cxplayer, xplay_time_t *time);

/**
 * @brief  get media info
 * @param  [in] cxplayer
 * @param  [in/out] minfo : need free by the caller(use media_info_uninit function)
 * @return 0/-1
 */
int cxplayer_get_media_info(cxplayer_t *cxplayer, xmedia_info_t *minfo);

/**
 * @brief  get soft vol index of the cxplayer
 * @param  [in] cxplayer
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int cxplayer_get_vol(cxplayer_t *cxplayer, uint8_t *vol);

/**
 * @brief  set soft vol index of the cxplayer
 * @param  [in] cxplayer
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int cxplayer_set_vol(cxplayer_t *cxplayer, uint8_t vol);

/**
 * @brief  get play speed of the cxplayer
 * @param  [in] cxplayer
 * @param  [out] speed
 * @return 0/-1
 */
int cxplayer_get_speed(cxplayer_t *cxplayer, float *speed);

/**
 * @brief  set play speed of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] speed : 0.5 ~ 2.0 for audio, etc
 * @return 0/-1
 */
int cxplayer_set_speed(cxplayer_t *cxplayer, float speed);

/**
 * @brief  mute/unmute the cxplayer
 * @param  [in] cxplayer
 * @param  [in] mute : mute flag
 * @return 0/-1
 */
int cxplayer_set_mute(cxplayer_t *cxplayer, uint8_t mute);

/**
 * @brief  get play source url
 * @param  [in] cxplayer
 * @return NULL on error
 */
const char* cxplayer_get_url(cxplayer_t *cxplayer);

/**
 * @brief  switch audio track of the player
 * @param  [in] cxplayer
 * @param  [in] idx : index of audios
 * @return 0/-1
 */
int cxplayer_switch_audio_track(cxplayer_t *cxplayer, uint8_t idx);

/**
 * @brief  switch subtitle track of the player
 * @param  [in] cxplayer
 * @param  [in] idx : index of subtitles
 * @return 0/-1
 */
int cxplayer_switch_subtitle_track(cxplayer_t *cxplayer, uint8_t idx);

/**
 * @brief  set external subtitle url of the player
 * @param  [in] cxplayer
 * @param  [in] url : url of the external subtitle
 * @return 0/-1
 */
int cxplayer_set_subtitle_url(cxplayer_t *cxplayer, const char *url);

/**
 * @brief  show/hide subtitle of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int cxplayer_set_subtitle_visible(cxplayer_t *cxplayer, uint8_t visible);

/**
 * @brief  show/hide video of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int cxplayer_set_video_visible(cxplayer_t *cxplayer, uint8_t visible);

/**
 * @brief  crop video of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] win : crop window
 * @return 0/-1
 */
int cxplayer_set_video_crop(cxplayer_t *cxplayer, const cxwindow_t *win);

/**
 * @brief  set display window of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] win : display window
 * @return 0/-1
 */
int cxplayer_set_display_window(cxplayer_t *cxplayer, const cxwindow_t *win);

/**
 * @brief  enable/disable fullscreen for the cxplayer
 * @param  [in] cxplayer
 * @param  [in] onoff
 * @return 0/-1
 */
int cxplayer_set_fullscreen(cxplayer_t *cxplayer, uint8_t onoff);

/**
 * @brief  set display format for the cxplayer
 * @param  [in] cxplayer
 * @param  [in] format
 * @return 0/-1
 */
int cxplayer_set_display_format(cxplayer_t *cxplayer, cxdisplay_format_t format);

/**
 * @brief  rotate video of the cxplayer
 * @param  [in] cxplayer
 * @param  [in] type : rotate type
 * @return 0/-1
 */
int cxplayer_set_video_rotate(cxplayer_t *cxplayer, cxrotate_type_t type);

__END_DECLS__

#endif /* __CXPLAYER_H__ */


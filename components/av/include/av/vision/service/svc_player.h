/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __SVC_PLAYER_H__
#define __SVC_PLAYER_H__

#include "av/avutil/av_config.h"
#include "svc_player/svc_player_typedef.h"

__BEGIN_DECLS__

typedef struct svc_player_cls svc_player_cls_t;

/**
 * @brief  regist svc_player class
 * @param  [in] ops
 * @return 0/-1
 */
int svc_player_cls_register(const svc_player_cls_t *cls);

/**
* @brief  get the default config param
* @param  [in] conf
* @return 0/-1
*/
int svc_player_global_config_init(player_service_cnf_t *conf);

/**
 * @brief  init player service(init once)
 * @param  [in] conf : config params
 * @return 0/-1
 */
int svc_player_global_init(const player_service_cnf_t *conf);

/**
 * @brief  alloc one player id by player-service
 * @return -1 on error
 */
int svc_player_alloc_pid();

/**
 * @brief  init config param for one player
 * @param  [in/out] conf
 * @return 0/-1
 */
int svc_player_config_init(svc_player_cnf_t *conf);

/**
 * @brief  svc_player play interface
 * @param  [in] svc_player
 * @param  [in] url : example: http://ip:port/xx.mp3
 * @return 0/-1
 */
int svc_player_play(int pid, const char *url, const svc_player_cnf_t *conf);

/**
 * @brief  pause the svc_player
 * @param  [in] svc_player
 * @return 0/-1
 */
int svc_player_pause(int pid);

/**
 * @brief  resume the svc_player
 * @param  [in] svc_player
 * @return 0/-1
 */
int svc_player_resume(int pid);

/**
 * @brief  stop the svc_player
 * @param  [in] svc_player
 * @return 0/-1
 */
int svc_player_stop(int pid);

/**
 * @brief  free/destroy the svc_player obj
 * @param  [in] svc_player
 * @return 0/-1
 */
int svc_player_free_pid(int pid);

/**
 * @brief  seek to the time
 * @param  [in] svc_player
 * @param  [in] timestamp : seek time
 * @return 0/-1
 */
int svc_player_seek(int pid, uint64_t timestamp);

/**
 * @brief  set start play time before play
 * @param  [in] svc_player
 * @param  [in] start_time : start play time
 * @return 0/-1
 */
int svc_player_set_start_time(int pid, uint64_t start_time);

/**
 * @brief  get play time(current time & duration, ms)
 * @param  [in] svc_player
 * @param  [in/out] time
 * @return 0/-1
 */
int svc_player_get_time(int pid, xplay_time_t *time);

/**
 * @brief  get media info
 * @param  [in] svc_player
 * @param  [in/out] minfo : need free by the caller(use media_info_uninit function)
 * @return 0/-1
 */
int svc_player_get_media_info(int pid, xmedia_info_t *minfo);

/**
 * @brief  get soft vol index of the svc_player
 * @param  [in] svc_player
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int svc_player_get_vol(int pid, uint8_t *vol);

/**
 * @brief  set soft vol index of the svc_player
 * @param  [in] svc_player
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int svc_player_set_vol(int pid, uint8_t vol);

/**
 * @brief  get play speed of the svc_player
 * @param  [in] svc_player
 * @param  [out] speed
 * @return 0/-1
 */
int svc_player_get_speed(int pid, float *speed);

/**
 * @brief  set play speed of the svc_player
 * @param  [in] svc_player
 * @param  [in] speed : 0.5 ~ 2.0 for audio, etc
 * @return 0/-1
 */
int svc_player_set_speed(int pid, float speed);

/**
 * @brief  mute/unmute the svc_player
 * @param  [in] svc_player
 * @param  [in] mute : mute flag
 * @return 0/-1
 */
int svc_player_set_mute(int pid, uint8_t mute);

/**
 * @brief  get play source url
 * @param  [in] svc_player
 * @return NULL on error
 */
const char* svc_player_get_url(int pid);

/**
 * @brief  switch audio track of the player
 * @param  [in] svc_player
 * @param  [in] idx : index of audios
 * @return 0/-1
 */
int svc_player_switch_audio_track(int pid, uint8_t idx);

/**
 * @brief  switch subtitle track of the player
 * @param  [in] svc_player
 * @param  [in] idx : index of subtitles
 * @return 0/-1
 */
int svc_player_switch_subtitle_track(int pid, uint8_t idx);

/**
 * @brief  set external subtitle url of the player
 * @param  [in] svc_player
 * @param  [in] url : url of the external subtitle
 * @return 0/-1
 */
int svc_player_set_subtitle_url(int pid, const char *url);

/**
 * @brief  show/hide subtitle of the svc_player
 * @param  [in] svc_player
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int svc_player_set_subtitle_visible(int pid, uint8_t visible);

/**
 * @brief  show/hide video of the svc_player
 * @param  [in] svc_player
 * @param  [in] visible : show/hide
 * @return 0/-1
 */
int svc_player_set_video_visible(int pid, uint8_t visible);

/**
 * @brief  crop video of the svc_player
 * @param  [in] svc_player
 * @param  [in] win : crop window
 * @return 0/-1
 */
int svc_player_set_video_crop(int pid, const xwindow_t *win);

/**
 * @brief  set display window of the svc_player
 * @param  [in] svc_player
 * @param  [in] win : display window
 * @return 0/-1
 */
int svc_player_set_display_window(int pid, const xwindow_t *win);

/**
 * @brief  enable/disable fullscreen for the svc_player
 * @param  [in] svc_player
 * @param  [in] onoff
 * @return 0/-1
 */
int svc_player_set_fullscreen(int pid, uint8_t onoff);

/**
 * @brief  set display format for the svc_player
 * @param  [in] svc_player
 * @param  [in] format
 * @return 0/-1
 */
int svc_player_set_display_format(int pid, xdisplay_format_t format);

/**
 * @brief  rotate video of the svc_player
 * @param  [in] svc_player
 * @param  [in] type : rotate type
 * @return 0/-1
 */
int svc_player_set_video_rotate(int pid, xrotate_type_t type);

__END_DECLS__

#endif /* __SVC_PLAYER_H__ */


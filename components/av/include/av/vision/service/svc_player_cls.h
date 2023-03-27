/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __SVC_PLAYER_CLS_H__
#define __SVC_PLAYER_CLS_H__

#include "av/avutil/common.h"
#include "svc_player/svc_player_typedef.h"
#ifdef __linux__
#include "aos_port/list.h"
#endif

__BEGIN_DECLS__

typedef struct svc_player_struct svc_player_t;
typedef struct svc_player_cls svc_player_cls_t;

struct svc_player_struct  {
    //TODO:
    struct svc_player_cls        *cls;
    aos_mutex_t                  lock;
};

#define GET_DERIVED_PLAYER_SVC(p) (((char*)(p)) + sizeof(svc_player_t))

struct svc_player_ops  {
    int   (*init)                   (svc_player_t *svc, int pid);
    int   (*play)                   (svc_player_t *svc, int pid, const char *url, const svc_player_cnf_t *conf);
    int   (*pause)                  (svc_player_t *svc, int pid);
    int   (*resume)                 (svc_player_t *svc, int pid);
    int   (*seek)                   (svc_player_t *svc, int pid, uint64_t timestamp);
    int   (*stop)                   (svc_player_t *svc, int pid);
    void  (*uninit)                 (svc_player_t *svc, int pid);

    int   (*get_media_info)         (svc_player_t *svc, int pid, xmedia_info_t *minfo);
    int   (*get_time)               (svc_player_t *svc, int pid, xplay_time_t *time);
    int   (*set_speed)              (svc_player_t *svc, int pid, float speed);
    int   (*set_vol)                (svc_player_t *svc, int pid, uint8_t vol);
    int   (*set_mute)               (svc_player_t *svc, int pid, uint8_t mute);

#if !CONFIG_AV_AUDIO_ONLY_SUPPORT
    int   (*switch_audio_track)     (svc_player_t *svc, int pid, uint8_t idx);
    int   (*switch_subtitle_track)  (svc_player_t *svc, int pid, uint8_t idx);
    int   (*set_subtitle_url)       (svc_player_t *svc, int pid, const char *url);
    int   (*set_subtitle_visible)   (svc_player_t *svc, int pid, uint8_t visible);
    int   (*set_video_visible)      (svc_player_t *svc, int pid, uint8_t visible);
    int   (*set_video_crop)         (svc_player_t *svc, int pid, const xwindow_t *win);
    int   (*set_fullscreen)         (svc_player_t *svc, int pid, uint8_t onoff);
    int   (*set_display_window)     (svc_player_t *svc, int pid, const xwindow_t *win);
    int   (*set_display_format)     (svc_player_t *svc, int pid, xdisplay_format_t format);
    int   (*set_video_rotate)       (svc_player_t *svc, int pid, xrotate_type_t type);
#endif
};

struct svc_player_cls  {
    const char                   *name;
    size_t                       priv_size;
    const struct svc_player_ops  *ops;
};

__END_DECLS__

#endif /* __SVC_PLAYER_CLS_H__ */


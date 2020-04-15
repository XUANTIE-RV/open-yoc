/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <devices/driver.h>

#ifndef __AOS_MIXER__
#define __AOS_MIXER__

#include <alsa/pcm.h>
#include <alsa/snd.h>
//控制发送信号和控制声音大小
#ifdef __cplusplus
extern "C" {
#endif


/** Mixer simple element channel identifier */
typedef enum _aos_mixer_selem_channel_id {
    /** Unknown */
    AOS_MIXER_SCHN_UNKNOWN = -1,
    /** Front left */
    AOS_MIXER_SCHN_FRONT_LEFT = 0,
    /** Front right */
    AOS_MIXER_SCHN_FRONT_RIGHT,
    /** Rear left */
    AOS_MIXER_SCHN_REAR_LEFT,
    /** Rear right */
    AOS_MIXER_SCHN_REAR_RIGHT,
    /** Front center */
    AOS_MIXER_SCHN_FRONT_CENTER,
    /** Woofer */
    AOS_MIXER_SCHN_WOOFER,
    /** Side Left */
    AOS_MIXER_SCHN_SIDE_LEFT,
    /** Side Right */
    AOS_MIXER_SCHN_SIDE_RIGHT,
    /** Rear Center */
    AOS_MIXER_SCHN_REAR_CENTER,
    AOS_MIXER_SCHN_LAST = 31,
    /** Mono (Front left alias) */
    AOS_MIXER_SCHN_MONO = AOS_MIXER_SCHN_FRONT_LEFT
} aos_mixer_selem_channel_id_t;

enum aos_mixer_selem_regopt_abstract {
    AOS_MIXER_SABSTRACT_NONE = 0,
    AOS_MIXER_SABSTRACT_BASIC,
};

struct aos_mixer_selem_regopt {
    int ver;
    enum aos_mixer_selem_regopt_abstract abstract;
    const char *device;
    aos_pcm_t *playback_pcm;
    aos_pcm_t *capture_pcm;
} aos_mixer_selem_regopt_t;

typedef struct elem_inter {
    int cur;
    int min;
    int max;
} elem_inter_t;

typedef struct aos_mixer aos_mixer_t;
typedef struct _aos_mixer_elem aos_mixer_elem_t;

struct aos_mixer {
    const char *card_name;
    card_dev_t *card;
    aos_mixer_elem_t *cur_elem;
    slist_t elems_head;
};

struct _aos_mixer_elem {
    const char *name;
    elem_inter_t vol_l;
    elem_inter_t vol_r;
    elem_inter_t dB_l;
    elem_inter_t dB_r;
    aos_mixer_t *mixer;
    aos_mutex_t mutex;
    void *hdl;
    struct sm_elem_ops *ops;
    slist_t next;
};

typedef struct sm_elem_ops {
    // int (*open)(aos_mixer_elem_t *elem);
    // int (*is)(aos_mixer_elem_t *elem, int dir, int cmd, int val);
    // int (*get_range)(aos_mixer_elem_t *elem, int dir, int *min, int *max);
    // int (*set_range)(aos_mixer_elem_t *elem, int dir, int min, int max);
    // int (*get_dB_range)(aos_mixer_elem_t *elem, int dir, int *min, int *max);
    // int (*ask_vol_dB)(aos_mixer_elem_t *elem, int dir, int value, int *dbValue);
    // int (*ask_dB_vol)(aos_mixer_elem_t *elem, int dir, int dbValue, int *value, int xdir);
    // int (*get_volume)(aos_mixer_elem_t *elem, int dir, aos_mixer_selem_channel_id_t channel, int *value);
    // int (*get_dB)(aos_mixer_elem_t *elem, int dir, aos_mixer_selem_channel_id_t channel, int *value);
    // int (*set_volume)(aos_mixer_elem_t *elem, int dir, aos_mixer_selem_channel_id_t channel, int value);
    int (*set_dB)(aos_mixer_elem_t *elem, int l_dB, int r_dB);
    int (*volume_to_dB)(aos_mixer_elem_t *elem, int value);
    // int (*get_switch)(aos_mixer_elem_t *elem, int dir, aos_mixer_selem_channel_id_t channel, int *value);
    // int (*set_switch)(aos_mixer_elem_t *elem, int dir, aos_mixer_selem_channel_id_t channel, int value);
    // int (*enum_item_name)(aos_mixer_elem_t *elem, unsigned int item, size_t maxlen, char *buf);
    // int (*get_enum_item)(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, unsigned int *itemp);
    // int (*set_enum_item)(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, unsigned int item);
} sm_elem_ops_t;

int aos_mixer_open(aos_mixer_t **mixer, int mode);
int aos_mixer_close(aos_mixer_t *mixer);
int aos_mixer_selem_set_playback_volume(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, int value);
int aos_mixer_selem_set_capture_volume(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, int value);
int aos_mixer_selem_set_playback_volume_all(aos_mixer_elem_t *elem, int value);
int aos_mixer_selem_set_capture_volume_all(aos_mixer_elem_t *elem, int value);
int aos_mixer_selem_set_playback_volume_range(aos_mixer_elem_t *elem, int min, int max);
int aos_mixer_selem_set_capture_volume_range(aos_mixer_elem_t *elem, int min, int max);
int aos_mixer_selem_get_playback_volume(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, int *value);
int aos_mixer_load(aos_mixer_t *mixer);
int snd_elem_new(aos_mixer_elem_t **elem_ret, const char *name, sm_elem_ops_t *ops);
void aos_mixer_free(aos_mixer_t *mixer);
aos_mixer_elem_t *aos_mixer_first_elem(aos_mixer_t *mixer);
aos_mixer_elem_t *aos_mixer_last_elem(aos_mixer_t *mixer);
aos_mixer_elem_t *aos_mixer_elem_next(aos_mixer_elem_t *elem);
aos_mixer_elem_t *aos_mixer_elem_prev(aos_mixer_elem_t *elem);
int aos_mixer_attach(aos_mixer_t *mixer, const char *name);
// void aos_mixer_set_callback(aos_mixer_t *obj, aos_mixer_callback_t val);
// void aos_mixer_set_callback_private(aos_mixer_t *mixer, void * val);
// void * aos_mixer_get_callback_private(const aos_mixer_t *mixer);
// unsigned int aos_mixer_get_count(const aos_mixer_t *mixer);
// void aos_mixer_elem_set_callback(aos_mixer_elem_t *mixer,
//               aos_mixer_elem_callback_t val);
// void aos_mixer_elem_set_callback_private(aos_mixer_elem_t *mixer, void * val);
// void * aos_mixer_elem_get_callback_private(const aos_mixer_elem_t *mixer);
// aos_mixer_elem_type_t aos_mixer_elem_get_type(const aos_mixer_elem_t *mixer);
// int aos_mixer_set_compare(aos_mixer_t *mixer, aos_mixer_compare_t compare);

#ifdef __cplusplus
}
#endif

#endif
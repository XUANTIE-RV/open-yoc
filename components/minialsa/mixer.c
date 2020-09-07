/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <aos/aos.h>
#include <alsa/mixer.h>
#include <alsa/snd.h>

#define TAG "mixer"

#define ELEM_LOCK(elem) aos_mutex_lock(&elem->mutex, AOS_WAIT_FOREVER)
#define ELEM_UNLOCK(elem) aos_mutex_unlock(&elem->mutex)

int aos_mixer_open(aos_mixer_t **mixer_ret, int mode)
{
    aos_mixer_t *mixer;

    mixer = aos_zalloc_check(sizeof(aos_mixer_t));

    *mixer_ret = mixer;
    return 0;
}

int aos_mixer_close(aos_mixer_t *mixer)
{
    aos_free(mixer);

    return 0;
}

int aos_mixer_attach(aos_mixer_t *mixer, const char *card_name)
{
    card_dev_t *card;

    aos_card_attach(card_name, &card);
    if (card == NULL) {
        return -1;
    }
    mixer->card = card;

    return 0;
}

static void elem_inter_init(elem_inter_t *inter, int cur, int min, int max)
{
    inter->cur = cur;
    inter->min = min;
    inter->max = max;
}

int snd_elem_new(aos_mixer_elem_t **elem_ret, const char *name, sm_elem_ops_t *ops)
{
    aos_mixer_elem_t *elem = aos_malloc_check(sizeof(aos_mixer_elem_t));

    elem->name = name;
    elem->ops = ops;
    elem_inter_init(&elem->vol_l, 50, 0, 100);
    elem_inter_init(&elem->vol_r, 50, 0, 100);
    // elem->dB_l.cur = elem->dB_r.cur = ops->volume_to_dB(elem, elem->vol_r.cur);
    // ops->set_dB(elem, elem->dB_l.cur, elem->dB_r.cur);
    aos_mutex_new(&elem->mutex);

    *elem_ret = elem;
    return 0;
}

int aos_mixer_selem_register(aos_mixer_t *mixer, struct aos_mixer_selem_regopt *options, void **classp)
{

    return 0;
}

int aos_mixer_load(aos_mixer_t *mixer)
{
    aos_check_return_einval(mixer && mixer->card);
    snd_card_drv_t *drv = (snd_card_drv_t *)mixer->card->device.drv;

    mixer->elems_head = drv->mixer_head;

    return 0;
}

aos_mixer_elem_t *aos_mixer_first_elem(aos_mixer_t *mixer)
{
    aos_check_return_val(mixer && mixer->card, NULL);

    aos_mixer_elem_t *elem;

    elem = slist_first_entry(&mixer->elems_head, aos_mixer_elem_t, next);
    mixer->cur_elem = elem;

    return elem;
}

aos_mixer_elem_t *aos_mixer_last_elem(aos_mixer_t *mixer)
{
    aos_check_return_val(mixer && mixer->card, NULL);

    aos_mixer_elem_t *elem;

    slist_for_each_entry(&mixer->elems_head, elem, aos_mixer_elem_t, next);
    mixer->cur_elem = elem;

    return elem;
}

aos_mixer_elem_t *aos_mixer_elem_next(aos_mixer_elem_t *elem_p)
{
    aos_check_return_val(elem_p, NULL);

    aos_mixer_elem_t *elem = elem_p->mixer->cur_elem;

    elem = slist_entry(&elem_p->next, aos_mixer_elem_t, next);

    return elem;
}

aos_mixer_elem_t *aos_mixer_elem_prev(aos_mixer_elem_t *elem_p)
{
    aos_check_return_val(elem_p, NULL);

    aos_mixer_elem_t *elem = NULL;

    return elem;
}

static int _volum_to_dB(aos_mixer_elem_t *elem, int volum)
{
    ELEM_LOCK(elem);
    int dB = elem->ops->volume_to_dB(elem, volum);
    ELEM_UNLOCK(elem);

    return dB;
}

static int _aos_mixer_elem_volume(aos_mixer_elem_t *elem, int l_dB, int r_dB)
{
    ELEM_LOCK(elem);
    elem->ops->set_dB(elem, l_dB, r_dB);
    ELEM_UNLOCK(elem);
    return 0;
}

int aos_mixer_selem_set_playback_volume(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, int value)
{
    aos_check_return_einval(elem);

    int dB = _volum_to_dB(elem, value);
    int ret = -1;

    if(AOS_MIXER_SCHN_FRONT_LEFT == channel) {
        ret = _aos_mixer_elem_volume(elem, dB, elem->dB_r.cur);
        if(ret == 0) {
            elem->dB_l.cur = dB;
            elem->vol_l.cur = value;
        }
    } else if(AOS_MIXER_SCHN_FRONT_RIGHT == channel) {
        ret = _aos_mixer_elem_volume(elem, elem->dB_l.cur, dB);
        if(ret == 0) {
            elem->dB_r.cur = dB;
            elem->vol_r.cur = value;
        }
    }

    return ret;
}

int aos_mixer_selem_set_capture_volume(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, int value)
{
    aos_check_return_einval(elem);

    return 0;
}

int aos_mixer_selem_set_playback_volume_all(aos_mixer_elem_t *elem, int value)
{
    aos_check_return_einval(elem);

    int dB = _volum_to_dB(elem, value);

    int ret = _aos_mixer_elem_volume(elem, dB, dB);
    if(ret == 0) {
        elem->dB_l.cur = elem->dB_r.cur = dB;
        elem->vol_l.cur = elem->vol_r.cur = value;
    }

    return ret;
}

int aos_mixer_selem_set_capture_volume_all(aos_mixer_elem_t *elem, int value)
{
    aos_check_return_einval(elem);

    return 0;
}

int aos_mixer_selem_get_playback_volume(aos_mixer_elem_t *elem, aos_mixer_selem_channel_id_t channel, int *value)
{
    aos_check_return_einval(elem && value);

    if(AOS_MIXER_SCHN_FRONT_LEFT == channel) {
        *value = elem->vol_l.cur;
    } else if(AOS_MIXER_SCHN_FRONT_RIGHT == channel) {
        *value = elem->vol_r.cur;
    }

    return 0;
}
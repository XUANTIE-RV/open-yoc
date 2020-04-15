/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <aos/aos.h>
#include <alsa/pcm.h>
#include <alsa/snd.h>

#define TAG "pcm"

#define PCM_LOCK(pcm) aos_mutex_lock(&pcm->mutex, AOS_WAIT_FOREVER)
#define PCM_UNLOCK(pcm) aos_mutex_unlock(&pcm->mutex)

#define hw_params(pcm) pcm->hw_params
#define sw_params(pcm) pcm->sw_params

typedef struct {
    char *channel;
    int offset;
} pcm_access_t;

static void pcm_event(aos_pcm_t *pcm, int event_id, void *priv)
{
    aos_event_set(&pcm->evt, event_id, AOS_EVENT_OR);
}

int aos_pcm_new(aos_pcm_t **pcm_ret, int type, const char *name, aos_pcm_stream_t stream, int mode)
{
    aos_pcm_t *pcm = aos_calloc_check(sizeof(aos_pcm_t), 1);

    pcm->pcm_name = name;
    pcm->mode = mode;
    pcm->stream = stream;
    *pcm_ret = pcm;

    return 0;
}

int aos_pcm_open(aos_pcm_t **pcm_ret, const char *name, aos_pcm_stream_t stream, int mode)
{
    aos_pcm_t *pcm;
    card_dev_t *card;

    snd_card_attach("card0", &card); // Do not put it behind of " (aos_pcm_dev_t *)device_open(name)"

    if (card == NULL) {
        *pcm_ret = NULL;
        return -1;
    }

    aos_pcm_dev_t *dev = (aos_pcm_dev_t *)device_open(name);

    if(dev == NULL) {
        *pcm_ret = NULL;
        return -1;
    }

    pcm = &dev->pcm;

    aos_mutex_new(&pcm->mutex);
    aos_event_new(&pcm->evt, 0);
    pcm->stream = stream;
    if(mode == 0) {
        pcm->event.cb = pcm_event;
        pcm->event.priv = NULL;
    }
    pcm->mode   = mode;
    ringbuffer_create(&pcm->ringbuffer, aos_malloc_check(1024), 1024);
    pcm->state = AOS_PCM_STATE_OPEN;
    pcm->pcm_name = name;
    *pcm_ret = pcm;
    return 0;
}

int aos_pcm_close(aos_pcm_t *pcm)
{
    aos_check_return_einval(pcm);

    //LOGE(TAG, "pcm close");
    aos_mutex_free(&pcm->mutex);
    aos_event_free(&pcm->evt);

    if (pcm->hw_params) {
        aos_free(pcm->hw_params);
    }

    if (pcm->sw_params) {
        aos_free(pcm->sw_params);
    }

    aos_free(pcm->ringbuffer.buffer);
    ringbuffer_destroy(&pcm->ringbuffer);

    aos_dev_t *dev = (aos_dev_t *)(((int)(pcm)) + sizeof(aos_pcm_t) - sizeof(aos_pcm_dev_t));

    device_close(dev);

    return 0;
}

int aos_pcm_hw_params_any(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    aos_check_return_einval(pcm);

    params->access      = AOS_PCM_ACCESS_RW_INTERLEAVED;
    params->channels    = 2;
    params->rate        = 16000;
    params->sample_bits = 16;

    pcm->hw_params = params;
    return 0;
}

int aos_pcm_hw_params_set_access(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, aos_pcm_access_t _access)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->access = _access;

    return 0;
}

int aos_pcm_hw_params_set_format(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, int val)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->format = val;

    return 0;
}

int aos_pcm_hw_params_set_channels(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, unsigned int val)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->channels = val;

    return 0;
}

int aos_pcm_hw_params_set_rate_near(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->rate = *val;

    return 0;
}

int aos_pcm_hw_params_set_buffer_time_near(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->buffer_time = *val;

    return 0;
}

int aos_pcm_hw_params_set_buffer_size_near(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, aos_pcm_uframes_t *val)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->buffer_size = *val;

    return 0;
}

int aos_pcm_hw_params_set_period_size_near(aos_pcm_t *pcm, aos_pcm_hw_params_t *params, aos_pcm_uframes_t *val, int *dir)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->period_size = *val;

    return 0;
}

int aos_pcm_set_params(aos_pcm_t *pcm, int format, aos_pcm_access_t acc, unsigned int channels, unsigned int rate, int soft_resample, unsigned int latency)
{
    aos_check_return_einval(pcm);

    hw_params(pcm)->format = format;
    hw_params(pcm)->access = acc;
    hw_params(pcm)->channels = channels;
    hw_params(pcm)->rate = rate;

    PCM_LOCK(pcm);
    pcm->ops->hw_params_set(pcm, pcm->hw_params);
    pcm->state = AOS_PCM_STATE_PREPARED;
    PCM_UNLOCK(pcm);

    return 0;
}

int aos_pcm_hw_params(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    aos_check_return_einval(pcm && params);

    hw_params(pcm)->period_bytes = (hw_params(pcm)->period_size * ( hw_params(pcm)->format / 8)) * hw_params(pcm)->channels;
    hw_params(pcm)->buffer_bytes = (hw_params(pcm)->buffer_size * ( hw_params(pcm)->format / 8)) * hw_params(pcm)->channels;

    PCM_LOCK(pcm);
    pcm->ops->hw_params_set(pcm, params);
    pcm->state = AOS_PCM_STATE_PREPARED;
    PCM_UNLOCK(pcm);

    return 0;
}

int aos_pcm_sw_params_current(aos_pcm_t *pcm, aos_pcm_sw_params_t *params)
{
    aos_check_return_einval(pcm && params);

    pcm->sw_params = params;
    return 0;
}

int aos_pcm_sw_params_set_start_threshold(aos_pcm_t *pcm, aos_pcm_sw_params_t *params, aos_pcm_uframes_t val)
{
    aos_check_return_einval(pcm && params);

    sw_params(pcm)->start_threshold = val;

    return 0;
}

int aos_pcm_sw_params_set_stop_threshold(aos_pcm_t *pcm, aos_pcm_sw_params_t *params, aos_pcm_uframes_t val)
{
    aos_check_return_einval(pcm && params);

    sw_params(pcm)->stop_threshold = val;

    return 0;
}

int aos_pcm_sw_params(aos_pcm_t *pcm, aos_pcm_sw_params_t *params)
{
    aos_check_return_einval(pcm && params);

    return 0;
}

aos_pcm_sframes_t aos_pcm_writei(aos_pcm_t *pcm, const void *buffer, aos_pcm_uframes_t size)
{
    aos_check_return_einval(pcm && pcm->stream == AOS_PCM_STREAM_PLAYBACK && \
                            pcm->hw_params->access == AOS_PCM_ACCESS_RW_INTERLEAVED);

    PCM_LOCK(pcm);

    int ret = -1;
    int w_size = aos_pcm_frames_to_bytes(pcm, size);
    unsigned int actl_flags = 0;
    char *send = (char *)buffer;

    while (w_size) {
        ret = pcm->ops->write(pcm, (void *)(send), w_size);
        if (ret < w_size) {
            aos_event_get(&pcm->evt, PCM_EVT_WRITE | PCM_EVT_XRUN, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
            if ((actl_flags | PCM_EVT_XRUN) == PCM_EVT_XRUN) {
                printf("pcm write xrun\n");
            }
        }

        w_size -= ret;
        send += ret;
    }

    PCM_UNLOCK(pcm);

    return ret;
}

static void pcm_access(aos_pcm_t *pcm, void *buffer, int bytes)
{
    char *recv = aos_malloc_check(bytes);
    pcm_access_t *c = aos_malloc_check(pcm->hw_params->channels * sizeof(pcm_access_t));
    int channel_size = bytes / pcm->hw_params->channels;
    int frame_size = pcm->hw_params->format / 8;

    memcpy(recv, buffer, bytes);
    for (int i = 0; i < pcm->hw_params->channels; i++) {
        (c+i)->channel = recv + channel_size * i;
        (c+i)->offset  = 0;
    }

    char *dec = (char *)buffer;
    int offset = 0;

    while (offset < bytes) {
        for (int j = 0; j < pcm->hw_params->channels; j++) {
            pcm_access_t *p = (c + j);

            memcpy(dec + offset, p->channel + p->offset, frame_size);
            offset += frame_size;
            p->offset += frame_size;
        }
    }

    aos_free(recv);
    aos_free(c);
}

aos_pcm_sframes_t aos_pcm_readi(aos_pcm_t *pcm, void *buffer, aos_pcm_uframes_t size)
{
    aos_check_return_einval(pcm && pcm->stream == AOS_PCM_STREAM_CAPTURE && \
                            pcm->hw_params->access == AOS_PCM_ACCESS_RW_INTERLEAVED);

    PCM_LOCK(pcm);
    int bytes = pcm->ops->read(pcm, buffer, aos_pcm_frames_to_bytes(pcm, size));
    PCM_UNLOCK(pcm);

    pcm_access(pcm, buffer, aos_pcm_frames_to_bytes(pcm, size));

    return (aos_pcm_bytes_to_frames(pcm, bytes));
}

aos_pcm_sframes_t aos_pcm_readn(aos_pcm_t *pcm, void **bufs, aos_pcm_uframes_t size)
{
    aos_check_return_einval(pcm && pcm->stream == AOS_PCM_STREAM_CAPTURE && \
                            pcm->hw_params->access == AOS_PCM_ACCESS_RW_NONINTERLEAVED);

    PCM_LOCK(pcm);
    int bytes = pcm->ops->read(pcm, (void *)bufs, aos_pcm_frames_to_bytes(pcm, size));
    PCM_UNLOCK(pcm);

    return (aos_pcm_bytes_to_frames(pcm, bytes));
}

int aos_pcm_hw_params_alloca(aos_pcm_hw_params_t **p)
{
    *p = aos_zalloc_check(sizeof(aos_pcm_hw_params_t));

    return 0;
}

int aos_pcm_sw_params_alloca(aos_pcm_sw_params_t **p)
{
    *p = aos_malloc_check(sizeof(aos_pcm_sw_params_t));

    return 0;
}

int aos_pcm_drop(aos_pcm_t *pcm)
{

    return 0;
}

int aos_pcm_drain(aos_pcm_t *pcm)
{
    //FIXME
    aos_check_return_einval(pcm);

    pcm->state = AOS_PCM_STATE_DRAINING;
    if (pcm->state == AOS_PCM_STATE_DRAINING && pcm->hw_params != NULL && pcm->stream == AOS_PCM_STREAM_PLAYBACK) {
        if (pcm->ops->hw_get_remain_size) {
            int size;

            while(1) {
                size = pcm->ops->hw_get_remain_size(pcm);

                if (size) {
                    int lli_size;
                    aos_pcm_hw_params_t *params = pcm->hw_params;
                    lli_size = (params->rate / 1000) * (params->sample_bits / 8) * params->channels;
                    aos_msleep((size / lli_size) + 1);
                } else {
                    break;
                }
            }
        } else {
            unsigned int actl_flags;
            
            aos_event_get(&pcm->evt, PCM_EVT_XRUN, AOS_EVENT_OR_CLEAR, &actl_flags, AOS_WAIT_FOREVER);
        }
    }

    return 0;
}

int aos_pcm_pause(aos_pcm_t *pcm, int enable)
{
    aos_check_return_einval(pcm);

    PCM_LOCK(pcm);
    pcm->ops->pause(pcm, enable);
    PCM_UNLOCK(pcm);

    return 0;
}

int aos_pcm_wait(aos_pcm_t *pcm, int timeout)
{
    aos_check_return_einval(pcm);
    unsigned int actl_flags;

    PCM_LOCK(pcm);
    aos_event_get(&pcm->evt, PCM_EVT_READ | PCM_EVT_XRUN, AOS_EVENT_OR_CLEAR, &actl_flags, timeout);
    PCM_UNLOCK(pcm);

    return 0;
}

aos_pcm_sframes_t aos_pcm_bytes_to_frames(aos_pcm_t *pcm, ssize_t bytes)
{
    aos_check_return_einval(pcm);

    return (bytes / (hw_params(pcm)->channels * hw_params(pcm)->format / 8));
}

ssize_t aos_pcm_frames_to_bytes(aos_pcm_t *pcm, aos_pcm_sframes_t frames)
{
    aos_check_return_einval(pcm);

    return (frames * (hw_params(pcm)->channels * hw_params(pcm)->format / 8));
}

void aos_pcm_set_ops(aos_pcm_t *pcm, int direction, struct aos_pcm_ops *ops)
{
    aos_check_param(pcm && ops);

    pcm->ops = ops;
}

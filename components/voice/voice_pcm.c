/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <ipc.h>
#include <csi_core.h>
#include <drv/codec.h>
#include <alsa/pcm.h>
#include <aos/list.h>

#include <voice_def.h>

#define TAG                    "voice"
struct __voice_pcm {
    void                    *priv;
    voice_capture_t         *mic;
    voice_capture_t         *ref;
    void                    *data;
    int                      len;
    voice_pcm_send           pcm_send;
};

static aos_pcm_t *_param_init(voice_pcm_t *vpcm, char *name, voice_pcm_param_t *p)
{
    if (name == NULL || p == NULL) {
        return NULL;
    }

    aos_pcm_hw_params_t *params;
    aos_pcm_t *pcm;
    int err, period_frames, buffer_frames;

    aos_pcm_open(&pcm, name, AOS_PCM_STREAM_CAPTURE, 0);

    aos_pcm_hw_params_alloca(&params);
    err = aos_pcm_hw_params_any(pcm, params);

    if (err < 0) {
        LOGD(TAG, "Broken configuration for this PCM: no configurations available");
    }

    err = aos_pcm_hw_params_set_access(pcm, params, p->access == 0 ? AOS_PCM_ACCESS_RW_NONINTERLEAVED :AOS_PCM_ACCESS_RW_INTERLEAVED);

    if (err < 0) {
        LOGD(TAG, "Access type not available");
    }

    err = aos_pcm_hw_params_set_format(pcm, params, p->sample_bits);

    if (err < 0) {
        LOGD(TAG, "Sample format non available");
    }

    err = aos_pcm_hw_params_set_channels(pcm, params, p->channles);

    if (err < 0) {
        LOGD(TAG, "Channels count non available");
    }

    aos_pcm_hw_params_set_rate_near(pcm, params, &p->rate, 0);

    period_frames = p->period_bytes / (p->sample_bits * p->channles / 8);
    aos_pcm_hw_params_set_period_size_near(pcm, params, &period_frames, 0);

    buffer_frames = period_frames * 3;
    aos_pcm_hw_params_set_buffer_size_near(pcm, params, &buffer_frames);

    err = aos_pcm_hw_params(pcm, params);

    return pcm;
}

static int pcm_param_init(voice_pcm_t *p)
{
    p->mic->hdl = _param_init(p, p->mic->param->pcm_name,  p->mic->param);

    if (p->ref) {
        p->ref->hdl = _param_init(p, p->ref->param->pcm_name,  p->ref->param);
    }

    return 0;
}

#ifdef VOICE_DEBUG
int g_hello_offset = 0;
static int pcm_recv(aos_pcm_t *pcm, void *data , int len, int access)
{
     if ((len + g_hello_offset) >= local_audio_nhxb_len) {
        g_hello_offset = 0;
    }

    memcpy((char *)data, local_audio_nhxb + g_hello_offset, len);
    g_hello_offset += len;
    aos_msleep(80);

    return len;
}
#else
// static char test_data[1024*4];
static int pcm_recv(aos_pcm_t *pcm, void *data , int len, int access)
{
    int ret = -1;

    while (1) {
        aos_pcm_wait(pcm, AOS_WAIT_FOREVER);

        if (access) {
            // ret = aos_pcm_readi(pcm, (void *)test_data, aos_pcm_bytes_to_frames(pcm, len));
            ret = aos_pcm_readi(pcm, (void *)data, aos_pcm_bytes_to_frames(pcm, len));
        } else {
            // ret = aos_pcm_readn(pcm, (void **)test_data, aos_pcm_bytes_to_frames(pcm, len));
            ret = aos_pcm_readn(pcm, (void **)data, aos_pcm_bytes_to_frames(pcm, len));
        }

        if(ret > 0) {
            break;
        }
    }

    // LOGD(TAG, "captrue over");
    // memcpy((char *)data, test_data, len);

    return aos_pcm_frames_to_bytes(pcm, ret);
}
#endif
static void pcm_buffer_init(voice_pcm_t *pcm)
{
    int mic_len = 0;
    int ref_len = 0;

    if (pcm->mic) {
        voice_pcm_param_t *p = pcm->mic->param;
        mic_len = p->period_bytes;
    }

    if (pcm->ref) {
        voice_pcm_param_t *p =pcm->ref->param;
        ref_len = p->period_bytes;
    }

    pcm->data = voice_malloc(mic_len + ref_len);
    pcm->len = mic_len + ref_len;
    pcm->mic->data = pcm->data;
    pcm->mic->len = mic_len;

    if (pcm->ref->param) {
        pcm->ref->data = (char *)pcm->data + mic_len;
        pcm->ref->len = ref_len;
    }
}

static void pcm_buffer_deinit(voice_pcm_t *p)
{
    aos_free(p->mic->param);
    aos_free(p->ref->param);
    aos_free(p->mic);
    aos_free(p->ref);
    aos_free(p->data);
}

static void pcm_entry(void *priv)
{
    voice_pcm_t *p = (voice_pcm_t *)priv;
    int ret = -1;
    voice_capture_t *capture;

    pcm_buffer_init(p);
    pcm_param_init(p);

    while (1) {
        capture = p->mic;
        ret = pcm_recv(capture->hdl, capture->data, capture->len,capture->param->access);
        capture = p->ref;
        if (capture) {
            ret = pcm_recv(capture->hdl, capture->data, capture->len,capture->param->access);
        }

        if (ret >= 0) {
            p->pcm_send(p->priv, p->data, p->len);
        }
    }
}

voice_pcm_t *pcm_init(voice_pcm_send send, void *priv)
{
    voice_pcm_t *p = aos_zalloc_check(sizeof(voice_pcm_t));

    p->pcm_send = send;
    p->priv     = priv;

    return p;
}

void pcm_deinit(voice_pcm_t *p)
{
    pcm_buffer_deinit(p);
}

void pcm_mic_config(voice_pcm_t *p, voice_pcm_param_t *param)
{
    voice_capture_t *capture = p->mic;

    if (param == NULL) {
        return;
    }

    if (capture == NULL) {
        capture = aos_malloc_check(sizeof(voice_capture_t));
        capture->param = aos_malloc_check(sizeof(voice_pcm_param_t));
        p->mic = capture;
    }

    memcpy(capture->param, param, sizeof(voice_pcm_param_t));
}

void pcm_ref_config(voice_pcm_t *p, voice_pcm_param_t *param)
{
    voice_capture_t *capture = p->ref;

    if (param == NULL) {
        return;
    }

    if (capture == NULL) {
        capture = aos_malloc_check(sizeof(voice_capture_t));
        capture->param = aos_malloc_check(sizeof(voice_pcm_param_t));
        p->ref = capture;
    }

    memcpy(capture->param, param, sizeof(voice_pcm_param_t));
}

int pcm_start(voice_pcm_t *p)
{
    aos_task_t task;
    int ret = aos_task_new_ext(&task, "vpcm", pcm_entry, p, 2 * 1024, AOS_DEFAULT_APP_PRI - 4);

    if (ret < 0) {
        return -1;
    }

    return 0;
}


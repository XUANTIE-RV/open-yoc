/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <ulog/ulog.h>
#include <alsa/pcm.h>

#include "yoc/pcm_input_port.h"

#include "../../pcm_input_internal.h"

#define TAG "AACQ"

/* Debug: If equal to 1,pcm simulation mode, at startup time at ignore alsa capture data */
extern int g_pcminput_ignore_alsa;
extern  aos_pcm_sframes_t aos_pcm_avail(aos_pcm_t *pcm);

static aos_pcm_t *pcmC0_ = NULL;
static aos_pcm_t *capture_init(const char *devname, unsigned int sample_rate /*16000*/, int chn_num,
                               int bit_format /*16bit*/, aos_pcm_uframes_t peroid_size)
{
    aos_pcm_hw_params_t *params;
    aos_pcm_t           *pcm = NULL;

    int err = 0;

    err = aos_pcm_open(&pcm, devname, AOS_PCM_STREAM_CAPTURE, 0);

    if (err < 0) {
        LOGE(TAG, "aos_pcm_open %s error", devname);
        return NULL;
    }

    aos_pcm_hw_params_alloca(&params);
    err = aos_pcm_hw_params_any(pcm, params);

    if (err < 0) {
        LOGE(TAG, "Broken configuration for this PCM: no configurations available");
        aos_pcm_close(pcm);
        return NULL;
    }

    err = aos_pcm_hw_params_set_access(pcm, params, AOS_PCM_ACCESS_RW_NONINTERLEAVED);

    if (err < 0) {
        LOGE(TAG, "Access type not available");
        aos_pcm_close(pcm);
        return NULL;
    }

    err = aos_pcm_hw_params_set_format(pcm, params, bit_format);

    if (err < 0) {
        LOGE(TAG, "Sample bit_format non available");
        aos_pcm_close(pcm);
        return NULL;
    }

    err = aos_pcm_hw_params_set_channels(pcm, params, chn_num);

    if (err < 0) {
        LOGE(TAG, "Channels count non available");
        aos_pcm_close(pcm);
        return NULL;
    }

    aos_pcm_hw_params_set_rate_near(pcm, params, &sample_rate, 0);

    aos_pcm_uframes_t val_peroid_size = peroid_size;
    aos_pcm_hw_params_set_period_size_near(pcm, params, &val_peroid_size, 0);

    aos_pcm_uframes_t val_buffer_frames = val_peroid_size * 23; /*buffer保存16个frame*/
    aos_pcm_hw_params_set_buffer_size_near(pcm, params, &val_buffer_frames);

    err = aos_pcm_hw_params(pcm, params);

    if (err < 0) {
        LOGE(TAG, "aos_pcm_hw_params error");
        aos_pcm_close(pcm);
        return NULL;
    }

    return pcm;
}

/**
 * @brief  capture audio data from alsa
 *
 * @param  [out] data : 3-channel interleaved audio (mic1,mic2,ref)
 * @param  [int] len : data buffer byte length
 * @return <0 failed, >=0 byte length of capture data
 */
static int bl606p_voice_pcm_acquire(void *data, int len)
{
    int rlen = 0;
    int ret = 0;

    if (pcmC0_ == NULL) {
        return 0;
    }

    //long long wait_start = aos_now_ms();
    while (aos_pcm_avail(pcmC0_) < aos_pcm_bytes_to_frames(pcmC0_, len)) {
        ret = aos_pcm_wait(pcmC0_, AOS_WAIT_FOREVER);
    }

    if (ret < 0) {
        //LOGD(TAG, "pcm xrun: read %d, avail %d, ms=%lld", aos_pcm_bytes_to_frames(pcmC0_, len), 
        //            aos_pcm_avail(pcmC0_), loop_time, aos_now_ms() - wait_start );
        rlen = 0;
        int i = 0;
        for (i = 0; i < 64; i++) {
            if (rlen >= 0) {
                rlen += aos_pcm_readn(pcmC0_, (void **)data, aos_pcm_bytes_to_frames(pcmC0_, len));
            } else {
                break;
            }
        }
        LOGD(TAG, "pcm xrun reset, resetlen=%d loop=%d", aos_pcm_frames_to_bytes(pcmC0_, rlen), i);
    }

    rlen = aos_pcm_readn(pcmC0_, (void**)data, aos_pcm_bytes_to_frames(pcmC0_, len));
    rlen = aos_pcm_frames_to_bytes(pcmC0_, rlen);

    /* pcm push hook, overwrite capture data */
    if (g_pcminput_ignore_alsa == 0) {
        voice_pcm_http_rewrite(data, rlen);
    } else {
        /* no alsa data to alg when dev startup */
        int hook_ret = voice_pcm_http_rewrite(data, rlen);

        if (hook_ret <= 0) {
            rlen = hook_ret;
        }
    }
    return rlen;
}

static int bl606p_voice_pcm_acquire_init(int bit_format, int sample_rate, int frame_ms, int chn_num)
{
    static int init_flag = 0;
    if (init_flag) {
        return 0;
    }

    /* Single frame single channel sample count */
    aos_pcm_uframes_t peroid_size = frame_ms * (sample_rate / 1000);

    pcmC0_ = capture_init("pcmC0", sample_rate, chn_num, bit_format, peroid_size);

    if (pcmC0_ == NULL) {
        return -1;
    }

    ssize_t capture_byte = aos_pcm_frames_to_bytes(pcmC0_, peroid_size);

    init_flag = 1;

    return (int)capture_byte;
}

pcm_input_ops_t g_pcm_acquire_ops = {
    .init        = bl606p_voice_pcm_acquire_init,
    .pcm_acquire = bl606p_voice_pcm_acquire,
};

void pcm_input_register()
{
    pcm_acquire_register(&g_pcm_acquire_ops);
}
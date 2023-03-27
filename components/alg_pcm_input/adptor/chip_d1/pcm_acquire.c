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

static aos_pcm_t *pcmC0_ = NULL;
static aos_pcm_t *pcmC1_ = NULL;

static int        d1_voice_pcm_acquire(void *data, int len);
static aos_pcm_t *capture_init(const char *devname, unsigned int sample_rate /*16000*/, int chn_num,
                               int bit_format /*16bit*/, aos_pcm_uframes_t peroid_size)
{
    aos_pcm_hw_params_t *params;
    aos_pcm_t *          pcm = NULL;

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

    err = aos_pcm_hw_params_set_access(pcm, params, AOS_PCM_ACCESS_RW_INTERLEAVED);

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

    aos_pcm_uframes_t val_buffer_frames = val_peroid_size * 16; /*buffer保存16个frame*/
    aos_pcm_hw_params_set_buffer_size_near(pcm, params, &val_buffer_frames);

    err = aos_pcm_hw_params(pcm, params);

    if (err < 0) {
        LOGE(TAG, "aos_pcm_hw_params error");
        aos_pcm_close(pcm);
        return NULL;
    }

    return pcm;
}

static int d1_voice_pcm_acquire_init(int bit_format, int sample_rate, int frame_ms, int chn_num)
{
    static int init_flag = 0;

    if (init_flag) {
        return 0;
    }

    ssize_t  capture_byte = 0;
    int16_t *capture_buf  = NULL;
    //int      err          = 0;

    /* Single frame single channel sample count */
    aos_pcm_uframes_t peroid_size = frame_ms * (sample_rate / 1000);

    /* init sound car, peroid_size / 2 reduce driver latency */
#ifndef DISABLED_PCMC0
    pcmC0_ = capture_init("pcmC0", sample_rate, chn_num, bit_format, peroid_size / 2);
#endif

    if (pcmC0_) {
        capture_byte = aos_pcm_frames_to_bytes(pcmC0_, peroid_size);
        capture_buf  = (int16_t *)malloc(capture_byte);
    } else {
        LOGW(TAG, "Init device pcmC0 error");
    }

    pcmC1_ = capture_init("pcmC1", sample_rate, chn_num, bit_format, peroid_size / 2);

    if (pcmC1_) {
        if (pcmC0_ == NULL) {
            capture_byte = aos_pcm_frames_to_bytes(pcmC1_, peroid_size);
            capture_buf  = (int16_t *)malloc(capture_byte);
        }
    } else {
        LOGW(TAG, "Init device pcmC1 error");
    }

    /* start capture device */
    if (pcmC0_) {
        aos_pcm_readi(pcmC0_, (void *)capture_buf, 1);
    }

    if (pcmC1_) {
        aos_pcm_readi(pcmC1_, (void *)capture_buf, 1);
    }

    LOGD(TAG, "Ignore initial packets");

    for (int i = 0; i < 1200; i++) {
        d1_voice_pcm_acquire((void *)capture_buf, capture_byte / 4);
    }

    if (capture_buf) {
        free(capture_buf);
    }

    init_flag = 1;

    return (int)capture_byte;
}

/**
 * @brief  capture audio data from alsa
 * The current code is aimed at two D1 sound cards.
 * pcmC0：Digital microphone,3 channel: dmic1 dmic2 dmic3
 * pcmC1：Analog microphone,3 channel: ref1 ref2 hamic(headphone mic)
 *
 * pcmC0 & pcmC1：dmic1 dmic2 ref1
 * only pcmC0：dmic1 dmic2 zero
 * only pcmC1：hamic hamic ref1
 *
 * @param  [out] data : 3-channel interleaved audio (mic1,mic2,ref)
 * @param  [int] len : data buffer byte length
 * @return <0 failed, >0 byte length of capture data
 */
static int d1_voice_pcm_acquire(void *data, int len)
{
    int     rlen = 0, reflen = len;
    int8_t *dataref = NULL;

    /* pcmC0 & pcmC1 */
    if (pcmC0_ && pcmC1_) {
        aos_pcm_wait(pcmC0_, AOS_WAIT_FOREVER);
        rlen = aos_pcm_readi(pcmC0_, (void *)data, aos_pcm_bytes_to_frames(pcmC0_, len));
        rlen = aos_pcm_frames_to_bytes(pcmC0_, rlen);

        dataref = (int8_t *)malloc(len);

        aos_pcm_wait(pcmC1_, AOS_WAIT_FOREVER);
        int rlenc1 = aos_pcm_readi(pcmC1_, (void *)dataref, aos_pcm_bytes_to_frames(pcmC1_, reflen));
        rlenc1     = aos_pcm_frames_to_bytes(pcmC1_, rlenc1);

        if (rlen != rlenc1) {
            LOGW(TAG, "capture %d %d", rlen, reflen);
            free(dataref);
            return 0;
        }

        if (rlen != len) {
            LOGW(TAG, "capture %d", rlen);
            free(dataref);
            return 0;
        }
    }

    /* Only pcmC0 */
    if (pcmC0_ && pcmC1_ == NULL) {
        aos_pcm_wait(pcmC0_, AOS_WAIT_FOREVER);
        rlen = aos_pcm_readi(pcmC0_, (void *)data, aos_pcm_bytes_to_frames(pcmC0_, len));
        rlen = aos_pcm_frames_to_bytes(pcmC0_, rlen);

        int16_t *ptr = (int16_t *)data;

        for (int i = 0; i < rlen / 2; i += 3) {
            ptr[2] = 0; /* channel 3 set zero */
            ptr += 3;
        }
    }

    /* Only pcmC1: channel: ref1 ref2 hamic*/
    if (pcmC0_ == NULL && pcmC1_) {
        aos_pcm_wait(pcmC1_, AOS_WAIT_FOREVER);
        rlen = aos_pcm_readi(pcmC1_, (void *)data, aos_pcm_bytes_to_frames(pcmC1_, len));
        rlen = aos_pcm_frames_to_bytes(pcmC1_, rlen);

        int16_t *ptr = (int16_t *)data;

        for (int i = 0; i < rlen / 2; i += 3) {
            ptr[1] = ptr[2];
            ptr[2] = ptr[0];
            ptr[0] = ptr[1];
            ptr += 3;
        }
    }

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

    /* pcmC0 & pcmC1: channel 3 import reference sound*/
    if (pcmC0_ && pcmC1_) {
        if (dataref) {
            int16_t *pmic = (int16_t *)data;
            int16_t *pref = (int16_t *)dataref;

            for (int i = 0; i < rlen / 2; i += 3) {
#if defined(CONFIG_BOARD_F133_EVB) && CONFIG_BOARD_F133_EVB
                pmic[2] = pref[2];
#else
                pmic[2] = pref[0];
#endif
                pmic += 3;
                pref += 3;
            }

            free(dataref);
        }
    }

    return rlen;
}

pcm_input_ops_t g_pcm_acquire_ops = {
    .init        = d1_voice_pcm_acquire_init,
    .pcm_acquire = d1_voice_pcm_acquire,
};

void pcm_input_register()
{
    pcm_acquire_register(&g_pcm_acquire_ops);
}
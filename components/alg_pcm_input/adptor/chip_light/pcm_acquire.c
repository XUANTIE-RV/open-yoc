/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <ulog/ulog.h>

#include <alsa/asoundlib.h>

#include "yoc/pcm_input_port.h"
#include "../../pcm_input_internal.h"

#define TAG "AACQ"

#define FULL_DTB

#if defined(FULL_DTB)
/* HW config:setenv fdt_file light-a-val-full.dtb */
#define PCM_DEVICE "hw:0,1"
#define PCM_CHN_COUNT 2
#elif defined(TDM2_DTB)
/* setenv fdt_file light-a-val-audio-tdm.dtb */
#define PCM_DEVICE "tdm_2slot"
#define PCM_CHN_COUNT 2
#elif defined(TDM8_DTB)
/* setenv fdt_file light-a-val-audio-tdm.dtb */
#define PCM_DEVICE "tdm_8slot"
#define PCM_CHN_COUNT 8
#endif

#ifndef CONFIG_PCM_DATA_INTERLEAVED
#define CONFIG_PCM_DATA_INTERLEAVED   0
#endif

static snd_pcm_t *pcm_handle        = NULL;
static int        g_hw_buffer_size  = 0; /* 字节 */
static int        g_hw_frames       = 0; /* 一通道一帧的采样点数 */
static int        g_alg_buffer_size = 0; /* 字节 */

static int pcm_hw_init()
{
    int                  err;
    snd_pcm_hw_params_t *params;
    unsigned int         channels    = PCM_CHN_COUNT;         // 声道数
    unsigned int         sample_rate = 16000;                 // 采样率
    snd_pcm_format_t     format      = SND_PCM_FORMAT_S16_LE; // 采样格式
    snd_pcm_uframes_t    frames      = 320;                   // 缓冲区大小  20ms

    err = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "Unable to open PCM device: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_malloc(&params);
    if (err < 0) {
        fprintf(stderr, "Unable to allocate hardware parameter structure: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_any(pcm_handle, params);
    if (err < 0) {
        fprintf(stderr, "Unable to initialize hardware parameter structure: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        fprintf(stderr, "Unable to set access type: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    if (err < 0) {
        fprintf(stderr, "Unable to set channel count: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, 0);
    if (err < 0) {
        fprintf(stderr, "Unable to set sample rate: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_set_format(pcm_handle, params, format);
    if (err < 0) {
        fprintf(stderr, "Unable to set sample format: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, 0);
    if (err < 0) {
        fprintf(stderr, "Unable to set period size: %s", snd_strerror(err));
        return -1;
    }

    err = snd_pcm_hw_params(pcm_handle, params);
    if (err < 0) {
        fprintf(stderr, "Unable to set hardware parameters: %s", snd_strerror(err));
        return -1;
    }

    /* 重新获取frame大小 */
    snd_pcm_hw_params_get_period_size(params, &frames, 0);
    g_hw_frames = frames;

    /* 返回一帧的字节数 */
    return snd_pcm_frames_to_bytes(pcm_handle, frames);
}

static int pcm_hw_acquire(char *buffer, int len)
{
    // 等待缓冲区就绪
    int ret = snd_pcm_wait(pcm_handle, 1000);
    if (ret == -EAGAIN) {
        return 0;
    } else if (ret < 0) {
        return -1;
    }

    int err = snd_pcm_readi(pcm_handle, buffer, snd_pcm_bytes_to_frames(pcm_handle, len));
    if (err == -EPIPE) {
        fprintf(stderr, "Overrun occurred: %d", err);
        snd_pcm_prepare(pcm_handle);
        return 0;
    } else if (err < 0) {
        fprintf(stderr, "Unable to read from PCM device: %s", snd_strerror(err));
        return 0;
    } else {
        ; // printf("Read %d frames\r\n", err);
    }

    return snd_pcm_frames_to_bytes(pcm_handle, err);
}

static int voice_pcm_acquire_init(int bit_format, int sample_rate, int frame_ms, int chn_num)
{
    g_hw_buffer_size = pcm_hw_init();

    /* 帧时间转化为帧大小 */
    snd_pcm_uframes_t peroid_size = frame_ms * (sample_rate / 1000);

    /* 返回字节数，算法应用用来申请缓区大小*/
    g_alg_buffer_size = peroid_size * chn_num * (bit_format / 8);
    return g_alg_buffer_size;
}

static int voice_pcm_acquire(void *data, int len)
{
    if (g_alg_buffer_size != len) {
        fprintf(stderr, "pcm acquire param len %d(%d) error", len, g_alg_buffer_size);
        return -1;
    }

    /* 从设备读取数据 */
    char *buffer;
    buffer = (char *)malloc(g_hw_buffer_size);

    int ret = pcm_hw_acquire(buffer, g_hw_buffer_size);
    if (ret <= 0) {
        free(buffer);
        return -1;
    }

    /* 采集的数据转化为算法应用需要的数据 */
    short *ptr_src  = (short *)buffer;
    short *prt_dest = (short *)data;

#if defined(FULL_DTB) || defined(TDM2_DTB)
    /* 2路转3路，第2路置空 */
    for (int i = 0; i < g_hw_frames; i++) {
#if defined(CONFIG_PCM_DATA_INTERLEAVED) && CONFIG_PCM_DATA_INTERLEAVED > 1
        prt_dest[0] = ptr_src[0];
        prt_dest[1] = ptr_src[1];
        prt_dest[2] = 0;
        ptr_src += PCM_CHN_COUNT;
        prt_dest += 3;
#else
        prt_dest[i] = ptr_src[0];
        prt_dest[i + g_hw_frames] = ptr_src[1];
        prt_dest[i + (g_hw_frames << 1)] = 0;
        ptr_src += PCM_CHN_COUNT;
#endif
    }
#elif defined(TDM8_DTB)
    /* 8路转3路，取0 1 7(参考音) */
    for (int i = 0; i < g_hw_frames; i++) {
#if defined(CONFIG_PCM_DATA_INTERLEAVED) && CONFIG_PCM_DATA_INTERLEAVED > 1
        prt_dest[0] = ptr_src[0];
        prt_dest[1] = ptr_src[1];
        prt_dest[2] = ptr_src[7];
        ptr_src += PCM_CHN_COUNT;
        prt_dest += 3;
#else
        prt_dest[i] = ptr_src[0];
        prt_dest[i + g_hw_frames] = ptr_src[1];
        prt_dest[i + (g_hw_frames << 1)] = 0;
        ptr_src += PCM_CHN_COUNT;
#endif
    }
#endif
    free(buffer);
    return len;
}

pcm_input_ops_t g_pcm_acquire_ops = {
    .init        = voice_pcm_acquire_init,
    .pcm_acquire = voice_pcm_acquire,
};

void pcm_input_register()
{
    pcm_acquire_register(&g_pcm_acquire_ops);
}

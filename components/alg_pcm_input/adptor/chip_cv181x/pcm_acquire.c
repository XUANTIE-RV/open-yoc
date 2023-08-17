/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <ulog/ulog.h>
#include <alsa/pcm.h>

#include "yoc/pcm_input_port.h"
#include <speex/speex_resampler.h>

#include "../../pcm_input_internal.h"

#define TAG "AACQ"

#ifndef CONFIG_RESAMPLE_RATE
#define CONFIG_RESAMPLE_RATE 16000
#endif

#if defined(CONFIG_BOARD_RUIXIANG_BOARD)
#define CHANNEL_IDX_OF_REF_VOICE 0
#elif defined(CONFIG_BOARD_HUASHANPI)
#define CHANNEL_IDX_OF_REF_VOICE 1
#else
#define CHANNEL_IDX_OF_REF_VOICE 1
#endif

/* Debug: If equal to 1,pcm simulation mode, at startup time at ignore alsa capture data */
extern int g_pcminput_ignore_alsa;

/**
 * @brief  采集设备的初始化
 */
static aos_pcm_t *capture_init(const char *devname, unsigned int sample_rate /*16000*/, int chn_num,
                               int bit_format /*16bit*/, aos_pcm_uframes_t peroid_size)
{
    aos_pcm_hw_params_t *params;
    aos_pcm_t *          pcm = NULL;

    int err = 0;

    LOGD(TAG, "capture init param %d %d %d", bit_format, sample_rate, peroid_size);

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

	//params->period_size = peroid_size;
	//params->buffer_size = peroid_size * 4;

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

    int dir = 1;
    aos_pcm_hw_params_set_rate_near(pcm, params, &sample_rate, &dir);

#if 1
    aos_pcm_uframes_t val_peroid_size = peroid_size;
    aos_pcm_hw_params_set_period_size_near(pcm, params, &val_peroid_size, 0);

    aos_pcm_uframes_t val_buffer_frames = val_peroid_size * 4; /*buffer保存4个frame*/
    aos_pcm_hw_params_set_buffer_size_near(pcm, params, &val_buffer_frames);
#endif

    err = aos_pcm_hw_params(pcm, params);

    if (err < 0) {
        LOGE(TAG, "aos_pcm_hw_params error");
        aos_pcm_close(pcm);
        return NULL;
    }

    return pcm;
}

/**
 * 适配代码
 **/
static aos_pcm_t *pcmD1_ = NULL; //两路麦克风
static aos_pcm_t *pcmC0_ = NULL; //两路参考音，适配只取其中一路

/* 保存初始化音频参数 */
static int g_bit_format = 0;
static int g_sample_rate = 0;
static int g_frame_ms = 0;
static int g_chn_num = 0;

//#define SPEEX_RESAMPLE_SUPPORT 1

#ifdef SPEEX_RESAMPLE_SUPPORT
/* For Resample */
static SpeexResamplerState *s_pcm_srs = NULL;
static int                  g_pcm_error;
static spx_int16_t *resample = NULL;
static unsigned int resample_len = 0;
#endif

/**
 * @brief  采集初始化
 * @param  bit_format: 音频比特率，目前方案固定传入16
 * @param  sample_rate: 采样率，目前方案固定传入16000
 * @param  frame_ms: 一帧的时间，当前算法要求一般是10ms或20ms
 * @param  chn_num: 通道数，目前方案固定传入三路，要求麦克风1,麦克风2,回声参考音
 * @return <=0:初始化失败, >0:返回frame_ms对应一帧的字节数量,应用会一该长度读取调用cv181x_voice_pcm_acquire函数
 */
static int cv181x_voice_pcm_acquire_init(int bit_format, int sample_rate, int frame_ms, int chn_num)
{
    /* 打开两个设备，分别读取两路数据，最后合成算法需要的三路数据 */

    /* 单帧单通道采样点数 */
    aos_pcm_uframes_t peroid_size = frame_ms * (sample_rate / 1000);
    aos_pcm_uframes_t ref_peroid_size = frame_ms * (CONFIG_RESAMPLE_RATE / 1000);
#if 1
    pcmD1_ = capture_init("pcmD1", sample_rate, 2, bit_format, peroid_size);
    if (pcmD1_ == NULL) {
        LOGE(TAG, "Init device pcmD1 error");
        return -1;
    }
#endif
    pcmC0_ = capture_init("pcmC0", CONFIG_RESAMPLE_RATE, 2, bit_format, ref_peroid_size);
    if (pcmC0_ == NULL) {
        LOGE(TAG, "Init device pcmC0 error");
        return -1;
    }

    if (CONFIG_RESAMPLE_RATE != sample_rate && (sample_rate != 16000 || CONFIG_RESAMPLE_RATE != 48000)) {
        LOGE(TAG, "Only support (16K mic + ref) or (16K mic and 48K ref)");
        aos_check_return_val(0,  -1);
    }

#ifdef SPEEX_RESAMPLE_SUPPORT
    /* check if sample rate of pdm equal to ref audio;
       if there are different, need resample ref audio data.
    */
    if (CONFIG_RESAMPLE_RATE != sample_rate) {
        s_pcm_srs = speex_resampler_init((spx_uint32_t)2, (spx_uint32_t)CONFIG_RESAMPLE_RATE, (spx_uint32_t)sample_rate, 3, &g_pcm_error);
        resample_len = peroid_size * (bit_format / 8) * 2;
        resample = (spx_int16_t *)aos_malloc(resample_len);
        if (resample == NULL) {
            LOGE(TAG, "malloc resample buf error");
            aos_check_return_val(0,  -1);
        }
    }
#endif

    g_bit_format = bit_format;
    g_sample_rate = sample_rate;
    g_frame_ms = frame_ms;
    g_chn_num = chn_num;

    LOGD(TAG, "voice init param %d %d %d %d", g_bit_format, g_sample_rate, g_frame_ms, g_chn_num);

    /* 返回算法需要的一帧的字节数量 */
    return (peroid_size * chn_num * (bit_format / 8));
}

/**
 * @brief  capture audio data from alsa
 * @param  [out] data : 3-channel interleaved audio (mic1,mic2,ref)
 * @param  [int] len : data buffer byte length
 * @return <0 failed, >0 byte length of capture data
 */
static int cv181x_voice_pcm_acquire(void *data, int len)
{
    aos_pcm_sframes_t ret;
    int read_len;
    int16_t *data_ref_ptr = NULL;
    static int16_t *data_mic = NULL;
    static int16_t *data_ref = NULL;

    aos_pcm_uframes_t peroid_size = g_frame_ms * (g_sample_rate / 1000);
    aos_pcm_uframes_t ref_peroid_size = g_frame_ms * (CONFIG_RESAMPLE_RATE / 1000);

    int rlen_mic = (peroid_size * 2 * (g_bit_format / 8));//读取两通道数据
    int rlen_ref = (ref_peroid_size * 2 * (g_bit_format / 8));//读取两通道数据
    
    if (data_mic == NULL) {
        data_mic = (int16_t *)aos_malloc(rlen_mic + 4096);
        if (data_mic == NULL) {
            LOGE(TAG, "malloc %d", rlen_mic);
            aos_check_return_val(0,  -1);
        }
        memset(data_mic, 0, rlen_mic + 4096);
    }
#if 1
    aos_pcm_wait(pcmD1_, AOS_WAIT_FOREVER);
    ret = aos_pcm_readi(pcmD1_, data_mic, peroid_size);//接收交错音频数据, 320
    if (ret != peroid_size) {
        LOGE(TAG, "capture pcmD1  %d %d", ret, peroid_size);
        //aos_free(data_mic);
        return -1;
    }

    read_len = (peroid_size * g_chn_num * (g_bit_format / 8));
    if (read_len != len ) {
        LOGE(TAG, "pdm read len error %d %d", read_len, len);
        return -1;
    }
#endif

    if (data_ref == NULL) {
        data_ref = (int16_t *)aos_malloc(rlen_ref + 4096);
        if (data_ref == NULL) {
            LOGE(TAG, "malloc %d", rlen_ref);
            aos_check_return_val(0,  -1);
        }
        memset(data_ref, 0, rlen_ref + 4096);
    }

    aos_pcm_wait(pcmC0_, AOS_WAIT_FOREVER);
    ret = aos_pcm_readi(pcmC0_, data_ref, ref_peroid_size);//接收交错音频数据, 320
    if (ret != ref_peroid_size) {
        LOGE(TAG, "capture pcmC0 %d %d", ret, ref_peroid_size);
        //aos_free(data_ref);
        return -1;
    }

    data_ref_ptr = data_ref;

#ifdef SPEEX_RESAMPLE_SUPPORT
    /* check if sample rate of pdm equal to ref audio;
       if there are different, need resample ref audio data.
    */
    if (CONFIG_RESAMPLE_RATE != g_sample_rate && s_pcm_srs != NULL) {
        /* resample 48K -> 16K with single channel */
        speex_resampler_process_int(s_pcm_srs, CHANNEL_IDX_OF_REF_VOICE, (spx_int16_t *)data_ref, (spx_uint32_t *)&ref_peroid_size,
                                    (spx_int16_t *)resample, (spx_uint32_t *)&resample_len);
        ref_peroid_size = resample_len;
        data_ref_ptr = resample;
    }
#endif

    read_len = (ref_peroid_size * g_chn_num * (g_bit_format / 8));
    if (read_len != len ) {
        LOGE(TAG, "ref read len error %d %d", read_len, len);
        return -1;
    }

    /* 
     *采样数据输出 
    */
    /*单帧单通道采样点数*/
    aos_pcm_uframes_t voice_peroid_size = g_frame_ms * (g_sample_rate / 1000);
    //LOGD(TAG, ">>>>%p %p %p", data, data_mic, data_ref);
    int16_t *ptr = (int16_t *)data;
    int16_t *data_mic_ptr = data_mic;
    for (int i = 0; i < voice_peroid_size; i++) {
        ptr[0] = data_mic_ptr[0];
        ptr[1] = data_mic_ptr[1];
#ifdef SPEEX_RESAMPLE_SUPPORT
        /* Resample to single channel with SPEEX, so don't need filter  */
        if (CONFIG_RESAMPLE_RATE != g_sample_rate) {
            /* 16K Sample Rate with single channel */
            ptr[2] = data_ref_ptr[0];
            data_ref_ptr += 1;
        } else {
            /* 16K Sample Rate with 2 channels */
            ptr[2] = data_ref_ptr[CHANNEL_IDX_OF_REF_VOICE];
            data_ref_ptr += 2;
        }
#else
        ptr[2] = data_ref_ptr[CHANNEL_IDX_OF_REF_VOICE];
        if (CONFIG_RESAMPLE_RATE != g_sample_rate) {
            /* Filter, since 48K = 3 * 16K, need read 3 * 2 = 6 */
            data_ref_ptr += 6;
        } else {
            data_ref_ptr += 2;
        }
#endif
        ptr += 3;
        data_mic_ptr += 2;
    }
    //LOGD(TAG, ">>>>%p %p %p", data, data_mic_ptr, data_ref_ptr);

    /* pcm push hook, overwrite capture data */
    if (g_pcminput_ignore_alsa == 0) {
        voice_pcm_http_rewrite(data, len);
    } else {
        /* no alsa data to alg when dev startup */
        int hook_ret = voice_pcm_http_rewrite(data, len);

        if (hook_ret <= 0) {
            read_len = hook_ret;
        }
    }

    return len;
}

pcm_input_ops_t g_pcm_acquire_ops = {
    .init        = cv181x_voice_pcm_acquire_init,
    .pcm_acquire = cv181x_voice_pcm_acquire,
};

void pcm_input_register()
{
    pcm_acquire_register(&g_pcm_acquire_ops);
}
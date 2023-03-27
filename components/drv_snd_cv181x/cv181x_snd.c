/* SPDX-License-Identifier: GPL-2.0-or-later
 * CV181xC CODEC driver on CVITEK CV181xC
 *
 * Copyright 2022 CVITEK
 *
 * Author: Ruilong.Chen
 *
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <devices/driver.h>
#include <drv/codec.h>
#include <math.h>

#define TAG "snd"

#define pcm_uninit rvm_hal_device_free
#define mixer_uninit rvm_hal_device_free

#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)

#define BUFFER_SIZE 40960 //3840 //3200
#define DAC_WRITE_EVENT  (0x01)
#define ADC_READ_EVENT   (0x02)

#define AUDIO_OUTPUT_NONCACHE_MEM 0x80600000 //noncache
#define AUDIO_INPUT_NONCACHE_MEM (AUDIO_OUTPUT_NONCACHE_MEM + BUFFER_SIZE) //noncache
#define DAC_GAIN_MIN     (-24)
#define DAC_GAIN_MAX     (6)   // FIXME: because of pangu C2 board.

#define P_IDX 3
#define C_IDX 0
#define PDM_IDX 1

typedef struct {
    csi_codec_output_t *hdl;
    csi_dma_ch_t       *dma_hdl;
    aos_pcm_hw_params_t params;
    int state;
} playback_t;

typedef struct {
    csi_codec_input_t  *hdl;
    csi_dma_ch_t       *dma_hdl;
    aos_pcm_hw_params_t params;
    int state;
} capture_t;

typedef struct {
    csi_codec_output_t hdl;
    int l;
    int r;
} mixer_playback_t;


static csi_codec_t codec_a;
static csi_codec_t codec_pdm;
static csi_codec_t codec_p;
mixer_playback_t mixp0;
static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params);
static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params);

static int output_db2idx(int dB)
{
    static float db_array[33] = {
        -70.0f, -24.6f, -18.3f, -14.7f, -12.2f, -10.2f, -8.6f, -7.3f, -6.1f, -5.1f, -4.1f,
        -3.3f,  -2.5f,  -1.8f,  -1.2f,  -0.6f,  0.0f,   0.5f,  1.0f,  1.5f,  1.9f,  2.3f,
        2.7f,   3.1f,   3.5f,   3.9f,   4.2f,   4.5f,   4.8f,  5.1f,  5.4f,  5.7f,  6.0f,
    };

    int dB_idx = 0;
    if(dB < db_array[1]) {
        dB_idx = 0;
    } else if (dB > db_array[32]) {
        dB_idx = 32;
    } else {
        int i = 0;
        for (i = 0; i < 33; i++) {
            if (db_array[i] >= dB) {
                break;
            }
        }
        if (fabs(db_array[i] - dB) > fabs(db_array[i-1] - dB) ) {
            dB_idx = i - 1;
        } else {
            dB_idx = i;
        }
    }

    //printf(">>> output_db2idx [%d] = %.2f\r\n", dB_idx, db_array[dB_idx]);
    return dB_idx;
}

static uint8_t *get_buffer_alig_64(uint32_t len, uint64_t *first_addr)
{
	uint8_t  *addr;
	uint64_t tmp;

	tmp = (uintptr_t)aos_zalloc(len + (1 << 6) - 1);
	if(!tmp){
		return NULL;
	}
	first_addr = (uint64_t *)tmp;
	addr = (uint8_t *)((tmp+ (1 << 6) - 1) & ~((1 << 6) - 1));
	//printf("get_buffer_alig_64 tmp = 0x%lx, addr = %p\n", tmp, addr);
	return addr;
}


static void playback_free(playback_t *playback)
{
    if (playback->state == 1) {
        csi_codec_output_stop(playback->hdl);
        csi_codec_output_link_dma(playback->hdl, NULL);
        csi_codec_output_close(playback->hdl);

        if (playback->hdl->priv) {
            free((void*)playback->hdl->priv);
            playback->hdl->priv = NULL;
        }
        aos_free(playback->hdl->ring_buf);
        aos_free(playback->hdl);
        aos_free(playback->dma_hdl);
        playback->state   = 0;
        playback->hdl     = NULL;
        playback->dma_hdl = NULL;
    }
}


static int pcmp_get_remain_size(aos_pcm_t *pcm)
{
    playback_t *playback = (playback_t *)pcm->hdl;
    return csi_codec_output_buffer_remain(playback->hdl);
}



static int pcmp_lpm(rvm_dev_t *dev, int state)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = (playback_t *)pcm->hdl;

    if (state > 0) {
        playback_free(playback);
    } else {
        pcmp_param_set(pcm, &playback->params);
    }

    return 0;
}

static int pcmp_open(rvm_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = aos_zalloc(sizeof(playback_t));

    CHECK_RET_TAG_WITH_RET(NULL != playback, -1);
    pcm->hdl = playback;
    playback->state = 0;

    return 0;
}

static int pcmp_close(rvm_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);
    aos_free(playback);
    pcm->hdl = NULL;

    return 0;
}


static void codec_event_cb(csi_codec_input_t *codec, csi_codec_event_t event, void *arg)
{
    aos_pcm_t *pcm = (aos_pcm_t *)arg;

    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        pcm->event.cb(pcm, PCM_EVT_WRITE, pcm->event.priv);
    } else if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        pcm->event.cb(pcm, PCM_EVT_READ, pcm->event.priv);
    } else if (event == CODEC_EVENT_READ_BUFFER_FULL) {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    } else {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    }
}

static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);

    csi_codec_output_config_t output_config;
    csi_codec_output_t *codec = aos_zalloc(sizeof(csi_codec_output_t));
    codec->ring_buf = aos_zalloc(sizeof(dev_ringbuf_t));

    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

    int ret = csi_codec_output_open(&codec_p, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmp_err1;
    }

    csi_codec_output_attach_callback(codec, codec_event_cb, pcm);

    output_config.buffer = get_buffer_alig_64(params->buffer_bytes, (uint64_t *)codec->priv);//(uint8_t *)(AUDIO_OUTPUT_NONCACHE_MEM);
    output_config.bit_width = params->sample_bits;
    output_config.sample_rate = params->rate;
    output_config.buffer_size = params->buffer_bytes;
    output_config.period = params->period_size * params->channels * params->format / 8;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = params->channels;
    ret = csi_codec_output_config(codec, &output_config);
    if (ret != 0) {
        goto pcmp_err1;
    }

    if (mixp0.hdl.callback == NULL) {
        memcpy(&mixp0.hdl, codec, sizeof(csi_codec_output_t));
    }

    // if (mixp0.l == -31 || mixp0.r == -31) {
    //     csi_codec_output_mute(codec, 1);
    // } else {
    //     csi_codec_output_mute(codec, 0);

    //     // csi_codec_output_analog_gain(codec, mixp0.l);

    // }

    // csi_codec_output_analog_gain(codec, 0xaf);
    csi_codec_output_digital_gain(codec, output_db2idx(mixp0.l));
    csi_codec_output_buffer_reset(codec);

    //csi_dma_ch_t *dma_hdl = aos_zalloc_check(sizeof(csi_dma_ch_t));
    csi_dma_ch_t *dma_hdl = calloc(1,sizeof(csi_dma_ch_t));
    csi_codec_output_link_dma(codec, dma_hdl);

    playback->state         = 1;
    playback->hdl           = codec;
    playback->dma_hdl       = dma_hdl;
    memcpy(&playback->params, params, sizeof(aos_pcm_hw_params_t));

    csi_codec_output_start(codec);
    //printf("-------------------------code init ok\r\n");
    return 0;
pcmp_err1:
    free((void*)codec->priv);
    codec->priv = NULL;
    aos_free(codec);

    return -1;
}


static int pcm_send(aos_pcm_t *pcm, void *data, int len)
{
    playback_t *playback = (playback_t *)pcm->hdl;
    // if (g_dump_len + len< 1024*20) {
    //     memcpy(g_dump_buffer + g_dump_len, data, len);
    //     g_dump_len += len;
    // }

    int ret = csi_codec_output_write(playback->hdl, (uint8_t *)data, len);
    return ret;
}

static int pcm_pause(aos_pcm_t *pcm, int enable)
{
    playback_t *playback = (playback_t *)pcm->hdl;

    if (enable) {
        csi_codec_output_pause(playback->hdl);
    } else {
        csi_codec_output_resume(playback->hdl);
    }

    return 0;
}

//===============
static int snd_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
    //l r is dB value, Convert to csi api
    if (mixp0.hdl.callback != NULL) {
        //printf(">>> snd_set_gain %d %d dB\r\n", l, r);
        csi_codec_output_t *p = &mixp0.hdl;
        csi_codec_output_analog_gain(p, output_db2idx(l));
    } else {
        //printf(">>> snd_set_gain default %d %d dB\r\n", l, r);
    }

    mixp0.l = l;
    mixp0.r = r;

    return 0;
}

static int snd_volume_to_dB(aos_mixer_elem_t *elem, int val)
{
    //printf(">>> snd_volume_to_dB %d\r\n", val);
    //TODO: if use hardware volume(0~100), implement this
    return val;
}

static sm_elem_ops_t elem_codec1_ops = {
    .set_dB       = snd_set_gain,
    .volume_to_dB = snd_volume_to_dB,
};


static rvm_dev_t *pcm_init(driver_t *drv, void *config, int id)
{
    aos_pcm_dev_t *pcm_dev = (aos_pcm_dev_t *)rvm_hal_device_new(drv, sizeof(aos_pcm_dev_t), id);
    aos_pcm_drv_t *pcm_drv = (aos_pcm_drv_t *)drv;

    memset(&pcm_dev->pcm, 0x00, sizeof(aos_pcm_t));
    pcm_dev->pcm.ops = &(pcm_drv->ops);

    return (rvm_dev_t *)(pcm_dev);
}

static void capture_free(capture_t *capture)
{
    if (capture->state == 1) {
        csi_codec_input_stop(capture->hdl);
        csi_codec_input_link_dma(capture->hdl, NULL);
        csi_codec_input_close(capture->hdl);
        if (capture->hdl->priv) {
            free((void*)capture->hdl->priv);
            capture->hdl->priv = NULL;
        }

        aos_free(capture->hdl->ring_buf);
        aos_free(capture->hdl);
        capture->state = 0;
        capture->hdl = 0;

    }
}

static int pcmc_lpm(rvm_dev_t *dev, int state)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    if (state > 0) {
        printf("pcmc_lpm\n");
        capture_free(capture);
    } else {
        pcmc_param_set(pcm, &capture->params);
    }

    return 0;
}

static int pcmc_open(rvm_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = aos_zalloc(sizeof(capture_t));

    CHECK_RET_TAG_WITH_RET(NULL != capture, -1);
    pcm->hdl = capture;
    capture->state = 0;

    return 0;
}

static int pcmc_close(rvm_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);
    aos_free(capture);
    return 0;
}

static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

    csi_codec_input_config_t input_config;
    csi_codec_input_t *codec = aos_zalloc(sizeof(csi_codec_input_t));

    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);


    // pcm->pcm_name[4] is ch ascii data, change to decimal num
    codec->ring_buf = aos_zalloc(sizeof(dev_ringbuf_t));
    int ret = csi_codec_input_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmc_err1;
    }

    /* input ch config */
    csi_codec_input_attach_callback(codec, codec_event_cb, pcm);

    input_config.buffer = get_buffer_alig_64(params->buffer_bytes, (uint64_t *)codec->priv);
    input_config.bit_width = params->format;
    input_config.sample_rate = params->rate;
    input_config.buffer_size = params->buffer_bytes;
    input_config.period = params->period_size * params->channels * params->format / 8;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = params->channels;

    ret = csi_codec_input_config(codec, &input_config);
    if (ret != 0) {
        goto pcmc_err1;
    }

    csi_codec_input_analog_gain(codec, 12);
    csi_codec_input_digital_gain(codec, 12);

    csi_dma_ch_t *dma_hdl = aos_zalloc_check(sizeof(csi_dma_ch_t));
    csi_codec_input_link_dma(codec, dma_hdl);

    csi_codec_input_start(codec);

    capture->state = 1;
    capture->hdl = codec;
    capture->dma_hdl = dma_hdl;
    memcpy(&capture->params, params, sizeof(aos_pcm_hw_params_t));

    printf("csi codec open success\r\n");
    return 0;

pcmc_err1:
    free((void*)codec->priv);
    codec->priv = NULL;
    aos_free(codec);

    return -1;
}

static int interleave2noninterleave(short *buf, int frame_size)
{
	short temp;

	if (buf == NULL) {
		printf("input point is null \n");
		return -1;
	}

	for (int i = 1; i < frame_size;i++) {
		temp = buf[2*i];
		for (int j = 0; j < i; j++)
			buf[2 * i - j] = buf[2 * i - j - 1];

		buf[i] = temp;
	}

	return 0;
}

static int pcm_recv(aos_pcm_t *pcm, void *buf, int size)
{
    capture_t *capture = (capture_t *)pcm->hdl;
    int bytes_per_sample = pcm->hw_params->format / 8;
    int frame_size = size / pcm->hw_params->channels / bytes_per_sample;
    int ret = csi_codec_input_read(capture->hdl, (uint8_t *)buf, size);
    ret |= interleave2noninterleave((short *)buf, frame_size);
    return ret;
}

int pcmc_get_remain_size(aos_pcm_t *pcm)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    return csi_codec_input_buffer_remain(capture->hdl);
}

static int pcmd_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

    csi_codec_input_config_t input_config;
    csi_codec_input_t *codec = aos_zalloc(sizeof(csi_codec_input_t));

 
    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

    // pcm->pcm_name[4] is ch ascii data, change to decimal num
    codec->ring_buf = aos_zalloc(sizeof(dev_ringbuf_t));
    int ret = csi_codec_input_open(&codec_pdm, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmc_err1;
    }

    /* input ch config */
    csi_codec_input_attach_callback(codec, codec_event_cb, pcm);

    input_config.buffer = get_buffer_alig_64(params->buffer_bytes, (uint64_t *)codec->priv);
    input_config.bit_width = params->format;
    input_config.sample_rate = params->rate;
    input_config.buffer_size = params->buffer_bytes;
    input_config.period = params->period_size * params->channels * params->format / 8;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = params->channels;
    ret = csi_codec_input_config(codec, &input_config);
    if (ret != 0) {
        goto pcmc_err1;
    }


    csi_dma_ch_t *dma_hdl = aos_zalloc_check(sizeof(csi_dma_ch_t));
    csi_codec_input_link_dma(codec, dma_hdl);

    csi_codec_input_start(codec);
    input_config = input_config;
    capture->state = 1;
    capture->hdl = codec;
    capture->dma_hdl = dma_hdl;
    memcpy(&capture->params, params, sizeof(aos_pcm_hw_params_t));

    //printf("csi codec open success\r\n");
    return 0;

pcmc_err1:
    free((void*)codec->priv);
    codec->priv = NULL;
    aos_free(codec);

    return -1;
}
static aos_pcm_drv_t aos_pcm_drv[] = {
    {
        .drv = {
            .name               = "pcmP",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcmp_open,
            .close              = pcmp_close,
            .lpm                = pcmp_lpm,
        },
        .ops = {
            .hw_params_set      = pcmp_param_set,
            .write              = pcm_send,
            .pause              = pcm_pause,
            .hw_get_remain_size = pcmp_get_remain_size,
        },
    },
    {
        .drv = {
            .name               = "pcmC",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcmc_open,
            .close              = pcmc_close,
            .lpm                = pcmc_lpm,
        },
        .ops = {
            .hw_params_set      = pcmc_param_set,
            .read               = pcm_recv,
            .hw_get_remain_size = pcmc_get_remain_size,
        },
    },
    {
        .drv = {
            .name               = "pcmD",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcmc_open,
            .close              = pcmc_close,
            .lpm                = pcmc_lpm,
        },
        .ops = {
            .hw_params_set      = pcmd_param_set,
            .read               = pcm_recv,
            .hw_get_remain_size = pcmc_get_remain_size,
        },
    }
};


static int aos_pcm_register(void)
{
    // rvm_driver_register(&aos_pcm_drv[1].drv, NULL, 2);
    // rvm_driver_register(&aos_pcm_drv[1].drv, NULL, 1);
    rvm_driver_register(&aos_pcm_drv[0].drv, NULL, 0);
    rvm_driver_register(&aos_pcm_drv[1].drv, NULL, 0);
    rvm_driver_register(&aos_pcm_drv[2].drv, NULL, 1);
    return 0;
}

static int aos_pcm_unregister(void)
{
    rvm_driver_unregister("pcmP");
    rvm_driver_unregister("pcmC");
    rvm_driver_unregister("pcmD");
    return 0;
}


static rvm_dev_t *card_init(driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)rvm_hal_device_new(drv, sizeof(card_dev_t), id);
    snd_card_drv_t *card_drv = (snd_card_drv_t *)drv;
    aos_mixer_elem_t *elem;
    csi_error_t ret = -1;


    ret = csi_codec_init(&codec_a, C_IDX);
    ret |= csi_codec_init(&codec_p, P_IDX);
    ret |= csi_codec_init(&codec_pdm, PDM_IDX);
    if (ret != CSI_OK)
        printf("csi_codec_init error\n");

    //FIXME:  must sleep 500ms at least before PA ON, otherwise baoyin happens
    aos_pcm_register();
    slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec0", &elem_codec1_ops);
    slist_add(&elem->next, &card_drv->mixer_head);

    return (rvm_dev_t *)card;
}

static void card_uninit(rvm_dev_t *dev)
{

    csi_codec_uninit(&codec_a);
    csi_codec_uninit(&codec_p);
    csi_codec_uninit(&codec_pdm);
    aos_pcm_unregister();
    rvm_hal_device_free(dev);
}

static int card_open(rvm_dev_t *dev)
{

    return 0;
}

static int card_close(rvm_dev_t *dev)
{

    return 0;
}

static int card_lpm(rvm_dev_t *dev, int state)
{

    return 0;
}

static snd_card_drv_t snd_card_drv = {
        .drv = {
            .name   = "card",
            .init   = card_init,
            .uninit = card_uninit,
            .open   = card_open,
            .close  = card_close,
            .lpm    = card_lpm,
        },
 };

void snd_card_register(void *config)
{
    rvm_driver_register(&snd_card_drv.drv, NULL, 0);
}

void snd_card_unregister(void *config)
{
    rvm_driver_unregister("card");
}















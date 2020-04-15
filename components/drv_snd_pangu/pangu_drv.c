/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>

#include <drv/codec.h>

#define TAG "snd"

#define pcm_uninit device_free
#define mixer_uninit device_free

#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)


#define DAC_WRITE_EVENT  (0x01)
#define ADC_READ_EVENT   (0x02)

#define DAC_GAIN_MIN     (-31)
#define DAC_GAIN_MAX     (-5)   // FIXME: because of pangu C2 board.

typedef struct {
    codec_output_t *hdl;
    int state;
} playback_t;

typedef struct {
    codec_input_t *hdl;
    int channels;
    char *recv;
    int state;
} capture_t;

typedef struct {
    codec_output_t hdl;
    int l;
    int r;
} mixer_playback_t;

mixer_playback_t mixp0;

static void playback_free(playback_t *playback)
{
    if (playback->state == 1) {
        csi_codec_output_stop(playback->hdl);
        csi_codec_output_close(playback->hdl);
        aos_free(playback->hdl->buf);
        aos_free(playback->hdl);
        playback->state = 0;
        playback->hdl = 0;
    }
}

static int pcmp_open(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = aos_zalloc(sizeof(playback_t));

    CHECK_RET_TAG_WITH_RET(NULL != playback, -1);
    pcm->hdl = playback;
    playback->state = 0;

    return 0;
}

static int pcmp_close(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);
    aos_free(playback);

    return 0;
}

static void codec_event_cb(int idx, codec_event_t event, void *arg)
{
    aos_pcm_t *pcm = (aos_pcm_t *)arg;

    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        pcm->event.cb(pcm, PCM_EVT_WRITE, pcm->event.priv);
    } else if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        pcm->event.cb(pcm, PCM_EVT_READ, pcm->event.priv);
    } else {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    }
}

static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);

    codec_output_t *codec = aos_zalloc(sizeof(codec_output_t));
    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

    uint8_t *send = aos_malloc(params->buffer_bytes);
    if (send == NULL) {
        goto pcmp_err0;
    }

    codec->buf = send;
    codec->buf_size = params->buffer_bytes;
    codec->cb = codec_event_cb;
    codec->cb_arg = pcm;
    codec->ch_idx = pcm->pcm_name[4] - 0x30;
    codec->codec_idx = 0;
    codec->period = params->period_bytes;

    int ret = csi_codec_output_open(codec);
    if (ret != 0) {
        goto pcmp_err1;
    }

    codec_output_config_t config;
    config.bit_width = params->sample_bits;
    config.mono_mode_en = params->channels == 1? 1 : 0;
    config.sample_rate = params->rate;

    ret = csi_codec_output_config(codec, &config);
    if (ret != 0) {
        goto pcmp_err1;
    }

    csi_codec_output_start(codec);

    if (mixp0.hdl.cb == NULL) {
        memcpy(&mixp0.hdl, codec, sizeof(codec_output_t));

        if (mixp0.l == -31 || mixp0.r == -31) {
            csi_codec_output_mute(codec, 1);
        } else {
            csi_codec_output_mute(codec, 0);
            csi_codec_output_set_mixer_left_gain(codec, mixp0.l);
            csi_codec_output_set_mixer_right_gain(codec, mixp0.r);
        }
    }

    playback->state = 1;
    playback->hdl = codec;

    return 0;
pcmp_err1:
    aos_free(send);
pcmp_err0:
    aos_free(codec);

    return -1;
}

static int pcm_send(aos_pcm_t *pcm, void *data, int len)
{
    playback_t *playback = (playback_t *)pcm->hdl;

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

/* left_gain/right_gain [-31, 0] 1dB step*/
static int snd_set_gain(aos_mixer_elem_t *elem, int l, int r)
{

    if (mixp0.hdl.cb != NULL) {
        codec_output_t *p = &mixp0.hdl;

        if (l == -31 || r == -31) {
            csi_codec_output_mute(p, 1);
        } else {
            csi_codec_output_mute(p, 0);
            csi_codec_output_set_mixer_left_gain(p, l);
            csi_codec_output_set_mixer_right_gain(p, r);
        }
    } else {
        mixp0.l = l;
        mixp0.r = r;
    }

    return 0;
}

static int snd_volume_to_dB(aos_mixer_elem_t *elem, int val)
{
    int gain;
    int gain_s = DAC_GAIN_MIN, gain_e = DAC_GAIN_MAX;

    //FIXME
    gain = gain_s + (val / 100.0 * (gain_e - gain_s));
    return gain;
}


static sm_elem_ops_t elem_codec1_ops = {
    .set_dB       = snd_set_gain,
    .volume_to_dB = snd_volume_to_dB,
};

static aos_dev_t *pcm_init(driver_t *drv, void *config, int id)
{
    aos_pcm_dev_t *pcm_dev = (aos_pcm_dev_t *)device_new(drv, sizeof(aos_pcm_dev_t), id);
    aos_pcm_drv_t *pcm_drv = (aos_pcm_drv_t *)drv;

    memset(&pcm_dev->pcm, 0x00, sizeof(aos_pcm_t));
    pcm_dev->pcm.ops = &(pcm_drv->ops);

    return (aos_dev_t *)(pcm_dev);
}

static void capture_free(capture_t *capture)
{
    if (capture->state == 1) {

        codec_input_t *codec = capture->hdl;
        for (int i = 0; i < capture->channels; i++) {
            if (codec) {
                csi_codec_input_stop(codec);
                csi_codec_input_close(codec);
            }
            codec ++;
        }

        // aos_free(capture->hdl->buf);
        aos_free(capture->recv);
        aos_free(capture->hdl);
        capture->state = 0;
        capture->hdl = 0;
    }
}

static int pcmc_open(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = aos_zalloc(sizeof(capture_t));

    CHECK_RET_TAG_WITH_RET(NULL != capture, -1);
    pcm->hdl = capture;
    capture->state = 0;

    return 0;
}

static int pcmc_close(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);
    aos_free(capture);
    return 0;
}

const static int g_input_ch_idx[] = {0, 1, -1, 2};
volatile int g_input_flag = 0;

static void input_event_cb(int idx, codec_event_t event, void *arg)
{
    aos_pcm_t *pcm = (aos_pcm_t *)arg;

    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        g_input_flag |= (1 << idx);
        if (g_input_flag == 0x07) {
            pcm->event.cb(pcm, PCM_EVT_READ, pcm->event.priv);
            g_input_flag = 0;
        }
    } else {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    }

}

static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

    capture->channels = params->channels;
    capture->hdl = aos_zalloc_check(sizeof(codec_input_t) * capture->channels);
    CHECK_RET_TAG_WITH_RET(NULL != capture->hdl, -1);

    capture->recv = aos_zalloc_check(params->buffer_bytes * capture->channels);
    if (capture->recv == NULL) {
        goto pcmc_err0;
    }

    codec_input_t *codec = capture->hdl;
    uint8_t *recv = (uint8_t *)capture->recv;

    codec_input_config_t config;
    config.bit_width = params->sample_bits;
    config.sample_rate = params->rate;
    config.channel_num = 1;

    int buffer_bytes = params->buffer_bytes / capture->channels;
    int period = params->period_bytes / capture->channels;

    for (int i = 0; i < capture->channels; i++) {
        codec->buf = recv + i * buffer_bytes;
        codec->cb = input_event_cb;
        codec->ch_idx = g_input_ch_idx[i];
        codec->buf_size = buffer_bytes;
        codec->cb_arg = pcm;
        codec->codec_idx = 0;
        codec->period = period;

        if (codec->ch_idx == -1) {
            codec ++;
            continue;
        }

        int ret = csi_codec_input_open(codec);
        if (ret != 0) {
            goto pcmc_err1;
        }

        ret = csi_codec_input_config(codec, &config);
        if (ret != 0) {
            goto pcmc_err1;
        }

        if (codec->ch_idx == 0 || codec->ch_idx == 1) {//mic
            csi_codec_input_set_analog_gain(codec, 6);
        } else {//ref
            csi_codec_input_set_analog_gain(codec, 0);
        }

        codec ++;
    }

    codec = capture->hdl;
    for (int i = 0; i < capture->channels; i++) {
        if (codec->ch_idx != -1)
            csi_codec_input_start(codec);
        codec ++;
    }

    capture->state = 1;

    return 0;

pcmc_err1:
    aos_free(capture->hdl);
pcmc_err0:
    aos_free(capture->hdl);

    return -1;
}

static int pcm_recv(aos_pcm_t *pcm, void *buf, int size)
{
    capture_t *capture = (capture_t *)pcm->hdl;
    int ret = -1;
    int i;

    codec_input_t *codec = capture->hdl;
    char *recv = buf;
    int recv_size = size / capture->channels;

    for (i = 0; i < capture->channels; i++) {
        if (codec->ch_idx != -1) {
            ret = csi_codec_input_read(codec, (uint8_t *)recv, recv_size);

            if (ret != recv_size) {
                asm("bkpt");
            }
        } else {
            memset(recv, 0x00, recv_size);
            ret = recv_size;
        }

        if (ret != recv_size) {
            break;
        }

        recv += recv_size;
        codec ++;
    }

    return (ret * i);
}

static aos_pcm_drv_t aos_pcm_drv[] = {
    {
        .drv = {
            .name               = "pcmP",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcmp_open,
            .close              = pcmp_close,
        },
        .ops = {
            .hw_params_set      = pcmp_param_set,
            .write              = pcm_send,
            .pause              = pcm_pause,
        },
    },
    {
        .drv = {
            .name = "pcmC",
            .init = pcm_init,
            .uninit = pcm_uninit,
            .open = pcmc_open,
            .close = pcmc_close,
        },
        .ops = {
            .hw_params_set = pcmc_param_set,
            .read = pcm_recv,
        },
    }
};

static int aos_pcm_register(void)
{
    driver_register(&aos_pcm_drv[0].drv, NULL, 0);
    driver_register(&aos_pcm_drv[1].drv, NULL, 0);
    // driver_register(&aos_pcm_drv[1].drv, NULL, 2);
    // driver_register(&aos_pcm_drv[1].drv, NULL, 1);

    return 0;
}

static int aos_pcm_unregister(void)
{
    driver_unregister("pcmP0");

    return 0;
}

static aos_dev_t *card_init(driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)device_new(drv, sizeof(card_dev_t), id);
    snd_card_drv_t *card_drv = (snd_card_drv_t *)drv;
    aos_mixer_elem_t *elem;

    csi_codec_init(id);
    //FIXME:  must sleep 500ms at least before PA ON, otherwise baoyin happens
    aos_msleep(500);
    aos_pcm_register();
    slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec0", &elem_codec1_ops);
    slist_add(&elem->next, &card_drv->mixer_head);

    return (aos_dev_t *)card;
}

static void card_uninit(aos_dev_t *dev)
{

    //TODO free mixer elem;
    aos_pcm_unregister();
    device_free(dev);
}

static int card_open(aos_dev_t *dev)
{

    return 0;
}

static int card_close(aos_dev_t *dev)
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
    }
};

void snd_pangu_card_register(int vol_range)
{
    driver_register(&snd_card_drv.drv, NULL, 0);
}

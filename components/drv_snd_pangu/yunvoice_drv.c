/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <devices/driver.h>
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
#ifdef CONFIG_CSI_V2
    csi_codec_output_t *hdl;
#else
    codec_output_t *hdl;
#endif
    aos_pcm_hw_params_t params;
    int state;

    /* patch for baoyin */
    int pause;
    int wcnt;
    int wcnt_threshold;
} playback_t;

typedef struct {
#ifdef CONFIG_CSI_V2
    csi_codec_input_t *hdl;
#else
    codec_input_t *hdl;
#endif
    aos_pcm_hw_params_t params;
    int state;
} capture_t;

typedef struct {
#ifdef CONFIG_CSI_V2
    csi_codec_output_t hdl;
#else
    codec_output_t hdl;
#endif
    int l;
    int r;
    int mute;
} mixer_playback_t;

#ifdef CONFIG_CSI_V2
static csi_codec_t codec_a;
#endif

mixer_playback_t mixp0;

static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params);
static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params);

static void playback_free(playback_t *playback)
{
    if (playback->state == 1) {
        csi_codec_output_stop(playback->hdl);
        csi_codec_output_close(playback->hdl);

#ifdef CONFIG_CSI_V2
        aos_free(playback->hdl->ring_buf->buffer);
        aos_free(playback->hdl->ring_buf);
#else
        aos_free(playback->hdl->buf);
#endif

        aos_free(playback->hdl);
        playback->state = 0;
        playback->hdl = 0;
    }
}

static int pcmp_lpm(aos_dev_t *dev, int state)
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

#ifdef CONFIG_CSI_V2
static void codec_event_cb(csi_codec_input_t *codec, csi_codec_event_t event, void *arg)
#else
static void codec_event_cb(int idx, codec_event_t event, void *arg)
#endif
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

//FIXME: frequent switching of mute will cause popping
static void _csi_codec_output_mute(int mute)
{
#ifdef CONFIG_CSI_V2
    csi_codec_output_t *p = &mixp0.hdl;
#else
    codec_output_t *p = &mixp0.hdl;
#endif

    if (p && (mute != mixp0.mute)) {
        csi_codec_output_mute(p, mute);
        mixp0.mute = mute;
    }
}

static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);

#ifdef CONFIG_CSI_V2
    csi_codec_output_config_t output_config;
    csi_codec_output_t *codec = aos_zalloc(sizeof(csi_codec_output_t));
    codec->ring_buf = aos_zalloc(sizeof(ringbuffer_t));
#else
    codec_output_t *codec = aos_zalloc(sizeof(codec_output_t));
#endif

    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

    uint8_t *send = aos_malloc(params->buffer_bytes);
    if (send == NULL) {
        goto pcmp_err0;
    }

#ifdef CONFIG_CSI_V2
    int ret = csi_codec_output_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmp_err1;
    }

    csi_codec_output_attach_callback(codec, codec_event_cb, pcm);

    output_config.bit_width = params->sample_bits;
    output_config.sample_rate = params->rate;
    output_config.buffer = send;
    output_config.buffer_size = params->buffer_bytes;
    output_config.period = params->period_bytes;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = params->channels;
    ret = csi_codec_output_config(codec, &output_config);
    if (ret != 0) {
        goto pcmp_err1;
    }
#else
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

#endif
    csi_codec_output_start(codec);
#ifdef CONFIG_CSI_V2

    if (mixp0.hdl.callback == NULL)
#else

    if (mixp0.hdl.cb == NULL)
#endif
    {
#ifdef CONFIG_CSI_V2
        memcpy(&mixp0.hdl, codec, sizeof(csi_codec_output_t));
#else
        memcpy(&mixp0.hdl, codec, sizeof(codec_output_t));
#endif
    }

    if (mixp0.l == -31 || mixp0.r == -31) {
        _csi_codec_output_mute(1);
    } else {
        _csi_codec_output_mute(0);

#ifdef CONFIG_CSI_V2
        csi_codec_output_analog_gain(codec, mixp0.l);

#else
        csi_codec_output_set_analog_left_gain(codec, mixp0.l);
        csi_codec_output_set_analog_left_gain(codec, mixp0.r);
#endif
    }

    playback->state = 1;
    playback->hdl   = codec;
    /* patch for baoyin */
    playback->wcnt_threshold = params->rate / 1000 * params->channels * params->sample_bits / 8 * 40;
    memcpy(&playback->params, params, sizeof(aos_pcm_hw_params_t));

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
    if (ret > 0 && playback->pause) {
        playback->wcnt += ret;
        if (playback->wcnt >= playback->wcnt_threshold) {
            playback->pause = 0;
            playback->wcnt  = 0;
            _csi_codec_output_mute(0);
        }
    }

    return ret;
}

static int pcm_pause(aos_pcm_t *pcm, int enable)
{
    playback_t *playback = (playback_t *)pcm->hdl;

    //FIXME: patch for xiaoya on mixer
    if (enable) {
        playback->pause = 1;
        playback->wcnt  = 0;
        _csi_codec_output_mute(1);
    } else {
#ifdef CONFIG_CSI_V2
        memset(playback->hdl->ring_buf->buffer, 0, playback->hdl->ring_buf->size);
#else
        memset(playback->hdl->buf, 0, playback->hdl->buf_size);
#endif
    }

    return 0;
}

/* left_gain/right_gain [-31, 0] 1dB step*/
static int snd_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
#ifdef CONFIG_CSI_V2

    if (mixp0.hdl.callback != NULL)
#else
    if (mixp0.hdl.cb != NULL)
#endif
    {

#ifdef CONFIG_CSI_V2
        csi_codec_output_t *p = &mixp0.hdl;
#else
        codec_output_t *p = &mixp0.hdl;
#endif

        if (l == -31 || r == -31) {
            _csi_codec_output_mute(1);
        } else {
            _csi_codec_output_mute(0);
#ifdef CONFIG_CSI_V2
            csi_codec_output_analog_gain(p, l);
#else
            csi_codec_output_set_analog_left_gain(p, l);
            csi_codec_output_set_analog_right_gain(p, l);
#endif
        }
        mixp0.l = l;
        mixp0.r = r;
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
        csi_codec_input_stop(capture->hdl);
        csi_codec_input_close(capture->hdl);
#ifdef CONFIG_CSI_V2
        aos_free(capture->hdl->ring_buf->buffer);
        aos_free(capture->hdl->ring_buf);
#else
        aos_free(capture->hdl->buf);
#endif
        aos_free(capture->hdl);
        capture->state = 0;
        capture->hdl = 0;
    }
}

static int pcmc_lpm(aos_dev_t *dev, int state)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    if (state > 0) {
        capture_free(capture);
    } else {
        pcmc_param_set(pcm, &capture->params);
    }

    return 0;
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

static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

#ifdef CONFIG_CSI_V2
    csi_codec_input_config_t input_config;
    csi_codec_input_t *codec = aos_zalloc(sizeof(csi_codec_input_t));
#else
    codec_input_t *codec = aos_zalloc(sizeof(codec_input_t));
#endif
    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

    uint8_t *recv = aos_malloc(params->buffer_bytes);
    if (recv == NULL) {
        goto pcmc_err0;
    }

#ifdef CONFIG_CSI_V2

    // pcm->pcm_name[4] is ch ascii data, change to decimal num
    codec->ring_buf = aos_zalloc(sizeof(ringbuffer_t));
    int ret = csi_codec_input_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmc_err1;
    }

    /* input ch config */
    csi_codec_input_attach_callback(codec, codec_event_cb, pcm);
    input_config.bit_width = params->sample_bits;
    input_config.sample_rate = params->rate;
    input_config.buffer = recv;
    input_config.buffer_size = params->buffer_bytes;
    input_config.period = params->period_bytes;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = 1;
    ret = csi_codec_input_config(codec, &input_config);
    if (ret != 0) {
        goto pcmc_err1;
    }
#else
    // pcm->pcm_name[4] is ch ascii data, change to decimal num
    codec->buf = recv;
    codec->buf_size = params->buffer_bytes;
    codec->cb = codec_event_cb;
    codec->cb_arg = pcm;
    codec->ch_idx = pcm->pcm_name[4] - 0x30;
    codec->codec_idx = 0;
    codec->period = params->period_bytes;

    int ret = csi_codec_input_open(codec);
    if (ret != 0) {
        goto pcmc_err1;
    }

    codec_input_config_t config;
    config.bit_width = params->sample_bits;
    config.sample_rate = params->rate;
    config.channel_num = 1;
    ret = csi_codec_input_config(codec, &config);
    if (ret != 0) {
        goto pcmc_err1;
    }
#endif

#ifdef CONFIG_CSI_V2

    if (codec->ch_idx == 0) {//mic
        csi_codec_input_analog_gain(codec, 6);
    } else {//ref
        csi_codec_input_analog_gain(codec, 0);
    }

#else

    if (codec->ch_idx == 0) {//mic
        csi_codec_input_set_analog_gain(codec, 6);
    } else {//ref
        csi_codec_input_set_analog_gain(codec, 0);
    }

#endif

    csi_codec_input_start(codec);

    capture->state = 1;
    capture->hdl = codec;
    memcpy(&capture->params, params, sizeof(aos_pcm_hw_params_t));

    return 0;

pcmc_err1:
    aos_free(recv);
pcmc_err0:
    aos_free(codec);

    return -1;
}

static int pcm_recv(aos_pcm_t *pcm, void *buf, int size)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    int ret = csi_codec_input_read(capture->hdl, (uint8_t *)buf, size);

    return ret;
}

int pcmc_get_remain_size(aos_pcm_t *pcm)
{
    capture_t *capture = (capture_t *)pcm->hdl;

#ifdef CONFIG_CSI_V2
    csi_codec_input_t *codec = capture->hdl;
#else
    codec_input_t *codec = capture->hdl;
#endif

    int ret = 0x7fffffff;

    // if (codec->ch_idx >= 0) {
    volatile int avail = 0;
#ifdef CONFIG_CSI_V2
    avail = csi_codec_input_buffer_avail(codec);
#else
    if (codec->ch_idx < 0) {
        return ret;
    }
    avail = csi_codec_input_buf_avail(codec);
#endif
    if (avail < ret) {
        ret = avail;
    }
    // }

    return ret;
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
            .hw_params_set = pcmc_param_set,
            .read = pcm_recv,
            //.hw_get_remain_size = pcmc_get_remain_size,
        },
    }
};

static int aos_pcm_register(void)
{
    driver_register(&aos_pcm_drv[0].drv, NULL, 0);
    driver_register(&aos_pcm_drv[1].drv, NULL, 0);
    driver_register(&aos_pcm_drv[1].drv, NULL, 2);
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
#ifdef CONFIG_CSI_V2
    csi_error_t ret;
    ret = csi_codec_init(&codec_a, 0);
    if (ret != CSI_OK) {
        printf("csi_codec_init error\n");
    }

#else
    csi_codec_init(id);
#endif
    //FIXME:  must sleep 500ms at least before PA ON, otherwise baoyin happens
    aos_msleep(500);
    aos_pcm_register();
    slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec0", &elem_codec1_ops);
    slist_add(&elem->next, &card_drv->mixer_head);
    mixp0.mute = -1;

    return (aos_dev_t *)card;
}

static void card_uninit(aos_dev_t *dev)
{

#ifdef CONFIG_CSI_V2
    csi_codec_uninit(&codec_a);
#else
    //TODO free mixer elem;
    csi_codec_uninit(dev->id);
#endif

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

static int card_lpm(aos_dev_t *dev, int state)
{
#ifdef CONFIG_CSI_V2
    extern void ck_codec_lpm(uint32_t idx, uint32_t state);
    if( state > 0) {
        ck_codec_lpm(dev->id, 1);
    } else {
        ck_codec_lpm(dev->id, 0);
    }
#else

    if (state > 0) {
        csi_codec_lpm(dev->id, CODEC_MODE_SLEEP);
    } else {
        csi_codec_lpm(dev->id, CODEC_MODE_RUN);
    }

#endif

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
    }
};

void snd_card_register(void *config)
{
    driver_register(&snd_card_drv.drv, NULL, 0);
}

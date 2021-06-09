/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>

#include <drv/codec.h>
#include <devices/drv_snd_pangu.h>
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
} playback_t;

typedef struct {
#ifdef CONFIG_CSI_V2
    csi_codec_input_t *hdl;
#else
    codec_input_t *hdl;
#endif
    aos_pcm_hw_params_t params;
    int channels;
    char *recv;
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

mixer_playback_t mixp0;
static int g_dac_gain_max, g_dac_gain_min;
#ifdef CONFIG_CSI_V2
static csi_codec_t codec_a;
static ringbuffer_t output_ring_buffer;
static ringbuffer_t input_ring_buffer;
#endif
static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params);
static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params);

static void playback_free(playback_t *playback)
{
    if (playback->state == 1) {
        csi_codec_output_stop(playback->hdl);
        csi_codec_output_close(playback->hdl);
#ifdef CONFIG_CSI_V2
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

    if (playback) {
        pcm->hdl = playback;
        playback->state = 0;
        return 0;
    }

    return -1;
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
    if (codec == NULL) {
        return -1;
    }
    codec->ring_buf = &output_ring_buffer;
#else
    codec_output_t *codec = aos_zalloc(sizeof(codec_output_t));
    if (codec == NULL) {
        return -1;
    }
#endif
    uint8_t *send = aos_malloc(params->buffer_bytes);

    if (!(send)) {
        goto pcmp_err0;
    }

#ifdef CONFIG_CSI_V2
    csi_codec_output_open(&codec_a, codec, 0);
    csi_codec_output_attach_callback(codec, codec_event_cb, NULL);
    output_config.bit_width = params->sample_bits;
    output_config.sample_rate = params->rate;
    output_config.buffer = send;
    output_config.buffer_size = params->buffer_bytes;
    output_config.period = params->period_bytes;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = params->channels;

    int ret = csi_codec_output_config(codec, &output_config);
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
    config.mono_mode_en = params->channels == 1 ? 1 : 0;
    config.sample_rate = params->rate;

    ret = csi_codec_output_config(codec, &config);

    if (ret != 0) {
        goto pcmp_err1;
    }

#endif
    csi_codec_output_start(codec);

#ifdef CONFIG_CSI_V2
    if (mixp0.hdl.callback == NULL) {
        memcpy(&mixp0.hdl, codec, sizeof(csi_codec_output_t));

        csi_codec_output_t *p = &mixp0.hdl;
#else
    if (mixp0.hdl.cb == NULL) {
        memcpy(&mixp0.hdl, codec, sizeof(codec_output_t));

        codec_output_t *p = &mixp0.hdl;
#endif
        int tmp_r = mixp0.r;
        int tmp_l = mixp0.l;

        if (tmp_l <= g_dac_gain_min || tmp_r <= g_dac_gain_min) {
            _csi_codec_output_mute(1);
        } else {
            _csi_codec_output_mute(0);

#ifdef CONFIG_CSI_V2
            csi_codec_output_analog_gain(p, tmp_l < -31 ? -31 : tmp_l);
#else
            csi_codec_output_set_analog_left_gain(p, tmp_l < -31 ? -31 : tmp_l);
            csi_codec_output_set_analog_right_gain(p, tmp_r < -31 ? -31 : tmp_r);
#endif
            tmp_l = tmp_l >= -31 ? 0 : tmp_l + 31;
            tmp_r = tmp_r >= -31 ? 0 : tmp_r + 31;

#ifdef CONFIG_CSI_V2
            csi_codec_output_mix_gain(p, tmp_l);
#else
            csi_codec_output_set_mixer_left_gain(p, tmp_l);
            csi_codec_output_set_mixer_right_gain(p, tmp_r);

#endif
        }
    }

    playback->state = 1;
    playback->hdl = codec;
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

/* left_gain/right_gain [g_dac_gain_min, g_dac_gain_max] 1dB step*/
static int snd_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
#ifdef CONFIG_CSI_V2

    if (mixp0.hdl.callback != NULL) {
        csi_codec_output_t *p = &mixp0.hdl;
#else

    if (mixp0.hdl.cb != NULL) {
        codec_output_t *p = &mixp0.hdl;
#endif
        int tmp_r = r;
        int tmp_l = l;

        if (tmp_l <= g_dac_gain_min || tmp_r <= g_dac_gain_min) {
            _csi_codec_output_mute(1);
        } else {
            _csi_codec_output_mute(0);
#ifdef CONFIG_CSI_V2
            csi_codec_output_analog_gain(p, tmp_l < -31 ? -31 : tmp_l);
#else
            csi_codec_output_set_analog_left_gain(p, tmp_l < -31 ? -31 : tmp_l);
            csi_codec_output_set_analog_right_gain(p, tmp_r < -31 ? -31 : tmp_r);
#endif

            tmp_l = tmp_l >= -31 ? 0 : tmp_l + 31;
            tmp_r = tmp_r >= -31 ? 0 : tmp_r + 31;
#ifdef CONFIG_CSI_V2
            csi_codec_output_mix_gain(p, tmp_l);
#else
            csi_codec_output_set_mixer_left_gain(p, tmp_l);
            csi_codec_output_set_mixer_right_gain(p, tmp_r);
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
    int gain_s = g_dac_gain_min, gain_e = g_dac_gain_max;

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
#ifdef CONFIG_CSI_V2
        csi_codec_input_t *codec = capture->hdl;
#else
        codec_input_t *codec = capture->hdl;
#endif

        for (int i = 0; i < capture->channels; i++) {
            if (codec) {
#ifdef CONFIG_CSI_V1
                if (codec->ch_idx >= 0)
#endif
                {
                    csi_codec_input_stop(codec);
                    csi_codec_input_close(codec);
                }
                codec ++;
            }
        }

        // aos_free(capture->hdl->buf);
        aos_free(capture->recv);
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

    if (capture) {
        pcm->hdl = capture;
        capture->state = 0;
        return 0;
    }

    return -1;
}

static int pcmc_close(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);
    aos_free(capture);
    return 0;
}

static int g_input_ch_idx[PANGU_MAX_CHANNELS];
static int g_input_ch_flags = 0;
volatile int g_input_flag = 0;
#ifdef CONFIG_CSI_V2
static void input_event_cb(int idx, csi_codec_event_t event, void *arg)

#else
static void input_event_cb(int idx, codec_event_t event, void *arg)
#endif
{
    aos_pcm_t *pcm = (aos_pcm_t *)arg;

    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        g_input_flag |= (1 << idx);

        if (g_input_flag == g_input_ch_flags) {
            pcm->event.cb(pcm, PCM_EVT_READ, pcm->event.priv);
            g_input_flag = 0;
        }
    } else {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
        g_input_flag = 0;
    }
}

static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

    capture->channels = params->channels;
#ifdef CONFIG_CSI_V2
    csi_error_t ret;
    capture->hdl = aos_zalloc_check(sizeof(csi_codec_input_t) * capture->channels);
#else
    capture->hdl = aos_zalloc_check(sizeof(codec_input_t) * capture->channels);
#endif
    capture->recv = aos_zalloc_check(params->buffer_bytes * capture->channels);

    if (capture->recv == NULL) {
        goto pcmc_err0;
    }

#ifdef CONFIG_CSI_V2
    csi_codec_input_t *codec = capture->hdl;
    csi_codec_input_config_t input_config;
    codec->ring_buf = &input_ring_buffer;
#else
    codec_input_t *codec = capture->hdl;
    codec_input_config_t config;
#endif
    uint8_t *recv = (uint8_t *)capture->recv;

#ifdef CONFIG_CSI_V2
    codec->ring_buf = &input_ring_buffer;
    int buffer_bytes = params->buffer_bytes / capture->channels;
    int period = params->period_bytes / capture->channels;
    /* input ch config */

    for (int i = 0; i < capture->channels; i++) {
        csi_codec_input_attach_callback(codec, input_event_cb, NULL);
        input_config.bit_width = params->sample_bits;
        input_config.sample_rate = params->rate;
        input_config.buffer = recv + i * buffer_bytes;
        input_config.buffer_size = buffer_bytes;
        input_config.period = period;
        input_config.mode = CODEC_INPUT_DIFFERENCE;
        input_config.sound_channel_num = 1;

        if (codec->ch_idx == -1) {
            codec ++;
            continue;
        }

        csi_codec_input_config(codec, &input_config);

        ret = csi_codec_input_open(&codec_a, codec, g_input_ch_idx[i]);
        if (ret != CSI_OK) {
            goto pcmc_err1;
        }

        ret =  csi_codec_input_config(codec, &input_config);
        if (ret != CSI_OK) {
            goto pcmc_err1;
        }

        if (codec->ch_idx == 4) {//mic
            csi_codec_input_analog_gain(codec, 0);
        } else {//ref
            csi_codec_input_analog_gain(codec, 6);
        }

        codec ++;
    }

#else
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

        if (codec->ch_idx == 4) {//mic
            csi_codec_input_set_analog_gain(codec, 0);
        } else {//ref
            csi_codec_input_set_analog_gain(codec, 6);
        }

        codec ++;
    }

#endif
    aos_kernel_sched_suspend();
    codec = capture->hdl;

    for (int i = 0; i < capture->channels; i++) {
        if (codec->ch_idx != -1) {
            csi_codec_input_start(codec);
        }

        codec ++;
    }

    aos_kernel_sched_resume();

    capture->state = 1;
    memcpy(&capture->params, params, sizeof(aos_pcm_hw_params_t));

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
    static int cnt = 0;

#ifdef CONFIG_CSI_V2
    csi_codec_input_t *codec = capture->hdl;
#else
    codec_input_t *codec = capture->hdl;
#endif

    char *recv = buf;
    int recv_size = size / capture->channels;

    for (i = 0; i < capture->channels; i++) {
        if (codec->ch_idx != -1) {
            ret = csi_codec_input_read(codec, (uint8_t *)recv, recv_size);

            if (ret != recv_size) {
                asm("bkpt");

                if (100 == ++cnt) {
                    printf("\n\n>> Warning codec_input_read(%d,%d)\n\n", ret, recv_size);
                    cnt = 0;
                }
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

static int pcmc_get_remain_size(aos_pcm_t *pcm)
{
    capture_t *capture = (capture_t *)pcm->hdl;

#ifdef CONFIG_CSI_V2
    csi_codec_input_t *codec = capture->hdl;
#else
    codec_input_t *codec = capture->hdl;
#endif

    int ret = 0x7fffffff;

    for (int i = 0; i < capture->channels; i++) {

#ifdef CONFIG_CSI_V2
        if (codec->ch_idx != 0xffffffff ) {
#else
        if (codec->ch_idx >= 0 ) {
#endif
            int avail = 0;
#ifdef CONFIG_CSI_V2
            avail = csi_codec_input_buffer_avail(codec);
#else
            avail = csi_codec_input_buf_avail(codec);
#endif
            if (avail < ret) {
                ret = avail;
            }
        }

        codec ++;
    }

    return ret * capture->channels;
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
            .hw_get_remain_size = pcmc_get_remain_size,
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
    memset(&mixp0, 0, sizeof(mixer_playback_t));
    mixp0.mute = -1;

    return (aos_dev_t *)card;
}

static void card_uninit(aos_dev_t *dev)
{
    //TODO free mixer elem;

#ifdef CONFIG_CSI_V2
    csi_codec_uninit(&codec_a);

#else
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
#else

    if (state == 1) {
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

void snd_card_pangu_register(void *config)
{
    snd_pangu_config_t *pangu_config = (snd_pangu_config_t *)config;

    aos_check_param(pangu_config->id_list && pangu_config->id_num > 0 && pangu_config->id_num <= PANGU_MAX_CHANNELS);

    g_input_ch_flags = 0;

    for (int i = 0; i < PANGU_MAX_CHANNELS; ++i) {
        if (pangu_config->id_list[i] < 0 || i >= pangu_config->id_num) {
            g_input_ch_idx[i] = -1;
            continue;
        }

        g_input_ch_flags |= 1 << pangu_config->id_list[i];
        g_input_ch_idx[i] = pangu_config->id_list[i];
    }

    if (pangu_config->dac_db_max < 0 && pangu_config->dac_db_max >= -62 &&
        pangu_config->dac_db_min < 0 && pangu_config->dac_db_min >= -62 &&
        pangu_config->dac_db_min < pangu_config->dac_db_max) {
        g_dac_gain_max = pangu_config->dac_db_max;
        g_dac_gain_min = pangu_config->dac_db_min;
    }

    driver_register(&snd_card_drv.drv, NULL, 0);
}

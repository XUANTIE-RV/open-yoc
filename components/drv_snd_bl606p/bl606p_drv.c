/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <devices/driver.h>
#include <drv/codec.h>

#include <devices/drv_snd_bl606p.h>

static snd_bl606p_config_t *g_audio_gain_config = NULL;

#define TAG "snd"

#define pcm_uninit rvm_hal_device_free
#define mixer_uninit rvm_hal_device_free

#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)


#define DAC_WRITE_EVENT  (0x01)
#define ADC_READ_EVENT   (0x02)

#define DAC_GAIN_MIN     (-31)
#define DAC_GAIN_MAX     (-5)   // FIXME: because of pangu C2 board.

#define CONST_TX_OUTPUT_BUFSIZE     (17*1024)
#define CONST_RX_INPUT_BUFSIZE      (12*1536)

typedef struct {
    csi_codec_output_t *hdl;
    csi_dma_ch_t       *dma_hdl;
    aos_pcm_hw_params_t params;
    csi_codec_output_config_t *output_config;
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

mixer_playback_t mixp0;

static int pcmp_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params);
static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params);

static void playback_free(playback_t *playback)
{
    if (playback->state == 1) {
        csi_codec_output_stop(playback->hdl);
        csi_codec_output_link_dma(playback->hdl, NULL);
        csi_codec_output_close(playback->hdl);

        aos_free(playback->hdl->ring_buf->buffer);
        aos_free(playback->hdl->ring_buf);

        aos_free(playback->hdl);
        aos_free(playback->dma_hdl);
        
        aos_free(playback->output_config->buffer);
        aos_free(playback->output_config);
        
        playback->state   = 0;
        playback->state   = 0;
        playback->state   = 0;
        playback->hdl     = NULL;
        playback->dma_hdl = NULL;
        playback->output_config->buffer = NULL;
        playback->output_config = NULL;
    }
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

volatile int g_tx_cnt;
static void codec_event_cb(csi_codec_input_t *codec, csi_codec_event_t event, void *arg)
{
    aos_pcm_t *pcm = (aos_pcm_t *)arg;

    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        g_tx_cnt ++;
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

    csi_codec_output_t *codec = aos_zalloc(sizeof(csi_codec_output_t));
    codec->ring_buf = aos_zalloc(sizeof(dev_ringbuf_t));
    codec->sound_channel_num = 2;

    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

#ifdef CONST_TX_OUTPUT_BUFSIZE
    uint8_t *send = aos_malloc(CONST_TX_OUTPUT_BUFSIZE);
#else
    uint8_t *send = aos_malloc(params->buffer_bytes);
#endif
    if (send == NULL) {
        goto pcmp_err0;
    }

    playback->output_config = aos_malloc(sizeof(csi_codec_output_config_t));
    if (playback->output_config == NULL) {
        goto pcmp_err0;
    }

    int ret = csi_codec_output_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmp_err1;
    }

    csi_codec_output_attach_callback(codec, codec_event_cb, pcm);

    playback->output_config->bit_width = params->sample_bits;
    playback->output_config->sample_rate = params->rate;
    playback->output_config->buffer = send;

#ifdef CONST_TX_OUTPUT_BUFSIZE
    playback->output_config->buffer_size = CONST_TX_OUTPUT_BUFSIZE;
#else
    playback->output_config->buffer_size = params->buffer_bytes;
#endif
    playback->output_config->period = 2400;
    //output_config.period = 2048;
    playback->output_config->mode = CODEC_OUTPUT_SINGLE_ENDED;
    playback->output_config->sound_channel_num = params->channels;
    ret = csi_codec_output_config(codec, playback->output_config);
    if (ret != 0) {
        goto pcmp_err1;
    }

    if (mixp0.hdl.callback == NULL) {
        memcpy(&mixp0.hdl, codec, sizeof(csi_codec_output_t));
    }

    //snd_set_gain will init mixp0.l
    extern int auo_analog_gain(void *context, float val);
    auo_analog_gain(NULL, (float)mixp0.l);
    // csi_codec_output_buffer_reset(codec);

    csi_dma_ch_t *dma_hdl = aos_zalloc_check(sizeof(csi_dma_ch_t));
    csi_codec_output_link_dma(codec, dma_hdl);

    playback->state         = 1;
    playback->hdl           = codec;
    playback->dma_hdl       = dma_hdl;
    memcpy(&playback->params, params, sizeof(aos_pcm_hw_params_t));

    csi_codec_output_start(codec);
    printf("-------------------------code init ok\r\n");
    return 0;
pcmp_err1:
    aos_free(send);
pcmp_err0:
    aos_free(codec);

    return -1;
}

//#define DUMP_DATA_FEATURE
#ifdef DUMP_DATA_FEATURE
#define DUMP_DATA_LEN (256*1024)
uint8_t g_csi_data[DUMP_DATA_LEN];
int g_csi_len;
#endif

static int pcm_send(aos_pcm_t *pcm, void *data, int len)
{
    playback_t *playback = (playback_t *)pcm->hdl;
    int ret = csi_codec_output_write_async(playback->hdl, (uint8_t *)data, len);

#ifdef DUMP_DATA_FEATURE
    if (ret + g_csi_len > DUMP_DATA_LEN) {
        // g_csi_len = -1;
    } else {
        memcpy(g_csi_data + g_csi_len, (uint8_t*)data, ret);
        g_csi_len += ret;                    
    }
#endif
    // printf("---------------------(%d)\r\n", ret);
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
    //l r is dB value, Convert to csi api
    if (mixp0.hdl.callback != NULL) {
        printf(">>> snd_set_gain %d %d dB\r\n", l, r);
        //csi_codec_output_t *p = &mixp0.hdl;
        //csi_codec_output_analog_gain(p, output_db2idx(l));
        extern int auo_analog_gain(void *context, float val);
        auo_analog_gain(NULL, (float)l);

    } else {
        printf(">>> snd_set_gain default %d %d dB\r\n", l, r);
    }

    mixp0.l = l;
    mixp0.r = r;

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
        csi_codec_input_close(capture->hdl);
        aos_free(capture->hdl->ring_buf->buffer);
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

#ifdef CONST_RX_INPUT_BUFSIZE
    uint8_t *recv = aos_malloc(CONST_RX_INPUT_BUFSIZE);
#else
    uint8_t *recv = aos_malloc(params->buffer_bytes);
#endif
    if (recv == NULL) {
        goto pcmc_err0;
    }

    // pcm->pcm_name[4] is ch ascii data, change to decimal num
    codec->ring_buf = aos_zalloc(sizeof(dev_ringbuf_t));
    int ret = csi_codec_input_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmc_err1;
    }

    /* input ch config */
    csi_codec_input_attach_callback(codec, codec_event_cb, pcm);
    input_config.bit_width = params->sample_bits;
    input_config.sample_rate = params->rate;
    input_config.buffer = recv;
#ifdef CONST_RX_INPUT_BUFSIZE
    input_config.buffer_size = CONST_RX_INPUT_BUFSIZE;
#else
    input_config.buffer_size = params->buffer_bytes;
#endif
    input_config.period = params->period_bytes;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = params->channels;

    printf("input bit_width(%d), sample_rate(%d), buffer_size(%d), period(%d), sound_channel_num(%d)\r\n", \
            input_config.bit_width, input_config.sample_rate, input_config.buffer_size, input_config.period, input_config.sound_channel_num);
    ret = csi_codec_input_config(codec, &input_config);
    if (ret != 0) {
        goto pcmc_err1;
    }

    if (g_audio_gain_config) {
        /*Fixme: csi接口未实现*/
        extern int _aui_analog_gain(int id, int32_t val);
        for (int i = 0; i < 3; i++) {
            _aui_analog_gain(i, g_audio_gain_config->audio_in_gain_list[i]);
        }
    } else {
        //csi_codec_input_analog_gain(codec, 0x2f);
        //csi_codec_input_digital_gain(codec, 25);
        // csi_codec_input_digital_gain(codec, 17);
    }

    csi_dma_ch_t *dma_hdl = aos_zalloc_check(sizeof(csi_dma_ch_t));
    csi_codec_input_link_dma(codec, dma_hdl);

    csi_codec_input_start(codec);

    capture->state = 1;
    capture->hdl = codec;
    capture->dma_hdl = dma_hdl;
    memcpy(&capture->params, params, sizeof(aos_pcm_hw_params_t));

    printf("csi codec input open success\r\n");
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

    return csi_codec_input_buffer_avail(capture->hdl);
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
    rvm_driver_register(&aos_pcm_drv[0].drv, NULL, 0);
    rvm_driver_register(&aos_pcm_drv[1].drv, NULL, 0);
    // rvm_driver_register(&aos_pcm_drv[1].drv, NULL, 2);
    // rvm_driver_register(&aos_pcm_drv[1].drv, NULL, 1);

    return 0;
}

static int aos_pcm_unregister(void)
{
    rvm_driver_unregister("pcmP0");

    return 0;
}

static rvm_dev_t *card_init(driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)rvm_hal_device_new(drv, sizeof(card_dev_t), id);
    snd_card_drv_t *card_drv = (snd_card_drv_t *)drv;
    aos_mixer_elem_t *elem;

    csi_error_t ret;
    ret = csi_codec_init(&codec_a, 0);
    if (ret != CSI_OK) {
        printf("csi_codec_init error\n");
    }

    //FIXME:  must sleep 500ms at least before PA ON, otherwise baoyin happens
    //aos_msleep(500);
    aos_pcm_register();
    slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec0", &elem_codec1_ops);
    slist_add(&elem->next, &card_drv->mixer_head);

    return (rvm_dev_t *)card;
}

static void card_uninit(rvm_dev_t *dev)
{
    csi_codec_uninit(&codec_a);
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
    }
};

void snd_card_bl606p_register(void *config)
{
    if (config) {
        if (g_audio_gain_config == NULL) {
            g_audio_gain_config = (snd_bl606p_config_t*)malloc(sizeof(snd_bl606p_config_t));
            *g_audio_gain_config = *((snd_bl606p_config_t*)config);
        } else {
            /* 再次调用注册,仅仅更新参数 */
            *g_audio_gain_config = *((snd_bl606p_config_t*)config);
            return;
        }
    }
    rvm_driver_register(&snd_card_drv.drv, NULL, 0);
}

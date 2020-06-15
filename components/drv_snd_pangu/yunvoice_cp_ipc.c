/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <ipc.h>
#include <drv/codec.h>
#include <csi_core.h>

#include "card_ipc_def.h"

#define TAG "snd"

#define MESSAGE_NUM 10
typedef struct {
    codec_output_t *hdl;
    int state;
} playback_t;

typedef struct {
    codec_input_t *hdl;
    int state;
} capture_t;

typedef struct {
    aos_queue_t              queue;
    aos_task_t               ipc_task;
    uint8_t                  buffer[sizeof(message_t) * MESSAGE_NUM];
    aos_task_t               task;
    ipc_t                   *ipc;
    playback_t              *playback;
    int                      snd_init;
    aos_event_t              evt;
} card_ipc_t;

#define pcm_uninit device_free
#define mixer_uninit device_free

#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)


#define OUTPUT_WRITE_EVENT  (0x01)
#define INPUT_READ_EVENT   (0x02)

static int ipc_cmd_send(ipc_t *ipc, int cmd, void *data, int len, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.command = cmd;
    msg.service_id = CARD_IPC_ID;
    msg.req_data = data;
    msg.req_len = len;
    msg.flag = sync;

    ipc_message_send(ipc, &msg, AOS_WAIT_FOREVER);

    return 0;
}

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

static int ipc_pcmp_open(card_ipc_t *c, void *data, int len)
{
    playback_t *playback = aos_zalloc(sizeof(playback_t));

    CHECK_RET_TAG_WITH_RET(NULL != playback, -1);
    c->playback = playback;
    playback->state = 0;

    return 0;
}

static int ipc_pcmp_close(card_ipc_t *c, void *data, int len)
{
    playback_t *playback = (playback_t *)c->playback;

    playback_free(playback);

    return 0;
}

static void codec_event_cb(int idx, codec_event_t event, void *arg)
{
    card_ipc_t *c = (card_ipc_t *)arg;

    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE || \
        event == CODEC_EVENT_WRITE_BUFFER_EMPTY) {
        aos_event_set(&c->evt, OUTPUT_WRITE_EVENT,  AOS_EVENT_OR);
    if (event == CODEC_EVENT_WRITE_BUFFER_EMPTY) {
        printf("empty evt\r\n");
        ipc_cmd_send(c->ipc, PCMP_WRITE_EMPTY_CMD, NULL, 0, 0);
    }
    }
}

static int ipc_pcmp_param_set(card_ipc_t *c, void *data, int len)
{
    playback_t *playback = (playback_t *)c->playback;
    aos_pcm_hw_params_t *params = (aos_pcm_hw_params_t *)data;

    printf("param(%d)(%d)(%d)(%d)(%d)\n",params->buffer_bytes, params->period_bytes, params->sample_bits, params->channels, params->rate);
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
    codec->cb_arg = c;
    codec->ch_idx = 0;
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

    csi_codec_output_set_analog_left_gain(codec, -10);
    csi_codec_output_set_analog_right_gain(codec, -10);
    playback->state = 1;
    playback->hdl = codec;

    return 0;
pcmp_err1:
    aos_free(send);
pcmp_err0:
    aos_free(codec);

    return -1;
}

static int ipc_pcm_send(card_ipc_t *c, void *data, int len)
{
    playback_t *playback = (playback_t *)c->playback;

    printf("write(%p)(%d)\r\n", data, len);
    int ret = -1;
    char *send = (char *)data;
    // int *send_len = (int *)data;
    unsigned int flag;
    // int snd_len = len;

    while (len) {
        ret = csi_codec_output_write(playback->hdl, (uint8_t *)send, len);
        if (ret < len) {
            aos_event_get(&c->evt, OUTPUT_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
        }

        len -= ret;
        send += ret;
    }

    // *send_len = snd_len;
    // csi_dcache_clean_range((uint32_t *)send_len, 4);

    return ret;
}

static int ipc_pcm_pause(card_ipc_t *c, void *data, int len)
{
    playback_t *playback = (playback_t *)c->playback;
    int enable = (int)data;

    if (enable) {
        csi_codec_output_pause(playback->hdl);
    } else {
        csi_codec_output_resume(playback->hdl);
    }

    return 0;
}

/* left_gain/right_gain [-31, 0] 1dB step*/
static int ipc_snd_set_gain(card_ipc_t *c, void *data, int len)
{
    playback_t *playback = (playback_t *)c->playback;
    int *gain = (int *)data;

    printf("mixer(%p),(%d)(%d)\r\n", gain, gain[0], gain[1]);
    if (playback->hdl) {
        csi_codec_output_set_digital_left_gain(playback->hdl, gain[0]);
        csi_codec_output_set_digital_right_gain(playback->hdl, gain[1]);
    }

    return 0;
}
#if 1
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
        aos_free(capture->hdl->buf);
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
    return 0;
}

static void input_event_cb(int idx, codec_event_t event, void *arg)
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

static int pcmc_param_set(aos_pcm_t *pcm, struct aos_pcm_hw_params *params)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

    codec_input_t *codec = aos_zalloc(sizeof(codec_input_t));
    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

    uint8_t *recv = aos_malloc(params->buffer_bytes);
    if (recv == NULL) {
        goto pcmc_err0;
    }

    codec->buf = recv;
    codec->buf_size = params->buffer_bytes;
    codec->cb = input_event_cb;
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

    if (codec->ch_idx == 0) {//mic
        csi_codec_input_set_analog_gain(codec, 6);
    } else {//ref
        csi_codec_input_set_analog_gain(codec, 0);
    }
    csi_codec_input_start(codec);

    capture->state = 1;
    capture->hdl = codec;

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

static aos_pcm_drv_t aos_pcm_drv[] = {
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
    driver_register(&aos_pcm_drv[0].drv, NULL, 2);
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

    csi_codec_init(id);
    aos_pcm_register();

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

#endif

static int ipc_card_init(card_ipc_t *c, void *data, int len)
{
    int id = (int)data;

    printf("init:%d\r\n", id);
    // csi_codec_init(id);
    c->snd_init = 1;

    return 0;
}

static int ipc_card_uninit(card_ipc_t *c, void *data, int len)
{

    return 0;
}

static int ipc_card_open(card_ipc_t *c, void *data, int len)
{

    return 0;
}

static int ipc_card_close(card_ipc_t *c, void *data, int len)
{

    return 0;
}

typedef int (*ipc_fun)(card_ipc_t *c, void *data, int len);
typedef struct {
    uint16_t cmd;
    ipc_fun fun;
} codec_ipc_dac_msg_t;

const codec_ipc_dac_msg_t c_ipc_msg_table[] = {
    { CARD_INIT_CMD, ipc_card_init},
    { CARD_DEINIT_CMD, ipc_card_uninit},
    { CARD_OPEN_CMD, ipc_card_open},
    { CARD_CLOSE_CMD, ipc_card_close},
    { PCMP_INIT_CMD, NULL},
    { PCMP_DEINIT_CMD, NULL},
    { PCMP_OPEN_CMD, ipc_pcmp_open},
    { PCMP_CLOSE_CMD, ipc_pcmp_close},
    { PCMP_PARAM_SET_CMD, ipc_pcmp_param_set},
    { PCMP_WRITE_CMD, ipc_pcm_send},
    { PCMP_PAUSE_CMD, ipc_pcm_pause},
    { MIXER_SET_GAIN_CMD, ipc_snd_set_gain},
};
#define IPC_MSG_TABLE_LEN (sizeof(c_ipc_msg_table) / sizeof(codec_ipc_dac_msg_t))

const char *test[] = {
    "CARD_INIT",
    "CARD_DEINIT",
    "CARD_OPEN",
    "CARD_CLOSE",
    "PCMP_INIT",
    "PCMP_DEINIT",
    "PCMP_OPEN",
    "PCMP_CLOSE",
    "PCMP_PARAM_SET",
    "PCMP_WRITE",
    "PCMP_REMAIN_SIZE",
    "PCMP_PAUSE",
    "MIXER_GAIN"
};
static void _msg_process(card_ipc_t *c, message_t *msg)
{
    codec_ipc_dac_msg_t *p = (codec_ipc_dac_msg_t *)&c_ipc_msg_table[0];

    for (int i = 0; i < IPC_MSG_TABLE_LEN; i++) {
        if (msg->command == p[i].cmd) {
            // printf("msg(%d):%s\r\n", msg->command, test[msg->command]);
            if (p[i].fun) {
                p[i].fun(c, msg->req_data, msg->req_len);
            }
            break;
        }
    }

    if (msg->flag & MESSAGE_SYNC) {
        ipc_message_ack(c->ipc, msg, AOS_WAIT_FOREVER);
    }
}

static void _ipc_process(ipc_t *ipc, message_t *msg, void *priv)
{
    card_ipc_t *c = (card_ipc_t *)priv;
    _msg_process(c, msg);
}

static int card_ipc_init(card_ipc_t *c)
{
    c->ipc = ipc_get(0);
    ipc_add_service(c->ipc, CARD_IPC_ID, _ipc_process, c);

    return 0;
}
card_ipc_t card_ipc;
void snd_card_cp_register(int vol_range)
{
    // card_ipc_t *card_ipc = (card_ipc_t *) aos_zalloc_check(sizeof(card_ipc_t));
    aos_event_new(&card_ipc.evt, 0);
    card_ipc_init(&card_ipc);
    driver_register(&snd_card_drv.drv, NULL, 0);
}

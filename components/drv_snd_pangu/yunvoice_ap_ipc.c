/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <yoc_config.h>
#include <ipc.h>
#include <stdio.h>
#include <csi_core.h>

#include "card_ipc_def.h"

#define TAG "card_ipc"

#define pcm_uninit device_free
#define mixer_uninit device_free

#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)

#define MESSAGE_NUM 10

#define DAC_GAIN_MIN     (-31)
#define DAC_GAIN_MAX     (0)

typedef struct {
    aos_queue_t              queue;
    aos_task_t               ipc_task;
    uint8_t                  buffer[sizeof(message_t) * MESSAGE_NUM];
    aos_task_t               task;
    ipc_t                   *ipc;
    void                    *priv;
} card_ipc_t;

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

static aos_dev_t *pcm_init(driver_t *drv, void *config, int id)
{
    aos_pcm_dev_t *pcm_dev = (aos_pcm_dev_t *)device_new(drv, sizeof(aos_pcm_dev_t), id);
    aos_pcm_drv_t *pcm_drv = (aos_pcm_drv_t *)drv;

    memset(&pcm_dev->pcm, 0x00, sizeof(aos_pcm_t));
    pcm_dev->pcm.ops = &(pcm_drv->ops);
    pcm_dev->pcm.hdl = config;

    return (aos_dev_t *)(pcm_dev);
}

static int pcm_p_open(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    card_ipc_t *c = (card_ipc_t *)pcm->hdl;

    ipc_cmd_send(c->ipc, PCMP_OPEN_CMD, NULL, 0, MESSAGE_SYNC);
    c->priv = pcm;

    return 0;
}

static int pcm_close(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    card_ipc_t *c = (card_ipc_t *)pcm->hdl;

    ipc_cmd_send(c->ipc, PCMP_CLOSE_CMD, NULL, 0, MESSAGE_SYNC);
    return 0;
}


static int pcm_send(aos_pcm_t *pcm, void *data, int len)
{
    card_ipc_t *c = (card_ipc_t *)pcm->hdl;

    ipc_cmd_send(c->ipc, PCMP_WRITE_CMD, data, len, MESSAGE_SYNC);

    return len;
}


static int pcm_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    card_ipc_t *c = (card_ipc_t *)pcm->hdl;

    ipc_cmd_send(c->ipc, PCMP_PARAM_SET_CMD, params, sizeof(aos_pcm_hw_params_t), MESSAGE_SYNC);

    return 0;
}

static int pcm_get_buffer_size(aos_pcm_t *pcm)
{

    return 0;
}

static int pcm_get_remain_size(aos_pcm_t *pcm)
{
    // card_ipc_t *c = (card_ipc_t *)pcm->hdl;
    void *data = NULL;

    // data = ipc_cmd_send(c->ipc, PCMP_REMAIN_SIZE_CMD, NULL, 0, MESSAGE_SYNC);

    return (int)data;
}

static int pcm_pause(aos_pcm_t *pcm, int enable)
{
    card_ipc_t *c = (card_ipc_t *)pcm->hdl;

    ipc_cmd_send(c->ipc, PCMP_PAUSE_CMD, (void *)enable, 1, MESSAGE_SYNC);

    return 0;
}

/* left_gain/right_gain [-31, 0] 1dB step*/
static int mixer_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
    card_ipc_t *c = (card_ipc_t *)elem->hdl;
    int gain[2];

    gain[0] = l;
    gain[1] = r;

    ipc_cmd_send(c->ipc, MIXER_SET_GAIN_CMD, (void *)gain, 8, MESSAGE_SYNC);

    return 0;
}

static int mixer_volume_to_dB(aos_mixer_elem_t *elem, int val)
{
    int gain;
    int gain_s = DAC_GAIN_MIN, gain_e = DAC_GAIN_MAX;

    //FIXME
    gain = gain_s + (val / 100.0 * (gain_e - gain_s));
    return gain;
}

static sm_elem_ops_t elem_codec1_ops = {
    .set_dB       = mixer_set_gain,
    .volume_to_dB = mixer_volume_to_dB,
};


static aos_pcm_drv_t aos_pcm_drv[] = {
    {
        .drv = {
            .name               = "pcmP",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcm_p_open,
            .close              = pcm_close,
        },
        .ops = {
            .hw_params_set      = pcm_param_set,
            .write              = pcm_send,
            .hw_get_size        = pcm_get_buffer_size,
            .hw_get_remain_size = pcm_get_remain_size,
            .pause              = pcm_pause,
        },
    },
};

static int aos_pcm_register(void *config)
{
    driver_register(&aos_pcm_drv[0].drv, config, 0);

    return 0;
}

static void _msg_process(card_ipc_t *c, message_t *msg)
{
    int cmd = msg->command;
    aos_pcm_t *pcm = (aos_pcm_t *)c->priv;

    if (cmd == PCMP_WRITE_EMPTY_CMD) {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
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

static void card_ipc_init(card_ipc_t *c)
{
    c->ipc = ipc_get(2);
    ipc_add_service(c->ipc, CARD_IPC_ID, _ipc_process, c);
}

static void card_codec_init(card_ipc_t *c)
{
    ipc_cmd_send(c->ipc, CARD_INIT_CMD, NULL, 0, MESSAGE_SYNC);
}

static void card_codec_deinit(card_ipc_t *c)
{
    ipc_cmd_send(c->ipc, CARD_DEINIT_CMD, NULL, 0, MESSAGE_SYNC);
}

static int card_open(aos_dev_t *dev)
{
    return 0;
}

static int card_close(aos_dev_t *dev)
{
    //TODO
    return 0;
}

static aos_dev_t *card_init(driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)device_new(drv, sizeof(card_dev_t), id);
    card_ipc_t *card_ipc = (card_ipc_t *) aos_zalloc_check(sizeof(card_ipc_t));
    snd_card_drv_t *card_drv = (snd_card_drv_t *)drv;
    aos_mixer_elem_t *elem;

    card_ipc_init(card_ipc);
    card_codec_init(card_ipc);

    card->priv = card_ipc;

    aos_pcm_register(card_ipc);
    slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec1", &elem_codec1_ops);
    elem->hdl = card_ipc;
    slist_add(&elem->next, &card_drv->mixer_head);

    return (aos_dev_t *)card;
}

void card_uninit(aos_dev_t *dev)
{ //TODO
    card_dev_t *card = (card_dev_t *)dev;
    card_ipc_t *card_ipc = card->priv;

    card_codec_deinit(card_ipc);
    aos_free(card_ipc);
    device_free(dev);
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

void snd_card_ap_register(int vol_range)
{
    driver_register(&snd_card_drv.drv, NULL, 0);
}

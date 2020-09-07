/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <ipc.h>
#include <csi_core.h>
#include <drv/codec.h>
#include <aos/list.h>

#include "voice.h"

#define TAG "Vap"

#define MESSAGE_NUM 10

struct __voice {
    voice_pcm_param_t       *mic_param;
    voice_pcm_param_t       *ref_param;
    voice_param_t            param;

    voice_evt_t              cb;
    void                    *priv;
    aos_queue_t              queue;
    aos_task_t               ipc_task;
    uint8_t                  buffer[sizeof(message_t) * MESSAGE_NUM];
    voice_ch_t              *ch;
    voice_ch_io_t           *ops;
    voice_pcm_t             *pcm;
};

static int voice_msg_send(voice_t *v, int cmd, void *data, int len, int sync)
{
    voice_msg_t msg;
    memset(&msg, 0, sizeof(voice_msg_t));
    int ret = -1;

    msg.command = cmd;
    msg.req_data = data;
    msg.req_len = len;
    msg.flag = sync;

    if (v->ops->msg_send)
        ret = (v->ops->msg_send(v->ch, &msg));

    return ret;
}

static void voice_msg_process(void *priv, voice_msg_t *msg)
{
    int cmd = msg->command;
    voice_t *v = (voice_t*)priv;
    int evt_id = 0;

    if (cmd == VOICE_KWS_CMD) {
        evt_id = VOICE_ASR_EVT;
        LOGD(TAG,"\r\nasr event\r\n");
    } else if (cmd == VOICE_SILENCE_CMD) {
        evt_id = VOICE_SILENCE_EVT;
        LOGD(TAG,"\r\nvad event\r\n");
    } else if (cmd == VOICE_DATA_CMD) {
        evt_id = VOICE_DATA_EVT;
    } else {
        return;
    }

    if (v->cb) {
        v->cb(v->priv, evt_id, msg->req_data, msg->req_len);
    }
}

static void voice_data_send(void *priv, void *data, int len)
{
    voice_t *v = (voice_t *)priv;

    voice_msg_send(v, VOICE_PCM_CMD, data, len, 1);
}

static void voice_pcm_param_init(voice_t *v)
{
    voice_pcm_param_t *p = v->mic_param;
    if (p) {
        p->period_bytes = p->channles_sum * p->rate * p->sample_bits / 8 * v->param.cts_ms / 1000;
    }

    p = v->ref_param;
    if (p) {
        p->period_bytes = p->channles_sum * p->rate * p->sample_bits / 8 * v->param.cts_ms / 1000;
    }

    // voice_dcache_writeback((uint32_t *)v, sizeof(voice_t));
    voice_dcache_writeback((uint32_t *)v->mic_param, sizeof(voice_pcm_param_t));
    voice_dcache_writeback((uint32_t *)v->ref_param, sizeof(voice_pcm_param_t));
    voice_msg_send(v, VOICE_PCM_PARAM_SET_CMD, (void *)v, sizeof(voice_t), 1);
    if (v->param.ipc_mode == 1) {
        v->pcm = pcm_init(voice_data_send, v);
        pcm_mic_config(v->pcm, v->mic_param);
        pcm_ref_config(v->pcm, v->ref_param);
        pcm_start(v->pcm);
    }
}

static void voice_cp_start(voice_t *v)
{
    voice_msg_send(v, VOICE_CP_START_CMD, NULL, 0, 1);
}

static void voice_ch_init(voice_t *v)
{
    if (v->param.ipc_mode) {
        v->ops = ipc_ch_get();
        v->ch = v->ops->init(voice_msg_process, v);
    } else {
        // TODO

    }
}

static void voice_ch_deinit(voice_t *v)
{

}

voice_t *voice_init(voice_evt_t cb, void *priv)
{
    voice_t *v = voice_malloc(sizeof(voice_t));
    memset(v, 0x00, sizeof(voice_t));
    // int ret;
    v->cb = cb;
    v->priv = priv;
    // ai_init();

    return v;
}

void voice_deinit(voice_t *v)
{//TODO
    voice_ch_deinit(v);
    pcm_deinit(v->pcm);
    aos_free(v);
}

int voice_config(voice_t *v, voice_param_t *p)
{
    aos_check_return_einval(v && p);

    memcpy(&v->param, p, sizeof(voice_param_t));

    return 0;
}

int voice_add_mic(voice_t *v, voice_pcm_param_t *p)
{
    if (v->mic_param == NULL) {
        v->mic_param = voice_malloc(sizeof(voice_pcm_param_t));
    }

    memcpy(v->mic_param, p, sizeof(voice_pcm_param_t));

    return 0;
}

int voice_add_ref(voice_t *v, voice_pcm_param_t *p)
{
    if (v->ref_param == NULL) {
        v->ref_param = voice_malloc(sizeof(voice_pcm_param_t));
    }

    memcpy(v->ref_param, p, sizeof(voice_pcm_param_t));

    return 0;
}

int voice_start(voice_t *v)
{
    voice_ch_init(v);
    voice_pcm_param_init(v);

    voice_cp_start(v);
    return 0;
}

int voice_stop(voice_t *v)
{
    // pcm_close(v->pcm);
    return 0;
}

int voice_backflow_control(voice_t *v, voice_backflow_id_t id, int flag)
{
    int tmep = (id) << 16 | flag;

    voice_msg_send(v, VOICE_BACKFLOW_CONTROL_CMD, &tmep, 4, 1);

    return 0;
}

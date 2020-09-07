/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <ipc.h>
#include <csi_core.h>

#include "voice_ai.h"

#define voice_ops(v) v->ai_ops
#define voice_control(v) v->control

struct __voice {
    voice_pcm_param_t       *mic_param;
    voice_pcm_param_t       *ref_param;
    voice_param_t            param;

    voice_cts_ops_t         *ai_ops;
    void                    *priv;
    aos_task_t               task;
    aos_sem_t                sem;
    ipc_t                   *ipc;
    long long                aec_time_stamp;
    long long                vad_time_stamp;
    long long                asr_time_stamp;
    int                      state;
    volatile int             task_running;
    voice_data_t             *pcm_data;
    voice_data_t             *backflow_data[VOCIE_BACKFLOW_DATA];
    int                      backflow_enable[VOCIE_BACKFLOW_DATA];

    int                      cts_ms; //每次语音识别算法所需的数据量，是否变成一个范围
    int                      silence_tmout;
    int                      silence_start;

    voice_ch_t              *ch;
    voice_ch_io_t           *ops;

    voice_pcm_t             *pcm;
};

#define voice_dcache_writeback csi_dcache_clean_range
#define voice_dcache_invalidate csi_dcache_invalid_range

#define PCM_CACHE_DATA (0x04)

static void voice_entry(void *p);
static void voice_msg_process(void *priv, voice_msg_t *msg);

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

static void pcm_data_update(voice_t *v, void *data, int len)
{
    if (v->pcm_data == NULL) {
        v->pcm_data = aos_malloc_check(len + sizeof(voice_data_t));
    }

    memcpy(v->pcm_data->data, data, len);
    v->pcm_data->len = len;

    aos_sem_signal(&v->sem);
}

static void pcm_backflow_update(voice_t *v, void *data, int len)
{
    int temp = *((int*)data);
    int data_id = ((temp) >> 16) & 0xffff;

    v->backflow_enable[data_id] = temp & 0xffff;
    v->silence_tmout = 1000;
    v->vad_time_stamp = 0;
}

// static void pcm_update(void *priv, void *data, int len)
// {
//     voice_t *v = (voice_t *)priv;

//     pcm_data_update(v, data, len);
// }

static void voice_cp_start(voice_t *v, void *data, int len)
{
    aos_task_new_ext(&v->task, "voice", voice_entry, v, 28 * 1024, AOS_DEFAULT_APP_PRI);
    v->task_running = 1;
}

static void voice_param_set(voice_t *v, void *data, int len)
{
    voice_t *ap = (voice_t *)data;

    voice_dcache_invalidate((uint32_t *)ap->mic_param, sizeof(voice_pcm_param_t));
    voice_dcache_invalidate((uint32_t *)ap->ref_param, sizeof(voice_pcm_param_t));

    if (v->mic_param == NULL) {
        v->mic_param = aos_zalloc_check(sizeof(voice_pcm_param_t));
    }
    memcpy(v->mic_param, ap->mic_param, sizeof(voice_pcm_param_t));

    if (ap->ref_param != NULL) {
    if (v->ref_param == NULL) {
        v->ref_param = aos_zalloc_check(sizeof(voice_pcm_param_t));
    }
        memcpy(v->ref_param, ap->ref_param, sizeof(voice_pcm_param_t));
    } else {
        v->ref_param = NULL;
    }

    memcpy(&v->param, &ap->param, sizeof(voice_param_t));
}

static void voice_ch_init(voice_t *v)
{
    if (v->param.ipc_mode) {
        v->ops = ipc_ch_get();
        v->ch = v->ops->init(voice_msg_process, v);
    } else {

    }
}

static void voice_msg_process(void *priv, voice_msg_t *msg)
{
    int cmd = msg->command;
    voice_t *v = (voice_t *)priv;

    switch (cmd) {
        case VOICE_PCM_CMD:
        if (v->param.ipc_mode == 1) {
            pcm_data_update(v, msg->req_data, msg->req_len);
        }
        break;
        case VOICE_BACKFLOW_CONTROL_CMD:
        pcm_backflow_update(v, msg->req_data, msg->req_len);
        break;
        case VOICE_CP_START_CMD:
        voice_cp_start(v, msg->req_data, msg->req_len);
        break;
        case VOICE_PCM_PARAM_SET_CMD:
        voice_param_set(v, msg->req_data, msg->req_len);
        break;
    }
}

static void voice_ch_deinit(voice_t *v)
{

}

static void *pcm_ref_get(voice_t *v, int ms)
{
    void *ref;
    voice_pcm_param_t *p = v->mic_param;

    int offset = p->period_bytes;

    ref = ((char *)v->pcm_data->data + offset);

    return ref;
}

static void *pcm_mic_get(voice_t *v, int ms)
{
    void *mic = (void **)v->pcm_data->data;

    return mic;
}

static void voice_evt_send(voice_t *v, int evt_id, void *data, int len)
{
    voice_msg_send(v, evt_id, data, len, 0);
}

static void voice_data_send(voice_t *v, int data_id, void *data, int len, int flag)
{
    if (v->backflow_enable[data_id] != 1) {
        return;
    }
    voice_data_t *p = v->backflow_data[data_id];

    if (p == NULL) {
        p = aos_malloc_check(len + sizeof(voice_data_t));
    }
    p->type = data_id;
    p->seq ++;
    p->flag = flag;
    memcpy(p->data, data, len);
    p->len = len;
    v->backflow_data[data_id] = p;

    voice_msg_send(v, VOICE_DATA_CMD, p, len + sizeof(voice_data_t), 1);
}

static int voice_wait_pcm(voice_t *v, int ms)
{
    return (aos_sem_wait(&v->sem, AOS_WAIT_FOREVER));
}

static void voice_entry(void *p)
{
    int ret;
    voice_t *v = (voice_t *)p;
    int ms = v->param.cts_ms;
    void *mic;
    void *ref;
    char *vad_out = aos_malloc_check(v->mic_param->period_bytes);

    voice_ops(v)->init(v->priv);

    while (v->task_running) {
        if (voice_wait_pcm(v, ms) == 0) {
            mic = pcm_mic_get(v, ms);
            ref = pcm_ref_get(v, ms);

            voice_data_send(v, VOICE_MIC_DATA, mic, v->mic_param->period_bytes, 0);
            voice_data_send(v, VOICE_REF_DATA, ref, v->ref_param->period_bytes, 0);

            if (voice_ops(v)->vad) {
                ret = voice_ops(v)->vad(v->priv, mic, ref, ms, vad_out);

                voice_data_send(v, VOICE_VAD_DATA, vad_out, v->mic_param->period_bytes, ret);
                if (v->vad_time_stamp == 0) {
                    v->vad_time_stamp = aos_now_ms();
                }

                if (ret > 0 && v->silence_start) {
                    if ((aos_now_ms() - v->vad_time_stamp) > v->silence_tmout) {
                        voice_evt_send(v, VOICE_SILENCE_CMD, NULL, 0);
                        v->vad_time_stamp = 0;
                        v->silence_start = 0;
                }
            } else {
                    v->vad_time_stamp = aos_now_ms();
                }
            } else {
                ret = -1;
                vad_out = mic;
            }

            voice_data_send(v, VOICE_AEC_DATA, vad_out, v->mic_param->period_bytes, 0);

            if (voice_ops(v)->kws) {
                voice_ops(v)->kws(v->priv, mic, ref, ms, vad_out);
            }

            if (voice_ops(v)->asr && voice_ops(v)->asr(v->priv, vad_out, ms) > 0) {
                v->silence_start = 1;
                v->silence_tmout = 2000;
                v->vad_time_stamp = 0;
                voice_evt_send(v, VOICE_KWS_CMD, NULL, 0);
            }
        }
    }

    voice_ops(v)->deinit(v->priv);
}

voice_t *voice_ai_init(void *priv, voice_cts_ops_t *ops)
{
    aos_check_return_null(ops);
    int ret;

    voice_t *v = aos_zalloc_check(sizeof(voice_t));

    v->ai_ops = ops;
    v->priv   = priv;

    ret = aos_sem_new(&v->sem, 0);
    v->param.ipc_mode = 1;
    voice_ch_init(v);

    if (ret < 0) {
        aos_free(v);
        return NULL;
    }

    return v;
}

void voice_ai_deinit(voice_t *v)
{
    aos_check_return(v);

    voice_ch_deinit(v);

    aos_sem_free(&v->sem);

    v->task_running = 0;

    aos_free(v);
}



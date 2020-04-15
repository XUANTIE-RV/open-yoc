#include <yoc_config.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>

#include <csi_core.h>
#include "yv_defs.h"
#include "yv_cp.h"
#include <ipc.h>

#define yv_dcache_writeback csi_dcache_clean_range
#define yv_dcache_invalidate csi_dcache_invalid_range

#define TAG "CP-YV"

struct _yv_cp_ {
    ipc_t *ipc;
    yv_pcm_param_t *param;

    yv_event_t cb;
    void *priv;
};

static char *ipc_buf_copy(void *data, int len)
{
    return data;
}

static int _pcm_setdata(yv_t *yv, char *data, int len)
{
    int evt_id = YV_PCM_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, data, len);
    }

    return 0;
}

static int _pcm_param_set(yv_t *yv, yv_pcm_param_t *param, int len)
{
    int evt_id = YV_PARM_SET_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, param, len);
    }

    return 0;
}

static int _pcm_param_get(yv_t *yv, yv_pcm_param_t *param, int len)
{
    int evt_id = YV_PARM_GET_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, param, len);
    }

    return 0;
}

static int _vad_enable(yv_t *yv, int enable)
{
    int evt_id = YV_VAD_ENABLE_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, (void *)enable, 1);
    }

    return 0;
}

static int _pcm_enable(yv_t *yv, int enable)
{
    int evt_id = YV_PCM_ENABLE_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, (void *)enable, 1);
    }

    return 0;
}

static int _asr_enable(yv_t *yv, int enable)
{
    int evt_id = YV_ASR_ENABLE_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, (void *)enable, 1);
    }

    return 0;
}

static int  _set_vad_timeout(yv_t *yv, int timeout)
{
    int evt_id = YV_SET_VAD_TIMEOUT_EVT;

    if (yv->cb) {
        yv->cb(yv->priv, evt_id, (void *)timeout, 1);
    }

    return 0;
}

static void *ipc_cmd_send(ipc_t *ipc, int cmd, void *data, int len, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.command = cmd;
    msg.service_id = YV_SERVICE_ID;
    msg.data = ipc_buf_copy(data, len);
    msg.len = len;
    msg.flag = sync;

    //csi_dcache_clean_range(msg.data, msg.len);
    ipc_message_send(ipc, &msg, AOS_WAIT_FOREVER);

    return msg.data;
}


static void _ipc_process(ipc_t *ipc, message_t *msg, void *priv)
{
    yv_t *yv = (yv_t *)priv;

    switch (msg->command) {
        case YV_VAD_ENABLE_CMD: {
            _vad_enable(yv, (int)msg->data);
            break;
        }
        case YV_KWS_ENABLE_CMD: {
            _asr_enable(yv, (int)msg->data);
            break;
        }

        case YV_PCM_ENABLE_CMD: {
            _pcm_enable(yv, (int)msg->data);

            break;
        }

        case YV_VAD_TIMEOUT_CMD: {
            _set_vad_timeout(yv, (int)msg->data);

            break;
        }

        case YV_PCM_SET_CMD: {
            _pcm_param_set(yv, (yv_pcm_param_t *)(msg->data), msg->len);

            break;
        }

        case YV_PCM_GET_CMD: {
            _pcm_param_get(yv, (yv_pcm_param_t *)(msg->data), msg->len);

            break;
        }

        case YV_PCM_DATA_CMD: {
            _pcm_setdata(yv, msg->data, msg->len);

            break;
        }

    }

    if (msg->flag & MESSAGE_SYNC) {

        ipc_message_ack(ipc, msg, AOS_WAIT_FOREVER);
    }
}

static void param_init(yv_pcm_param_t *param)
{
    param->buffer_num = BUFFER_NUM;
    param->buffer_size = BUFFER_SIZE;
    param->sentence_time_ms = SENTENCE_TIME;

    param->channels = CHANNELS;
    param->sample_bits = SAMPLE_BITS;
    param->rate = RATE;
    param->encode = ENCODE;
}

yv_t *yv_init(yv_event_t cb, void *priv)
{
    yv_t *yv = aos_malloc_check(sizeof(yv_t));

    yv->cb = cb;
    yv->priv = priv;

    yv->ipc = ipc_get(AP_CPU_IDX);
    ipc_add_service(yv->ipc, YV_SERVICE_ID, _ipc_process, yv);
    yv->param = aos_malloc_check(sizeof(yv_pcm_param_t));
    param_init(yv->param);

    return yv;
}


int yv_deinit(yv_t *yv)
{
    aos_free(yv->param);
    aos_free(yv);
    return 0;
}

int yv_pcm_send(yv_t *yv, void *pcm, int len)
{
    if (pcm && len > 0) {
        ipc_cmd_send(yv->ipc, YV_PCM_DATA_CMD, pcm, len, MESSAGE_ASYNC);
        return 0;
    }

    return -1;
}

int yv_voice_data_send(yv_t *yv, void *data)
{
    ipc_cmd_send(yv->ipc, YV_VOICE_DATA_CMD, data, sizeof(voice_t), MESSAGE_ASYNC);

    return 0;
}

int yv_asr_send(yv_t *yv, int type)
{
    // LOGI(TAG, "yv_asr_send, type: %d\n", type);
    ipc_cmd_send(yv->ipc, YV_ASR_CMD, (void *)type, sizeof(int), MESSAGE_ASYNC);

    return 0;
}

int yv_vad_send(yv_t *yv, int type)
{
    // LOGI(TAG, "yv_asr_send, type: %d\n", type);
    ipc_cmd_send(yv->ipc, YV_VAD_CMD, (void *)type, sizeof(int), MESSAGE_ASYNC);

    return 0;
}

int yv_sentence_timout(yv_t *yv)
{
    // LOGI(TAG, "yv_sentence_timout\n");
    ipc_cmd_send(yv->ipc, YV_SILENCE_CMD, NULL, 0, MESSAGE_ASYNC);

    return 0;
}



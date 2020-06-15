/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <csi_core.h>
#include <aos/log.h>
#include <aos/list.h>

#include "ipc_v1.h"
#include "yv_ap.h"

#define TAG "YVAP"

#define BUFFER_NUM (8)
#define BUFFER_SIZE (512)
#define SENTENCE_TIME (1000)
#define NOACK_TIME (8000)
#define MAX_TIME (15000)
#define SAMPLE_BITS (16)
#define CHANNELS (2)
#define RATE (16000)
#define ENCODE (0)

#define MAX_SHARED_MEM_NUM  10

struct _yv_ap_ {
    ipc_t *ipc;
    yv_pcm_param_t *param;
    yv_eq_settings_t *eq_parm;

    mic_event_t cb;
    void *priv;
};

typedef struct {
    uint32_t memory_id;
    void    *addr;
    size_t  size;
    char    reserved[4];    // make shared_mem_t 16 bytes to avoide cache problems
} shared_mem_t;

static yv_eq_param_t eq_param __attribute__ ((aligned(64)));

static shared_mem_t shared_mems[MAX_SHARED_MEM_NUM] __attribute__ ((aligned(16)));

static char *ipc_buf_copy(void *data, int len)
{

    return data;
}

static void *ipc_cmd_send(ipc_t *ipc, int cmd, void *data, int len, int sync)
{
    message_t msg;
    message_init(&msg, sync);

    msg.command = cmd;
    msg.data = ipc_buf_copy(data, len);
    msg.len = len;

    csi_dcache_clean_range(msg.data, msg.len);
    ipc_message_put(ipc, &msg, AOS_WAIT_FOREVER);

    if (sync) {
        message_wait(&msg);
        message_deinit(&msg);
        return msg.data;
    } else {
        return NULL;
    }
}

static void ap_ipc_process(ipc_t *ipc, message_t *msg)
{
    switch (msg->command) {
        case YV_KWS_ENABLE_CMD:
        case YV_PCM_ENABLE_CMD:
        case YV_ASR_WAKE_CMD:
        case YV_PCM_FREE_CMD:
        case YV_PCM_SET_CMD:
        case YV_PCM_GET_CMD:
        case YV_EQ_INIT_CMD:
        case YV_EQ_CONFIG_CMD:
        case YV_EQ_PROC_CMD:
        case YV_GET_DSP_VER:
        case YV_DBG_SET_MEMREC_ADDR:
        case YV_OFFLINE_GET_MAP:
        case YV_CONFIG_SHARED_MEM:
    {
            if (msg->sync == 1) {
                message_post(msg);
            }

            break;
        }
    }

}

//event
static void yv_pcm_data_event(yv_t *yv,void *data, int len);
static void yv_asr_event(yv_t *yv, int key_id);
static void yv_silence_event(yv_t *yv);
static void yv_vad_event(yv_t *yv, int value);
static void yv_vad_data_event(yv_t *yv, void *data, int len);
static void yv_kws_data_event(yv_t *yv, void *data, int len);

static void cp_ipc_process(ipc_t *ipc, message_t *msg)
{
    yv_t *yv = (yv_t *)ipc_get_privdata(ipc);

    switch (msg->command) {
        case YV_PCM_DATA_CMD: {
            yv_pcm_data_event(yv, msg->data, msg->len);
            break;
        }
        case YV_ASR_CMD: {
            yv_asr_event(yv, *(int*)msg->data);
            break;
        }
        case YV_SILENCE_CMD: {
            yv_silence_event(yv);
            break;
        }
        case YV_VAD_CMD: {
            yv_vad_event(yv, *(int*)msg->data);
            break;
        }
        case YV_VAD_DATA_CMD: {
            yv_vad_data_event(yv, msg->data, msg->len);
            break;            
        }
        case YV_KWS_DATA_CMD: {
            yv_kws_data_event(yv, msg->data, msg->len);
            break;
        }
        default:
            return;
    }

    if (msg->sync) {
        csi_dcache_clean_range(msg->data, msg->len);
        ipc_message_put(ipc, msg, AOS_WAIT_FOREVER);
    }
}

static void yv_ipc_process(ipc_t *ipc, message_t *msg)
{
    if (ipc == NULL || msg == NULL) {
        return;
    }

    csi_dcache_invalid_range(msg->data, msg->len);

    if (msg->command < YV_CP_CMD_START) {
        ap_ipc_process(ipc, msg);
    } else {
        cp_ipc_process(ipc, msg);
    }
}

static void param_init(yv_pcm_param_t *param)
{
    memset(param, 0, sizeof(yv_pcm_param_t));

    param->sentence_time_ms = SENTENCE_TIME;
    param->noack_time_ms = NOACK_TIME;
    param->max_time_ms      = MAX_TIME;

    param->channels = CHANNELS;
    param->sample_bits = SAMPLE_BITS;
    param->rate = RATE;

    param->nsmode   = 1;
    param->aecmode  = 1;
    param->vadmode  = 3;
}

static yv_pcm_param_t g_yv_param __attribute__ ((aligned(64)));
static yv_t g_yv_inst;
yv_t *yv_init(mic_event_t cb, void *priv)
{
    yv_t *yv = &g_yv_inst;

    yv->cb = cb;
    yv->priv = priv;

    yv->ipc = ipc_create(yv_channel_mailbox_get(), yv_ipc_process, 8, yv);
    yv->param = &g_yv_param;
    param_init(yv->param);

    return yv;
}

yv_t *yv_get_handler()
{
    return &g_yv_inst;
}

int yv_deinit(yv_t *yv)
{
    return 0;
}

//control
static uint8_t ver_dsp[64] __attribute__((aligned(64)));

int yv_get_dsp_version(yv_t *yv, uint8_t **dsp_version)
{
    memset(ver_dsp, 0, 64);
    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_GET_DSP_VER, ver_dsp, sizeof(ver_dsp), 1));

    *dsp_version = ver_dsp;
    return ret;
}

int yv_kws_enable(yv_t *yv, int flag)
{
    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_KWS_ENABLE_CMD, &flag, sizeof(flag), 1));

    return ret;
}

int yv_pcm_enable(yv_t *yv, int flag)
{
    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_PCM_ENABLE_CMD, &flag, sizeof(flag), 1));

    return ret;
}

int yv_wake_trigger(yv_t *yv, int flag)
{
    ipc_cmd_send(yv->ipc, YV_ASR_WAKE_CMD, &flag, sizeof(flag), 1);
    return 0;
}

int yv_dbg_set_memrec_addr(yv_t *yv, int addr)
{
    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_DBG_SET_MEMREC_ADDR, &addr, sizeof(addr), 1));

    return ret;
}

//pcm
int yv_pcm_param_set(yv_t *yv, yv_pcm_param_t *hw_param)
{
    if (hw_param == NULL || yv->param == NULL) {
        return -1;
    }

    *yv->param = *hw_param;

    /* 该函数实现在cb5654开发板组件中，用于获取mic和参考音采集数据 */
extern void *voice_get_adc_data(void);
    yv->param->ext_param1 = voice_get_adc_data();
    //LOGI(TAG, "param_set (%x,%x)(%d,%d,%d)(0x%x)", yv->param->ext_param1, yv->param->ext_param2, 
    //                            yv->param->sentence_time_ms, yv->param->noack_time_ms, yv->param->max_time_ms,
    //                            yv->param->channels);

    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_PCM_SET_CMD, yv->param, sizeof(yv_pcm_param_t), 1));

    return ret;
}

int yv_pcm_param_get(yv_t *yv, yv_pcm_param_t *hw_param)
{
    if (hw_param == NULL || yv->param == NULL) {
        return -1;
    }

    ipc_cmd_send(yv->ipc, YV_PCM_GET_CMD, hw_param, sizeof(yv_pcm_param_t), 0);

    return 0;
}

int yv_pcm_free(yv_t *yv, void *data)
{
    ipc_cmd_send(yv->ipc, YV_PCM_FREE_CMD, data, 4, 0);

    return 0;
}

int yv_eq_init(yv_t *yv, yv_eq_settings_t *eq_settings)
{
    if (eq_settings == NULL) {
        return -1;
    }

    yv->eq_parm = eq_settings;

    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_EQ_INIT_CMD, eq_settings, sizeof(yv_eq_settings_t), 1));

    return ret;
}

int yv_eq_proc(yv_t *yv, void *data, int len)
{
    ipc_cmd_send(yv->ipc, YV_EQ_PROC_CMD, data, len, 1);
    return 0;    
}

int yv_eq_config(yv_t *yv, int channel_num, int sample_rate, int type)
{
    eq_param.channel_num    = channel_num;
    eq_param.sample_rate    = sample_rate;
    eq_param.type           = type;

    int ret = *((int*)ipc_cmd_send(yv->ipc, YV_EQ_CONFIG_CMD, &eq_param, sizeof(yv_eq_param_t), 1));
    return ret;
}

int yv_init_kw_map(yv_t *yv, void **map)
{
    char data[64+3];
    void *p = (void *)(((int)data + 63) & ~0x003f);

    ipc_cmd_send(yv->ipc, YV_OFFLINE_GET_MAP, p, 4, 1);
    /* the map pointer is the buffer content */
    *map = (void *)(*(int *)p);
    
    return 0;
}

int yv_config_share_memory(yv_t *yv, uint32_t memory_id, void *addr, size_t size)
{
    if (memory_id == 0) {
        LOGW(TAG, "shared mem id cannot be 0");
        return -1;
    }

    for (int i = 0; i < MAX_SHARED_MEM_NUM; ++i) {
        if (shared_mems[i].memory_id == memory_id) {
            LOGW(TAG, "memory id conflict");
            return -1;
        }

        if (shared_mems[i].memory_id == 0) {
            shared_mems[i].memory_id = memory_id;
            shared_mems[i].addr = addr;
            shared_mems[i].size = size;
            
            ipc_cmd_send(yv->ipc, YV_CONFIG_SHARED_MEM, shared_mems, sizeof(shared_mem_t) * MAX_SHARED_MEM_NUM, 1);
            return 0;
        }
    }

    return -1;
}

void *yv_get_share_memory(uint32_t memory_id, size_t *size)
{
    if (memory_id == 0) {
        LOGW(TAG, "shared mem id cannot be 0");
        return NULL;
    }    

    for (int i = 0; i < MAX_SHARED_MEM_NUM; ++i) {
        if (shared_mems[i].memory_id == 0) {
            break;
        }

        if (shared_mems[i].memory_id == memory_id) {
            if (size) {
                *size = shared_mems[i].size;
            }
            return shared_mems[i].addr;
        }
    }

    return NULL;
}

/*************************
 * 事件回调处理
*************************/
//extern int g_dsp_version;
static int g_yv_debug = 0;
static mic_event_t g_yv_event_hook = NULL;
void yv_debug_hook(mic_event_t hook, int dbg_level)
{
    g_yv_event_hook = hook;
    g_yv_debug = dbg_level;
}

static int yv_debug_hook_call(int evt_id, void *data, int len)
{
    /* mic yv test hook */
    if (g_yv_debug && g_yv_event_hook) {
        g_yv_event_hook(NULL, evt_id, data, len);

        if (evt_id != YV_VAD_EVT && g_yv_debug != 2) {
            return 0; /* 跳过正常流程,不调用 cb*/
        }
    }

    return 1;
}

#define FRAME_SIZE ((16000 / 1000) * (16 / 8) * 20) /* 640 */
static void yv_pcm_data_event(yv_t *yv, void *data, int len)
{
    int evt_id = YV_PCM_EVT;

/////////////////////
    if (len < FRAME_SIZE) {
        //LOGW(TAG, "wrong frame size %d", len);
        return;
    }

#ifndef CONFIG_MIT_DSP
    int ret = yv_debug_hook_call(evt_id, data, len);

    /* 兼容5路数据 */
    uint8_t * mic_aec = data;
    if (len == (FRAME_SIZE * 5)) {
        mic_aec = &mic_aec[FRAME_SIZE * 4];
    }
/////////////////////

    if (yv->cb && ret) {
        yv->cb(yv->priv, evt_id, mic_aec, FRAME_SIZE);
    }
#else
    yv_debug_hook_call(evt_id, data, len);
#endif

    yv_pcm_free(yv, data);
}

static void yv_vad_event(yv_t *yv, int value)
{
    int evt_id = YV_VAD_EVT;

    int ret = yv_debug_hook_call(evt_id, &value, sizeof(int));

    if (yv->cb && ret) {
        yv->cb(yv->priv, evt_id, &value, sizeof(int));
    }
}

static void yv_vad_data_event(yv_t *yv, void *data, int len)
{
    int evt_id = YV_VAD_DAT_EVT;

    int ret = yv_debug_hook_call(evt_id, data, len);

    evt_id = YV_PCM_EVT;
    if (yv->cb && ret) {
        yv->cb(yv->priv, evt_id, data, len);
    }
}

static void yv_kws_data_event(yv_t *yv, void *data, int len)
{
    int evt_id = YV_KWS_EVT;

    int ret = yv_debug_hook_call(evt_id, data, len);

    if (yv->cb && ret) {
        yv->cb(yv->priv, evt_id, data, len);
    }
}

static void yv_asr_event(yv_t *yv, int type)
{
    int evt_id = YV_ASR_EVT;

    int ret = yv_debug_hook_call(evt_id, &type, sizeof(int));

    if (yv->cb && ret) {
        yv->cb(yv->priv, evt_id, &type, sizeof(int));
    }
}

static void yv_silence_event(yv_t *yv)
{
    int evt_id = YV_SILENCE_EVT;

    int ret = yv_debug_hook_call(evt_id, NULL, 0);

    if (yv->cb && ret) {
        yv->cb(yv->priv, evt_id, NULL, 0);
    }
}

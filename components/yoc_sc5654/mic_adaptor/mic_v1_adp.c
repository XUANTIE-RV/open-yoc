/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/aos.h>
#include <yoc/mic.h>
#include <yoc/mic_port.h>
#include <yv_ap.h>

typedef struct {
    int     wwv_enable;
    int     data_valid;
    void    *data;
    size_t  len;
    char    rsv[48];
} wwv_data_t;

static wwv_data_t wwv_data __attribute__ ((aligned(64)));
static int yv_debug_mode = 0;
static yv_pcm_param_t yv_param;

#define SHM_ID_WWV_DATA     0x1001

static mic_event_t mic_event;

static void mic_adaptor_event_cb(void *priv, mic_event_id_t evt_id, void *data, int size)
{
    if (evt_id == (int)YV_KWS_EVT) {
        mic_event(priv, MIC_EVENT_KWS_DATA, wwv_data.data, wwv_data.len);
        wwv_data.data_valid = 0;      // put data as invalid when got
    } else if (evt_id == (int)YV_ASR_EVT) {
        mic_kws_t kws;

        memset(&kws, 0, sizeof(mic_kws_t));
        kws.id = *(int*)data;
        mic_event(priv, MIC_EVENT_SESSION_START, (void*)&kws, sizeof(mic_kws_t));
    } else {
        mic_event(priv, evt_id, data, size);
    }
}

static int mic_adaptor_init(mic_t *mic, mic_event_t event)
{
    mic_event = event;
    yv_t *yv = yv_init(mic_adaptor_event_cb, (void*)mic);

    if (yv == NULL) {
        return -1;
    }

    mic_set_privdata(yv);

    return 0;
}

static int mic_adaptor_deinit(mic_t *mic)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_deinit(yv);

    return 0;
}

static int mic_adaptor_event_control(mic_t *mic, int flag)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    if (flag & MIC_EVENT_KWS_FLAG) {
        // yv_kws_enable(yv, flag);
        wwv_data.wwv_enable = flag;
        yv_config_share_memory(yv, SHM_ID_WWV_DATA, &wwv_data, sizeof(wwv_data_t));
    }

    return 0;
}

static int mic_adaptor_kws_wake(mic_t *mic, int flag)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_wake_trigger(yv, flag);

    return 0;
}

static int mic_adaptor_pcm_data_control(mic_t *mic, int type, int enable)
{
    if (yv_debug_mode == 2) {
        return -1;
    }

    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_pcm_enable(yv, enable);

    return 0;
}

static int mic_adaptor_pcm_aec_control(mic_t *mic, int flag)
{
    //yv_t *yv = (yv_t *)mic_get_privdata();

    //yv_aec_enable(yv, flag);

    return 0;
}

static int mic_adaptor_debug_control(mic_t *mic, int flag)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_debug_mode = flag;

    if (yv_debug_mode == 2) {
        yv_pcm_enable(yv, 15);
    } else if (yv_debug_mode == 0) {
        yv_pcm_enable(yv, 0);
    }

    return 0;
}

static int mic_adaptor_set_param(mic_t *mic, mic_param_t *hw)
{
    // yv_t *yv = (yv_t *)mic_get_privdata();

    yv_param.channels = hw->channels;
    yv_param.max_time_ms = hw->max_time_ms;
    yv_param.rate     = hw->rate;
    yv_param.sample_bits = hw->sample_bits;
    yv_param.sentence_time_ms = hw->sentence_time_ms;
    yv_param.noack_time_ms = hw->noack_time_ms;

    yv_t *yv = (yv_t *)mic_get_privdata();
    yv_pcm_param_set(yv, &yv_param);
    return 0;
}

int mic_adaptor_ai_ctl(mic_t *mic, int cmd, void *param)
{
    if (cmd == MIC_CTRL_SET_VAD_PARAM) {
        mic_vad_param_t *p = (mic_vad_param_t*)param;

        yv_param.vadmode = p->vadmode;
        yv_param.vadswitch = p->vadswitch;
        yv_param.vadfilter = p->vadfilter;
    } else if (cmd == MIC_CTRL_SET_AEC_PARAM) {
        mic_aec_param_t *p = (mic_aec_param_t *)param;

        yv_param.nsmode = p->nsmode;
        yv_param.aecmode = p->aecmode;
    }

    return 0;
}


static mic_ops_t mic_adp_ops = {
    .init = mic_adaptor_init,
    .deinit = mic_adaptor_deinit,
    .pcm_data_control = mic_adaptor_pcm_data_control,
    .set_param = mic_adaptor_set_param,
    .ai_ctl = mic_adaptor_ai_ctl,
    .debug_control = mic_adaptor_debug_control,
    .event_control = mic_adaptor_event_control,
};

void mic_thead_v1_register(void)
{
    mic_ops_register(&mic_adp_ops);
}

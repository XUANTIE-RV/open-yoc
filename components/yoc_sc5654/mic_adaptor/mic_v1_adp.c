/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/aos.h>
#include <aos/log.h>
#include <yoc/mic.h>
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

#define SHM_ID_WWV_DATA     0x1001

static int mic_adaptor_init(mic_t *mic, mic_event_t event)
{
    yv_t *yv = yv_init(event, (void*)mic);

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

static int mic_adaptor_kws_control(mic_t *mic, int flag)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_kws_enable(yv, flag);

    return 0;
}

static int mic_adaptor_kws_wake(mic_t *mic, int flag)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_wake_trigger(yv, flag);

    return 0;
}

static int mic_adaptor_pcm_data_control(mic_t *mic, int flag)
{
    if (yv_debug_mode == 2) {
        return -1;
    }

    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_pcm_enable(yv, flag);

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

static int mic_adaptor_wwv_enable(mic_t *mic, int flag)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    wwv_data.wwv_enable = flag;
    yv_config_share_memory(yv, SHM_ID_WWV_DATA, &wwv_data, sizeof(wwv_data_t));

    return 0;
}

static int mic_adaptor_wwv_get_data(mic_t *mic, void **data, size_t *size)
{
    if (!wwv_data.wwv_enable || !wwv_data.data_valid) {
        return -1;
    }
    
    *data = wwv_data.data;
    *size = wwv_data.len;
    wwv_data.data_valid = 0;      // put data as invalid when got
    return 0;
}

static int mic_adaptor_set_param(mic_t *mic, void *hw)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_pcm_param_set(yv, hw);

    return 0;
}

static int mic_adaptor_get_param(mic_t *mic, void *hw)
{
    yv_t *yv = (yv_t *)mic_get_privdata();

    yv_pcm_param_get(yv, hw);

    return 0;
}

static mic_ops_t mic_adp_ops = {
    .init = mic_adaptor_init,
    .deinit = mic_adaptor_deinit,
    .kws_control = mic_adaptor_kws_control,
    .kws_wake = mic_adaptor_kws_wake,
    .wwv_enable = mic_adaptor_wwv_enable,
    .wwv_get_data = mic_adaptor_wwv_get_data,
    .pcm_data_control = mic_adaptor_pcm_data_control,
    .pcm_aec_control = mic_adaptor_pcm_aec_control,
    .debug_control = mic_adaptor_debug_control,
    .pcm_set_param = mic_adaptor_set_param,
    .pcm_get_param = mic_adaptor_get_param,
};

void mic_thead_v1_register(void)
{
    mic_ops_register(&mic_adp_ops);
}

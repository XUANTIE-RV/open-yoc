#include <stdlib.h>
#include <aos/aos.h>
#include <aos/log.h>
#include <yoc/mic.h>

#include "huwen_yv_ap.h"

static int mic_adaptor_init(mic_t *mic, mic_event_t event)
{
    int ret;

    ret = huwen_yv_init(event,  mic);

    return ret;
}

static int mic_adaptor_deinit(mic_t *mic)
{
    //mic_get_privdata();

    return 0;
}

static int mic_adaptor_kws_control(mic_t *mic, int flag)
{
    huwen_yv_kws_enable(flag);

    return 0;
}

static int mic_adaptor_kws_wake(mic_t *mic, int flag)
{
    huwen_yv_wake_trigger(flag);

    return 0;
}

static int mic_adaptor_pcm_data_control(mic_t *mic, int flag)
{
    huwen_yv_pcm_enable(flag);

    return 0;
}

static int mic_adaptor_pcm_aec_control(mic_t *mic, int flag)
{
    //yv_t *yv = (yv_t *)mic_get_privdata();

    //yv_aec_enable(yv, flag);

    return 0;
}

static int mic_adaptor_set_param(mic_t *mic, mic_param_t *hw)
{
    //yv_t *yv = (yv_t *)mic_get_privdata();

    //yv_pcm_param_set(yv, hw);

    return 0;
}

static int mic_adaptor_get_param(mic_t *mic, mic_param_t *hw)
{
    //yv_t *yv = (yv_t *)mic_get_privdata();

    //yv_pcm_param_get(yv, hw);

    return 0;
}

static mic_ops_t mic_adp_ops = {
    .init = mic_adaptor_init,
    .deinit = mic_adaptor_deinit,
    .kws_control = mic_adaptor_kws_control,
    .kws_wake = mic_adaptor_kws_wake,
    .pcm_data_control = mic_adaptor_pcm_data_control,
    .pcm_aec_control = mic_adaptor_pcm_aec_control,
    .pcm_set_param = mic_adaptor_set_param,
    .pcm_get_param = mic_adaptor_get_param,
};

void mic_huwen_register(void)
{
    mic_ops_register(&mic_adp_ops);
}

/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/bt.h>
#include <bluetooth/audio_output.h>

#if (defined(CONFIG_AV_AO_ALSA) && CONFIG_AV_AO_ALSA)
#include <av/output/ao.h>
#include <av/avutil/sf.h>
#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_A2DP)
#define LOG_MODULE_NAME bt_a2dp
#include "common/log.h"

#ifndef CONFIG_RESAMPLE_RATE
#define CONFIG_RESAMPLE_RATE 48000
#endif

static ao_cls_t *ao;
static sf_t sf;
static u32_t frame_len;

static int audio_output_init(void)
{
    BT_DBG("");

    return 0;
}

static int audio_output_conf(audio_output_conf_t *conf)
{
    frame_len = conf->frame_len;

    sf = sf_make_channel(conf->channel) | sf_make_rate(conf->sample_rate)
         | sf_make_bit(16) | sf_make_signed(1);

    return 0;
}

static int audio_output_open(void)
{
    BT_DBG("");

    ao_conf_t  ao_cnf;

    ao_conf_init(&ao_cnf);
    ao_cnf.vol_en    = 1;
    /** set initial volume */
    ao_cnf.vol_index = 127;
    ao_cnf.resample_rate = CONFIG_RESAMPLE_RATE;
    ao = ao_open(sf, &ao_cnf);

    return 0;
}

static int audio_output_start(void)
{
    BT_DBG("");

    ao_start(ao);

    return 0;
}

static int audio_output_get_write_frame_buf(void **buf, u32_t *size)
{
    static uint16_t s_buf[256];

    *buf  = s_buf;
    *size = sizeof(s_buf);

    return 0;
}

static int audio_output_write_done(void *buf, u32_t size)
{
    ao_write(ao, buf, size);

    return 0;
}

static int audio_output_stop(void)
{
    BT_DBG("");

    ao_stop(ao);

    return 0;
}

static int audio_output_close(void)
{
    BT_DBG("");

    ao_close(ao);

    return 0;
}

static int audio_output_vol(u8_t vol)
{
    BT_DBG("");

    size_t size;
    ovol_set_t para;

    size = sizeof(para);
    /** convert vol to ao range: 0 ~ 127 -> 0 ~ 255 */
    para.vol_index = vol * 255 / BT_AVRCP_MAX_VOLUME;

    ao_control(ao, AO_CMD_VOL_SET, (void*)&para, &size);

    return 0;
}

static const bt_a2dp_audio_output_t audio_output = {
    .init          = audio_output_init,
    .open          = audio_output_open,
    .start         = audio_output_start,
    .conf          = audio_output_conf,
    .get_write_frame_buf = audio_output_get_write_frame_buf,
    .stop          = audio_output_stop,
    .close         = audio_output_close,
    .write_done    = audio_output_write_done,
    .vol           = audio_output_vol,
};

const bt_a2dp_audio_output_t *bt_a2dp_audio_output_get_interface(void)
{
    return &audio_output;
}
#endif /* CONFIG_AV_AO_ALSA */

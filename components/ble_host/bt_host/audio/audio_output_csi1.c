/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <bluetooth/audio_output.h>

#if defined (CONFIG_CSI_V1) && !defined(CONFIG_AV_AO_ALSA)
#include <drv/codec.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_A2DP)
#define LOG_MODULE_NAME bt_a2dp
#include "common/log.h"

#define PCM_EVT_WRITE    (1 << 0)
#define PCM_EVT_UNDERRUN (1 << 2)

static codec_output_t codec;
static aos_event_t    evt;

static void codec_event_cb(int idx, codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        aos_event_set(&evt, PCM_EVT_WRITE, AOS_EVENT_OR);
    } else {
        aos_event_set(&evt, PCM_EVT_UNDERRUN, AOS_EVENT_OR);
    }
}

/** left_gain/right_gain [-31, 0] 1dB step */
static int snd_set_gain(int l, int r)
{
    codec_output_t *p = &codec;

    if (l == -31 || r == -31) {
        csi_codec_output_mute(p, 1);
    } else {
        csi_codec_output_mute(p, 0);

        csi_codec_output_set_analog_left_gain(p, l);
        csi_codec_output_set_analog_right_gain(p, l);
    }

    return 0;
}

static int audio_output_init(void)
{
    BT_DBG("");
    csi_codec_init(0);
    aos_event_new(&evt, 0);
    return 0;
}

static int audio_output_conf(audio_output_conf_t *conf)
{
    BT_DBG("");
    codec.buf       = conf->buf;
    codec.buf_size  = conf->frame_len * conf->frame_num;
    codec.cb        = codec_event_cb;
    codec.cb_arg    = NULL;
    codec.ch_idx    = 0;
    codec.codec_idx = 0;
    codec.period    = conf->frame_len;

    int ret = csi_codec_output_open(&codec);

    if (ret != 0) {
        BT_ERR("ret : %d", ret);
        return -1;
    }

    codec_output_config_t config;
    config.bit_width    = 16;
    config.mono_mode_en = conf->channel == 1 ? 1 : 0;
    config.sample_rate  = conf->sample_rate;

    ret = csi_codec_output_config(&codec, &config);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static int audio_output_open(void)
{
    BT_DBG("");

    return 0;
}

static int audio_output_start(void)
{
    BT_DBG("");

    int ret = csi_codec_output_start(&codec);

    if (ret != 0) {
        BT_ERR("ret : %d", ret);
        return -1;
    }

    snd_set_gain(-11, -11);

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
    BT_DBG("");

    unsigned int actl_flags = 0;
    int          ret        = -1;
    char *       send       = (char *)buf;

    while (size) {
        ret = csi_codec_output_write(&codec, buf, size);
        if (ret < size) {
            aos_event_get(&evt, PCM_EVT_WRITE | PCM_EVT_UNDERRUN, AOS_EVENT_OR_CLEAR,
                          &actl_flags, AOS_WAIT_FOREVER);
        }

        size -= ret;
        send += ret;
    }

    return 0;
}

static int audio_output_stop(void)
{
    BT_DBG("");

    int ret = csi_codec_output_stop(&codec);

    if (ret != 0) {
        BT_ERR("ret : %d", ret);
        return -1;
    }

    return 0;
}

static int audio_output_close(void)
{
    BT_DBG("");

    int ret;

    csi_codec_output_stop(&codec);
    ret = csi_codec_output_close(&codec);

    if (ret != 0) {
        BT_ERR("ret : %d", ret);
        return -1;
    }

    return 0;
}

static int audio_output_vol(u8_t vol)
{
    BT_DBG("");

    int gain = vol * 31 / 100 - 31;

    snd_set_gain(gain, gain);

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
#endif /* CONFIG_CSI_V1 */

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <drv/i2s.h>
#include <drv/gpio.h>
#include <sound.h>
#include <pin_name.h>
#include <pinmux.h>
#include "tas5805.h"

#define TAG "tas5805"

#define mixer_uninit device_free

#define card_priv(dev) ((tas5805_snd_t *)(((card_dev_t *)dev)->priv))
#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)

#define DAC_WRITE_EVENT  (0x01)
#define ADC_READ_EVENT   (0x02)

#define reg_wr(reg_addr) *((volatile int *)reg_addr)

#define TAS5805_SEND_BUFFER (1024 * 100)

typedef struct _tas5805_snd {
    i2s_handle_t i2s_hdl;
    aos_dev_t       *i2c_dev;
} tas5805_snd_t;

aos_dev_t *tas5805_i2c_init(int id);
void vol_set(aos_dev_t *i2c_dev, int vol);

static volatile int i2s_event_flag = 0;

static void i2s_event_cb_fun(int32_t idx, i2s_event_e event)
{
    if (event == I2S_EVENT_RECEIVE_COMPLETE || event == I2S_EVENT_SEND_COMPLETE) {
        i2s_event_flag = 1;
    }
}

static int pcm_p_open(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    i2s_handle_t i2s_handle = pcm->hdl;

    ck_i2s_enable(i2s_handle);
    return 0;
}

static int pcm_close(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    i2s_handle_t i2s_handle = pcm->hdl;

    ck_i2s_disable(i2s_handle);
    return 0;
}

static int _pcm_send(aos_pcm_t *pcm, void *data, int len)
{
    i2s_handle_t i2s_handle = pcm->hdl;
    int send_size = len;
    int offset = 0;
    int cnt;
    // LOGE(TAG, "pcm(%p) send:%p, %d", i2s_handle, data, len);
    while (send_size) {
        if (send_size < TAS5805_SEND_BUFFER) {
            cnt = send_size;
        } else {
            cnt = TAS5805_SEND_BUFFER;
        }

        i2s_event_flag = 0;
        csi_i2s_send(i2s_handle, (char *)data + offset, cnt);

        while (!i2s_event_flag);

        offset += cnt;
        send_size -= cnt;
    }

	return len;
}

static int _snd_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{
    int ret = -1;
    i2s_handle_t i2s_handle = pcm->hdl;
    int rate = params->rate;
    int sample_bits = params->sample_bits;

    if (rate != 48000 || sample_bits != 16 || sample_bits != 24) {
        return -1;
    }

    ret = csi_i2s_config(i2s_handle, I2S_PROTOCOL_I2S, params->sample_bits, I2S_MODE_TX_MASTER, params->rate);

    if (sample_bits == 16) {
        reg_wr(0x90004c98) = 15;  // 16bit div2
    } else {
        reg_wr(0x90004c98) = 23;  // 24bit div2
    }

    return (ret == 0) ? ret : -1;
}

static int _snd_get_buffer_size(aos_pcm_t *pcm)
{
    int ret = 0;
    return ret;
}

static int _snd_get_remain_size(aos_pcm_t *pcm)
{
    int ret = 0;
    return ret;
}

/* left_gain/right_gain [-31, 0] 1dB step*/
static int _snd_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
    aos_dev_t *i2c_dev = elem->hdl;

    vol_set(i2c_dev, (l+r) / 2);
    return 0;
}

static int _snd_volume_to_dB(aos_mixer_elem_t *elem, int val)
{
    int gain;
    int gain_s = TAS5805M_VOLUME_MIN, gain_e = TAS5805M_VOLUME_MAX;

    //FIXME
    gain = gain_s + (val / 100.0 * (gain_e - gain_s));
    return gain;
}

static int _snd_pause(aos_pcm_t *pcm, int enable)
{


    return 0;
}

static sm_elem_ops_t elem_codec1_ops = {
    .set_dB       = _snd_set_gain,
    .volume_to_dB = _snd_volume_to_dB,
};

static aos_dev_t *pcm_init(driver_t *drv, void *config, int id)
{
    aos_pcm_dev_t *pcm_dev = (aos_pcm_dev_t *)device_new(drv, sizeof(aos_pcm_dev_t), id);
    aos_pcm_drv_t *pcm_drv = (aos_pcm_drv_t *)drv;

    memset(&pcm_dev->pcm, 0x00, sizeof(aos_pcm_t));
    pcm_dev->pcm.ops = &(pcm_drv->ops);

    pcm_dev->pcm.hdl = config;

    return (aos_dev_t *)(pcm_dev);
}

static void pcm_deinit(aos_dev_t *dev)
{
    if (dev->config) {
        aos_free(dev->config);
    }

    device_free(dev);
}
static aos_pcm_drv_t aos_pcm_drv[] = {
    {
        .drv = {
            .name               = "pcmP",
            .init               = pcm_init,
            .uninit             = pcm_deinit,
            .open               = pcm_p_open,
            .close              = pcm_close,
        },
        .ops = {
            .hw_params_set      = _snd_param_set,
            .write              = _pcm_send,
            .hw_get_size        = _snd_get_buffer_size,
            .hw_get_remain_size = _snd_get_remain_size,
            .pause              = _snd_pause,
        },
    }
};

static int aos_pcm_register(i2s_handle_t hdl)
{
    driver_register(&aos_pcm_drv[0].drv, (void *)hdl, 0);

    return 0;
}

static void pdn_pin(uint32_t id)
{
    gpio_pin_handle_t pin = NULL;

    drv_pinmux_config(id, PIN_FUNC_GPIO);

    pin = csi_gpio_pin_initialize(id, NULL);

    csi_gpio_pin_config_mode(pin, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_OUTPUT);

    csi_gpio_pin_write(pin, 0);
    aos_msleep(1000);
    csi_gpio_pin_write(pin, 1);
    aos_msleep(1000);
}

static int card_open(aos_dev_t *dev)
{
    aos_mixer_elem_t *elem;
    card_dev_t *card = (card_dev_t *)dev;
    sound_config_t *config = (sound_config_t *)dev->config;
    tas5805_snd_t *p = card->priv;
    snd_card_drv_t *card_drv = (snd_card_drv_t *)(card->device.drv);
    int i2s_id = config->i2s_id;
    i2s_handle_t i2s_handle;
    aos_dev_t *i2c_dev;

    pdn_pin(config->pin);
    i2s_handle = csi_i2s_initialize(i2s_id, i2s_event_cb_fun);

    csi_i2s_power_control(i2s_handle, DRV_POWER_FULL);
    aos_msleep(2000);

    int ret = csi_i2s_config(i2s_handle, I2S_PROTOCOL_I2S, 16, I2S_MODE_TX_MASTER, 48000);

    if (ret < 0) {
        LOGE(TAG, "csi_i2s_config error ret%x \n", ret);
        goto err;
    }

    csi_i2s_config_frame_width(i2s_handle, 32);

    ck_i2s_enable(i2s_handle);
    i2c_dev = tas5805_i2c_init(config->i2c_id);
    if (i2c_dev == NULL) {
        LOGE(TAG, "i2c open error\n");
        goto err;
    }

    ck_i2s_disable(i2s_handle);

    ret = csi_i2s_config(i2s_handle, I2S_PROTOCOL_I2S, 16, I2S_MODE_TX_MASTER, 48000);

    if (ret < 0) {
        LOGE(TAG, "csi_i2s_config error ret%x \n", ret);
        goto err;
    }

    p->i2c_dev = i2c_dev;
    p->i2s_hdl = i2s_handle;

    aos_pcm_register(i2s_handle);

    slist_init(&card_drv->mixer_head);
    snd_elem_new(&elem, "codec1", &elem_codec1_ops);
    slist_add(&elem->next, &card_drv->mixer_head);
    elem->hdl = i2c_dev;

    LOGE(TAG, "tas5805 open suc(%p)\r\n", i2s_handle);
    return 0;

err:
    ck_i2s_disable(i2s_handle);
    csi_i2s_power_control(i2s_handle, DRV_POWER_OFF);
    csi_i2s_uninitialize(i2s_handle);

    return -1;
}

static int card_close(aos_dev_t *dev)
{
    //TODO
    tas5805_snd_t *p = card_priv(dev);
    i2s_handle_t i2s_handle = p->i2s_hdl;

    csi_i2s_power_control(i2s_handle, DRV_POWER_OFF);
    csi_i2s_uninitialize(i2s_handle);
    return 0;
}

static aos_dev_t *card_init(driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)device_new(drv, sizeof(card_dev_t), id);

    card->device.config = (sound_config_t *)aos_malloc_check(sizeof(sound_config_t));
    card->priv = (tas5805_snd_t *)aos_malloc_check(sizeof(tas5805_snd_t));
    memcpy(card->device.config, config, sizeof(sound_config_t));

    return (aos_dev_t *)card;
}

static void card_deinit(aos_dev_t *dev)
{
    card_dev_t *card = (card_dev_t *)dev;

    if (card->device.config) {
        aos_free(card->device.config);
    }

    if (card->priv) {
        aos_free(card->priv);
    }

    device_free(dev);
}
static snd_card_drv_t snd_card_drv = {
    .drv = {
        .name   = "card",
        .init   = card_init,
        .uninit = card_deinit,
        .open   = card_open,
        .close  = card_close,
    }
};

void tas5805_snd_card_register(void *config)
{
    driver_register(&snd_card_drv.drv, config, 0);
}

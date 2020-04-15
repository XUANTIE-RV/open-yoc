/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <math.h>
#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>

#include <silan_syscfg.h>
#include <silan_adev.h>
#include <silan_iomux.h>

#define TAG "sc5654"

#define card_uninit device_free
#define pcm_uninit device_free
#define mixer_uninit device_free

#define pcm_dev(dev) &(((aos_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((aos_pcm_drv_t *)((((aos_pcm_dev_t *)dev)->device.drv)))->ops)


/** 数组的计算
 * 由于功放的倍数不同，音量调整的范围需要调整
 * 在 CODEC_DAC_GAIN_MIN(-7200) ~ CODEC_DAC_GAIN_MAX(2325)  范围内确定新的最大值
 * 例如  -7200 ~ -1900
 * 该公式能反应比较好的调整体验sqrt(i) * (7200-1900) / 10 - CODEC_DAC_GAIN_MAX
 * 在音量低的时候变化快些
*/
static int c_vol_tab[128] = {0,};

static ADEV_I2S_HANDLE *silan_hdl = NULL;
static int g_fix_l_gian = -1;
static int g_fix_r_gian = -1;
static int g_valid_vol_percent = 100;

static void silan_audio_event_cb_fun(int event_id, void *priv)
{
    aos_pcm_t *pcm = (aos_pcm_t *)priv;
    if(pcm->event.cb) {
        pcm->event.cb(pcm, event_id, pcm->event.priv);
    }
}

static ADEV_I2S_HANDLE *silan_init(void)
{
    if (silan_hdl == NULL) {
        ADEV_I2S_HANDLE *o_dev = silan_adev_i2s_alloc(I2S_ID_O1);

        LOGI(TAG, "i2s o1 pdev=%p", o_dev);
        o_dev->i2s_cfg.master = CLK_MASTER;
        o_dev->adev_cfg.width = 16;
        o_dev->i2s_cfg.ch     = I2S_CH_20;
        o_dev->i2s_cfg.ws     = I2S_WS_32;
        o_dev->i2s_cfg.prot   = I2S_PROT_I2S;
        o_dev->i2s_cfg.codec  = CODEC_ID_1;

        if (silan_adev_i2s_open(o_dev) < 0) {
            return NULL;
        }

        silan_hdl = o_dev;
    }

    return silan_hdl;
}

static void silan_deinit(ADEV_I2S_HANDLE *hdl)
{
    if (hdl == silan_hdl) {
        silan_hdl = NULL;
    }
}

static int pcm_p_open(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    //LOGE(TAG, "pcm hdl:%p", pcm);
    // pcm->ops = pcm_ops(dev);

    pcm->hdl = silan_init();
    silan_dma_cb_reg(silan_audio_event_cb_fun, (void *)pcm);
    pcm->state = AOS_PCM_STATE_SUSPENDED;

    return 0;
}

static int pcm_c_open(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);
    // pcm->ops = pcm_ops(dev);

    pcm->hdl = silan_adev_i2s_alloc(I2S_ID_I1);
    ADEV_I2S_HANDLE *i_dev = pcm->hdl;
    LOGI(TAG, "i2s i1 pdev=%p", i_dev);

    i_dev->adev_cfg.width = 16;
    i_dev->i2s_cfg.tr = I2S_TR_RO;

    i_dev->i2s_cfg.ch     = I2S_CH_20;
    i_dev->i2s_cfg.ws     = I2S_WS_32;
    i_dev->i2s_cfg.prot   = I2S_PROT_I2S;
    i_dev->i2s_cfg.codec  = CODEC_ID_EXT;
    i_dev->i2s_cfg.master = CLK_SLAVE;

    // i_dev->i2s_cfg.codec  = CODEC_ID_1;
    i_dev->codec_cfg.adc_sel = CODEC_ADSEL_AUX;
    i_dev->codec_cfg.adc_diff = CODEC_SINGLE;

    if (silan_adev_i2s_open(i_dev) < 0) {
        return -1;
    }

    silan_adev_i2s_set_rate(i_dev, 16000);
    silan_adev_i2s_start(i_dev);
    silan_adev_i2s_set_gain(i_dev, 0, 0);

    return 0;
}

static int pcm_close(aos_dev_t *dev)
{
    aos_pcm_t *pcm = pcm_dev(dev);

    //LOGD(TAG, "pcm close:%p\n", pcm->hdl);
    silan_adev_i2s_stop(pcm->hdl);
    silan_adev_i2s_revoke(pcm->hdl);
    silan_deinit(pcm->hdl);

    return 0;
}

static int card_open(aos_dev_t *dev)
{
    //TODO
    return 0;
}

static int card_close(aos_dev_t *dev)
{
    // aos_pcm_t *pcm;
    // snd_card_drv_t *drv = (snd_card_drv_t *)(dev->drv);

    //TODO mixer
    return 0;
}

static int _pcm_recv(aos_pcm_t *pcm, void *data, int len)
{
    if (len % pcm->hw_params->buffer_size != 0) {
        return -1;
    }

    ADEV_I2S_HANDLE *i_dev = pcm->hdl;
    int r_size = pcm->hw_params->buffer_size;
    int r_max = len / pcm->hw_params->buffer_size;
    int offset = 0;
    int ret;
    signed short  *in1;

    for (int i = 0; i < r_max; i++) {
        ret = silan_adev_pre_read((void *)i_dev, (char **)&in1, r_size);

        if (ret != r_size) {
            return offset;
        }

        dcache_invalidate(in1, r_size);

        silan_adev_read((void *)i_dev, r_size);
        memcpy((char *)data + offset, (char *)in1, ret);
        offset += ret;
    }

    return offset;
}

static int _pcm_send(aos_pcm_t *pcm, void *data, int len)
{
    ADEV_I2S_HANDLE *o_dev = pcm->hdl;
    int ret = -1;
    int w_size;
    int min_size = pcm->hw_params->buffer_size;
    int offset = 0;
    signed short  *out1;
    int send_len;

    //FIXME: for silan driver question
    if (o_dev->remain_size) {
        int msize = min_size - o_dev->remain_size;
        if (msize <= len) {
            /* remain_size >= 1ms pcm data, we can write it */
            offset += msize;
            memcpy(o_dev->remain_buf + o_dev->remain_size, (char*)data, msize);
            o_dev->remain_size += msize;
            for (;;) {
                /* write the remain size */
                w_size = min_size;
                ret    = silan_adev_pre_write((void *)o_dev, (char **)&out1, w_size);
                if (ret != w_size) {
                    aos_msleep(150);
                    continue;
                }

                memcpy((char *)out1, (char *)o_dev->remain_buf, w_size);
                dcache_writeback(out1, w_size);
                ret = silan_adev_write((void *)o_dev, w_size);
                if (ret != w_size) {
                    LOGE(TAG, "adev write fail, ret =%d\n", ret);
                    return -1;
                }
                o_dev->remain_size = 0;
                break;
            }
        } else {
            memcpy(o_dev->remain_buf + o_dev->remain_size, (char*)data, len);
            o_dev->remain_size += len;
            return len;
        }
    }

    if (len <= 0)
        return offset;

    send_len = min_size;
    for (;;) {
        w_size   = send_len < (len - offset) ? send_len : (len - offset);
        if (w_size < min_size) {
            memcpy(o_dev->remain_buf, (char*)data + offset, w_size);
            offset             += w_size;
            o_dev->remain_size  = w_size;
            break;
        }

        ret = silan_adev_pre_write((void *)o_dev, (char **)&out1, w_size);
        if (ret != w_size) {
            aos_msleep(150);
            continue;
        }

        memcpy((char *)out1, (char *)data + offset, w_size);
        dcache_writeback(out1, w_size);
        ret = silan_adev_write((void *)o_dev, w_size);
        if (ret != w_size) {
            LOGE(TAG, "adev write fail, ret =%d\n", ret);
            return -1;
        }

        offset += ret;
        if (offset >= len) {
            break;
        }
    }

    return offset;
}

static int silan_param_set(aos_pcm_t *pcm, aos_pcm_hw_params_t *params)
{

    ADEV_I2S_HANDLE *adev = pcm->hdl;
    uint8_t tr = adev->i2s_cfg.tr;

    if(tr & I2S_TR_TO) {
        adev->adev_cfg.width = params->format;
        silan_adev_i2s_set_rate(adev, params->rate);
        pcm->hw_params->buffer_size = silan_adev_i2s_submit(adev);
    } else {
        silan_adev_i2s_stop(adev);
        silan_adev_i2s_revoke(adev);
        adev->adev_cfg.width = params->format;
        silan_adev_i2s_set_rate(adev, params->rate);
        pcm->hw_params->buffer_size = silan_adev_i2s_submit(adev);
        silan_adev_i2s_start(adev);
    }

    return 0;
}

static int silan_get_buffer_size(aos_pcm_t *pcm)
{
    int ret = 0;

    if(pcm->hw_params != NULL && pcm->hw_params->buffer_size > 0) {
        ret = silan_adev_get_buffer_size(pcm->hdl);
    }
    return ret;
}

static int silan_get_remain_size(aos_pcm_t *pcm)
{
    int ret = 0;

    if(pcm->hw_params != NULL && pcm->hw_params->buffer_size > 0) {
        ret = silan_adev_get_read_size(pcm->hdl);
    }
    return ret;
}

/*
* silan_codec_dac_set_gain 的最大值
* API最大限制是127，实际100左右就有较大失真，设置最大为100
* 最后实际的最大值可由 g_valid_vol_percent 再次控制,由snd_card_register传入
* 
*/
#define API_MAX_GAIN_VAL 100
#define API_DB_SCALE 5000
static int silan_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
    silan_init(); //防止codec未初始化时调节音量失败

    if (silan_codec_check(CODEC1)) {
        int user_max_api_val = API_MAX_GAIN_VAL * g_valid_vol_percent / 100;
        int codec_l = l * user_max_api_val / (API_DB_SCALE * 2) + user_max_api_val;
        int codec_r = r * user_max_api_val / (API_DB_SCALE * 2) + user_max_api_val;

        if (codec_l < 0) {
            codec_l = 0;
        }

        if (codec_r < 0) {
            codec_r = 0;
        }

        //LOGI(TAG, "dbl:%d  dbr:%d\n", codec_l, codec_r);
        if (g_fix_l_gian > 0) {
            /* 左声道固定，作为参考音 */
            codec_l = API_MAX_GAIN_VAL * g_fix_l_gian / 100;
        }
        if (g_fix_r_gian > 0) {
            /* 右声道固定，作为参考音 */
            codec_r = API_MAX_GAIN_VAL * g_fix_r_gian / 100;
        }
        silan_codec_dac_set_gain(CODEC1, codec_l, codec_r);

        static int is_mute_last = 0;
        if (codec_l <= 0 || codec_r <= 0) {
            silan_codec_dac_mute(CODEC1);
            is_mute_last = 1;
        } else {
            silan_codec_dac_unmute(CODEC1);
            if (is_mute_last) {
                aos_msleep(350);
                is_mute_last = 0;
            }
        }
    }

    return 0;
}

static int silan_volume_to_dB(aos_mixer_elem_t *elem, int val)
{
    int dB = c_vol_tab[val];
    // LOGD(TAG, "val:%d db:%d\n", val, dB);
    return dB;
}

static int silan_pause(aos_pcm_t *pcm, int enable)
{
    ADEV_I2S_HANDLE *adev = pcm->hdl;

    if(enable) {
        silan_adev_i2s_stop(adev);
        //LOGE(TAG, "slian stop(%p)", pcm);
    } else {
        silan_adev_i2s_start(adev);
        //LOGE(TAG, "slian start(%p)", pcm);
    }

    return 0;
}

static int silan_event_set(aos_pcm_t *pcm,  pcm_event_cb cb, void *priv)
{
    pcm->event.cb = cb;
    pcm->event.priv = priv;

    silan_dma_cb_reg(silan_audio_event_cb_fun, (void *)pcm);
    return 0;
}

static sm_elem_ops_t elem_codec1_ops = {
    .set_dB = silan_set_gain,
    .volume_to_dB = silan_volume_to_dB,
};

static aos_dev_t *pcm_init(driver_t *drv, void *config, int id)
{
    aos_pcm_dev_t *pcm_dev = (aos_pcm_dev_t *)device_new(drv, sizeof(aos_pcm_dev_t), id);

    if (pcm_dev) {
        aos_pcm_drv_t *pcm_drv = (aos_pcm_drv_t *)drv;
        memset(&pcm_dev->pcm, 0x00, sizeof(aos_pcm_t));
        pcm_dev->pcm.ops = &(pcm_drv->ops);        
    }

    return (aos_dev_t *)(pcm_dev);
}

static aos_pcm_drv_t aos_pcm_drv[] = {
    {
        .drv = {
            .name = "pcmP",
            .init = pcm_init,
            .uninit = pcm_uninit,
            .open = pcm_p_open,
            .close = pcm_close,
        },
        .ops = {
            .hw_params_set = silan_param_set,
            .write = _pcm_send,
            .hw_get_size = silan_get_buffer_size,
            .hw_get_remain_size = silan_get_remain_size,
            .pause = silan_pause,
        },
    },
    {
        .drv = {
            .name = "pcmC",
            .init = pcm_init,
            .uninit = pcm_uninit,
            .open = pcm_c_open,
            .close = pcm_close,
        },
        .ops = {
            .hw_params_set = silan_param_set,
            .read = _pcm_recv,
            .set_event = silan_event_set,
        },
    }
};

static int aos_pcm_register(void)
{
    driver_register(&aos_pcm_drv[0].drv, NULL, 0);
    driver_register(&aos_pcm_drv[1].drv, NULL, 0);

    return 0;
}

static aos_dev_t *card_init(driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)device_new(drv, sizeof(card_dev_t), id);
    snd_card_drv_t *card_drv = (snd_card_drv_t *)drv;
    aos_mixer_elem_t *elem;

    silan_io_func_config(IO_CONFIG_PB5, IO_FUNC_I2S_20);
    silan_io_func_config(IO_CONFIG_PB6, IO_FUNC_I2S_20);
    silan_io_func_config(IO_CONFIG_PB7, IO_FUNC_I2S_20);

    silan_codec1_cclk_config(CLK_ON);
    silan_codec2_cclk_config(CLK_ON);

    silan_dmac_req_matrix_init();
    silan_dmac_init();
    silan_adev_init();

    aos_pcm_register();

    slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec1", &elem_codec1_ops);
    slist_add(&elem->next, &card_drv->mixer_head);

    return (aos_dev_t *)card;
}

static snd_card_drv_t snd_card_drv = {
    .drv = {
        .name = "card",
        .init = card_init,
        .uninit = card_uninit,
        .open   = card_open,
        .close  = card_close,
    }
};

static void snd_car_update_vol_tab()
{
    c_vol_tab[0] = -2 * 5000;

    /* -10000 ~ 0 */
    for (int i = 1; i <= 100; i++) {
        c_vol_tab[i] = log10(i / 100.0) * API_DB_SCALE;
    }

    //数组下标位音量百分比，
    // 0:-10000 静音
    c_vol_tab[1] = c_vol_tab[2];

#if 0
    /* Debug */
    for (int i = 0; i <= 100; i++) {
        int user_max_api_val = API_MAX_GAIN_VAL * g_valid_vol_percent / 100;
        int api_gain = c_vol_tab[i] * user_max_api_val / (API_DB_SCALE * 2) + user_max_api_val;
        printf("%d\n", api_gain);
    }
#endif
}

void sound_drv_sc5654_register(int vol_range, int fix_l_gain, int fix_r_gain)
{
    if (c_vol_tab[0] == 0) {
        g_fix_l_gian = fix_l_gain;
        g_fix_r_gian = fix_r_gain;
        g_valid_vol_percent = vol_range;

        snd_car_update_vol_tab();
        driver_register(&snd_card_drv.drv, NULL, 0);
    } else {
        g_valid_vol_percent = 100; /* debug mode */
    }
}

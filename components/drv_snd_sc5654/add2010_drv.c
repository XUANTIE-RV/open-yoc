/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     add2010_drv.c
 * @brief    source file for add2010 driver
 * @version  V1.0
 * @date     31. Oct. 2019
 ******************************************************************************/
#include <math.h>
#include <aos/aos.h>
#include <aos/log.h>
#include <aos/hal/i2c.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>

#include <silan_syscfg.h>
#include <silan_adev.h>
#include <silan_iomux.h>
#include <pinmux.h>

#include "add2010_config.h"
#include "add2010_drv.h"


#define TAG "AD2010"

#define ADD2010_ADDR 0x34

#define REG_LEFT_VOL    0x04
#define REG_RIGHT_VOL   0x05

#define MAX_GAIN_10DB    (-100)
#define MIN_GAIN_10DB    (-500)

#define ADD2010_I2C_TIMEOUT     200

static int last_l_vol = 0xff;
static int last_r_vol = 0xff;

static i2c_dev_t *g_i2c_dev;

static int add2010_i2c_write_reg(i2c_dev_t *dev, uint8_t addr, uint8_t val)
{
    uint8_t reg_val[2] = {addr, val};

    return hal_i2c_master_send(dev, ADD2010_ADDR, reg_val, 2, ADD2010_I2C_TIMEOUT);
}

static int add2010_config(i2c_dev_t *i2c_dev, int left_vol, int right_vol)
{
    int ret;

    /* add2010 reset */
    ret = add2010_i2c_write_reg(i2c_dev, 0x1A, 0x50);
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    aos_msleep(5);

    ret = add2010_i2c_write_reg(i2c_dev, 0x1A, 0x70);
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);
    aos_msleep(20);

    g_add2010_reg_tab[REG_LEFT_VOL].data = left_vol;
    g_add2010_reg_tab[REG_RIGHT_VOL].data = right_vol;

    /* mute AP */
    // ret = add2010_i2c_write_reg(i2c_dev, 0x02, 0x7f);
    // CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    /* init the reg table */
    ret = add2010_write_reg_table(i2c_dev, &g_add2010_reg_tab[0], g_add2010_reg_tab_size);
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    /* mute AP again to avoid override in reg table */
    // ret = add2010_i2c_write_reg(i2c_dev, 0x02, 0x7f);
    // CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    /* init the ram table */
    ret = add2010_write_ram_table(i2c_dev, 1, &g_add2010_ram1_tab[0], g_add2010_ram1_tab_size);
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    ret = add2010_write_ram_table(i2c_dev, 2, &g_add2010_ram2_tab[0], g_add2010_ram2_tab_size);
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    /* unmute AP */
    // ret = add2010_i2c_write_reg(i2c_dev, 0x02, 0x00);
    // CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    return 0;

err:
    LOGE(TAG, "add2010 init failed");

    return -1;
}

int add2010_init(i2c_dev_t *i2c_dev)
{
    LOGD(TAG, "start add2010 config");

    g_i2c_dev = i2c_dev;
    add2010_config(i2c_dev, last_l_vol, last_r_vol);

    LOGD(TAG, "end add2010 config");
    return 0;
}

int add2010_shutdown(i2c_dev_t *i2c_dev)
{
    int ret;

    ret = add2010_i2c_write_reg(i2c_dev, 0x1A, 0x00);
    CHECK_RET_TAG_WITH_RET(ret == 0, -1);

    return -1;
}

/**
 * set eq params for 1~12 EQ filters
 */
int add2010_set_eq_param(i2c_dev_t *i2c_dev, uint8_t params[180])
{
    printf("eq params:\n");
    for (int i = 0; i < 60; ++i) {
        g_add2010_ram1_tab[i].first_byte = params[i * 3];
        g_add2010_ram1_tab[i].second_byte = params[i * 3 + 1];
        g_add2010_ram1_tab[i].third_byte = params[i * 3 + 2];
        printf("Coef[%xh]=%x%x%xh\n", i, params[i * 3], params[i * 3 + 1], params[i * 3 + 2]);
    }

    int ret = add2010_init(i2c_dev);
    CHECK_RET_TAG_WITH_RET(ret == 0, -1);

    return 0;
}

int add2010_set_vol(i2c_dev_t *i2c_dev, int l_gain, int r_gain)
{
    int l_val, r_val;
    int ret;

    l_gain = l_gain < MIN_GAIN_10DB ? MIN_GAIN_10DB: (l_gain > MAX_GAIN_10DB ? MAX_GAIN_10DB : l_gain);
    r_gain = r_gain < MIN_GAIN_10DB ? MIN_GAIN_10DB: (r_gain > MAX_GAIN_10DB ? MAX_GAIN_10DB : r_gain);

    if (l_gain == MIN_GAIN_10DB) {
        l_val = 0xff;
    } else {
        l_val = 24 - l_gain / 5;
    }

    if (r_gain == MIN_GAIN_10DB) {
        r_val = 0xff;
    } else {
        r_val = 24 - r_gain / 5;
    }

    if (last_l_vol != l_val) {
        ret = add2010_i2c_write_reg(i2c_dev, REG_LEFT_VOL, l_val);
        if (ret != 0) {
            LOGW(TAG, "add2010 write error");
            return -1;
        }

        last_l_vol  = l_val;
    }

    if (last_r_vol != r_val) {
        ret = add2010_i2c_write_reg(i2c_dev, REG_RIGHT_VOL, r_val);
        if (ret != 0) {
            LOGW(TAG, "add2010 write error");
            return -1;
        }
        
        last_r_vol = r_val;
    }

    return 0;
}

int add2010_write_reg_table(i2c_dev_t *i2c_dev, add2010_reg_table_t *reg_address, int reg_num)
{
    add2010_reg_table_t *preg = reg_address;

    for (int i = 0; i < reg_num; i++) {
        if (add2010_i2c_write_reg(i2c_dev, preg->addr, preg->data) != 0) {
            return -1;
        }

        preg++;
    }

    return 0;
}

int add2010_write_ram_table(i2c_dev_t *i2c_dev, int channel, add2010_ram_table_t *ram_address, int data_size)
{
    add2010_ram_table_t *pram = ram_address;

    for (int i = 0; i < data_size; i++) {
        // do {
        //    	aos_msleep(10);
        // } while((add2010_read_reg(hdl, 0x2D)&0x01));

        uint8_t *pdata = &pram->addr;

        for (int j = 0; j < 4; j++) {
            if (add2010_i2c_write_reg(i2c_dev, 0x1D + j, *pdata++) != 0) {
                goto err;
            }
        }

        if (add2010_i2c_write_reg(i2c_dev, 0x2D, channel == 1 ? 0x01 : 0x41) != 0) {
            goto err;
        }

        pram++;
    }

    return 0;

err:
    LOGE(TAG, "write ram failed");

    return -1;
}

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
        ADEV_I2S_HANDLE *o_dev = silan_adev_i2s_alloc(I2S_ID_PCM);

        LOGI(TAG, "i2s o1 pdev=%p", o_dev);
        o_dev->i2s_cfg.master = CLK_MASTER;
        o_dev->adev_cfg.width = 16;
        o_dev->i2s_cfg.ch     = I2S_CH_20;
        o_dev->i2s_cfg.ws     = I2S_WS_32;
        o_dev->i2s_cfg.prot   = I2S_PROT_I2S;
        o_dev->i2s_cfg.codec  = CODEC_ID_EXT;
        o_dev->i2s_cfg.tr     = I2S_TR_TO;

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

static int pa_add2010_set_gain(aos_mixer_elem_t *elem, int l, int r)
{
    //LOGI(TAG, "dbl:%d  dbr:%d\n", codec_l, codec_r);
    if (g_fix_l_gian > 0) {
        /* 左声道固定，作为参考音 */
        l = g_fix_l_gian;
    }
    if (g_fix_r_gian > 0) {
        /* 右声道固定，作为参考音 */
        r = g_fix_r_gian;
    }

    add2010_set_vol(g_i2c_dev, l, r);

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
    .set_dB = pa_add2010_set_gain,
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

#define API_DB_SCALE        300     // unit in 0.1 dB
#define API_DB_OFFSET       (-100)  // unit in 0.1 dB

/**
 *  volume-dB mapping table:
 *  | vol | dB     |
 *  ----------------
 *  | 0   | MUTE   |
 *  | 10  | -40    |
 *  | 20  | -30.97 |
 *  | 30  | -25.69 |
 *  | 40  | -21.94 |
 *  | 50  | -19.03 |
 *  | 60  | -16.66 |
 *  | 70  | -14.65 |
 *  | 80  | -12.91 |
 *  | 90  | -11.38 |
 *  | 100 | -10    |
 * */

static void snd_car_update_vol_tab()
{
    c_vol_tab[0] = -2 * 500;

    /* -10000 ~ 0 */
    for (int i = 1; i <= 100; i++) {
        c_vol_tab[i] = log10(i / 100.0) * API_DB_SCALE + API_DB_OFFSET;
    }
}

void sound_drv_add2010_register(int vol_range, int fix_l_gain, int fix_r_gain)
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


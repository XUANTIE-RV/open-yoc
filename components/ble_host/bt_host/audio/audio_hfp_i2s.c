/**
 * Copyright (c) 2022  Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/bt.h>
#include <aos/ringbuffer.h>

#if defined(CONFIG_BT_HFP_AUDIO_I2S) && (CONFIG_BT_HFP_AUDIO_I2S)
#include <drv/i2s.h>
#include <output/ao.h>
#include <alsa/pcm.h>
#include <aos/classic/bt_stack_hfp_hf.h>
#include <speex/speex_resampler.h>
#include <bluetooth/audio_hfp.h>
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_HFP_AUDIO)
#include "common/log.h"
#include "t_head_audio.h"

#define EVT_RECV_COMPLETED 0x1
#define I2S_BUF_SIZE       (640 * 20)
#define I2S_PERIOD_SIZE    (640 * 2)

#define NUM_POINTS 1024

typedef struct {
    ao_cls_t *ao;
    int       samplerate;
    int       channels;
    int       format;
    sf_t      ao_sf;
    ao_conf_t ao_cnf;
    uint8_t   first_frame_output;
} snd_t;

static uint8_t s_pcm_send_flag = 0;

static i2s_handle_t         i2s_handle;
static uint8_t              i2s_recv_buf[I2S_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t              i2s_send_buf[I2S_BUF_SIZE] __attribute__((aligned(16)));
static aos_sem_t            g_sem_hfp_pcm;
static SpeexResamplerState *s_pcm_srs;
static int                  g_pcm_error;

static void i2s_event_cb_fun(int32_t idx, i2s_event_e event, void *arg)
{
    if (event == I2S_EVENT_RECEIVE_COMPLETE || event == I2S_EVENT_RX_BUFFER_FULL) {
        aos_sem_signal(&g_sem_hfp_pcm);
    }

    if (event == I2S_EVENT_RX_BUFFER_FULL) {
        BT_ERR("%s I2S_EVENT_RX_BUFFER_FULL", __func__);
    }
}

static snd_t *hfp_snd;

static void start_pcm_send(void)
{
    hfp_snd                     = malloc(sizeof(snd_t));
    hfp_snd->samplerate         = 8000;
    hfp_snd->channels           = 2;
    hfp_snd->format             = 16;
    hfp_snd->first_frame_output = 0;

    ao_conf_t ao_cnf;
    ao_cls_t *ao;
    sf_t      sf = sf_make_channel(hfp_snd->channels) | sf_make_rate(hfp_snd->samplerate) | sf_make_bit(hfp_snd->format)
              | sf_make_signed(1);

    ao_conf_init(&ao_cnf);

    ao_cnf.vol_en        = 1;
    ao_cnf.vol_index     = 50;
    ao_cnf.resample_rate = 48000;
    hfp_snd->ao_sf       = sf;
    hfp_snd->ao_cnf      = ao_cnf;

    ao = ao_open(sf, &ao_cnf);
    if (ao == NULL) {
        free(hfp_snd);
        return;
    }
    hfp_snd->ao = ao;

    ao_start(hfp_snd->ao);

    csi_i2s_enable(i2s_handle, 1);
    csi_i2s_receive_ctrl(i2s_handle, I2S_STREAM_START);

    s_pcm_send_flag = 1;
}

static void stop_pcm_send(void)
{
    s_pcm_send_flag = 0;

    csi_i2s_enable(i2s_handle, 0);

    // ringbuffer_clear(&hfp_ringbuff);

    if (hfp_snd != NULL) {
        ao_drain(hfp_snd->ao);
        ao_stop(hfp_snd->ao);
        ao_close(hfp_snd->ao);
        hfp_snd->ao = NULL;
        free(hfp_snd);
    }
}

static void hfp_pcm_recv_entry(void *arg)
{
    uint32_t recv_len;

    uint8_t *pcm_tmp = aos_malloc_check(I2S_PERIOD_SIZE);

    while (1) {
        aos_sem_wait(&g_sem_hfp_pcm, AOS_WAIT_FOREVER);

        recv_len = csi_i2s_receive(i2s_handle, (uint8_t *)pcm_tmp, I2S_PERIOD_SIZE);

        if (hfp_snd->first_frame_output == 0) {
            BT_WARN("enter %s, first frame %d", __func__, recv_len);
            hfp_snd->first_frame_output = -1;
        }

        int ret = ao_write(hfp_snd->ao, pcm_tmp, recv_len);
        if (ret < 0) {
            BT_WARN("APP_PCM_STATE_UNDERRUN");
        }
    }
    aos_free(pcm_tmp);
}

static aos_pcm_t *_pcm_init(unsigned int *rate, uint8_t period_size)
{
    aos_pcm_hw_params_t *params;
    aos_pcm_t *          pcm;
    int                  err, period_frames, buffer_frames;

    aos_pcm_open(&pcm, "pcmC0", AOS_PCM_STREAM_CAPTURE, 0);

    aos_pcm_hw_params_alloca(&params);
    err = aos_pcm_hw_params_any(pcm, params);

    if (err < 0) {
        BT_DBG("Broken configuration for this PCM: no configurations available");
    }

    err = aos_pcm_hw_params_set_access(pcm, params, AOS_PCM_ACCESS_RW_NONINTERLEAVED);

    if (err < 0) {
        BT_DBG("Access type not available");
    }

    err = aos_pcm_hw_params_set_format(pcm, params, 16);

    if (err < 0) {
        BT_DBG("Sample format non available");
    }

    err = aos_pcm_hw_params_set_channels(pcm, params, 2);

    if (err < 0) {
        BT_DBG("Channels count non available");
    }

    aos_pcm_hw_params_set_rate_near(pcm, params, rate, 0);

    period_frames = 160;
    aos_pcm_hw_params_set_period_size_near(pcm, params, &period_frames, 0);

    buffer_frames = period_frames * 16;
    aos_pcm_hw_params_set_buffer_size_near(pcm, params, &buffer_frames);

    err = aos_pcm_hw_params(pcm, params);

    return pcm;
}

static void hfp_mic_alsa_entry(void *priv)
{
    aos_pcm_t *  pcm;
    int          ret;
    unsigned int rate   = 16000;
    int          r_size = (rate / 1000) * (16 / 8 * 2) * 10; // 10ms
    char *       data   = aos_malloc_check(r_size);

    pcm = _pcm_init(&rate, r_size);
    BT_DBG("alsa init succful rate %d", rate);

    int    resample_len = r_size / 2;
    short *resample     = aos_malloc_check(resample_len);

    int    aec_len = r_size / 2;
    short *aec     = aos_malloc_check(resample_len);

    while (1) {
        ret = aos_pcm_wait(pcm, -1);
        if (ret < 0) {
            aos_pcm_recover(pcm, ret, 1);
            continue;
        }

        ret = aos_pcm_readn(pcm, (void *)data, aos_pcm_bytes_to_frames(pcm, r_size));

        if (ret <= 0) {
            continue;
        } else {
            if (s_pcm_send_flag) {
                aec_len = ret * 2;
                /** AEC */
                T_Head_audio_process((short *)data, (short *)(data + ret * 2), aec);

//#define AUDIO_DUMP_DEBUG
#ifdef AUDIO_DUMP_DEBUG
                static short    aec_data[0x300000];
                static uint32_t aec_data_len = 0;
                if (aec_data_len + ret * 3 < sizeof(aec_data) / 2) {
                    for (int i = 0; i < ret; i++) {
                        aec_data[aec_data_len + i * 3]     = *((short *)data + i);
                        aec_data[aec_data_len + i * 3 + 1] = *((short *)(data + ret * 2) + i);
                        aec_data[aec_data_len + i * 3 + 2] = *((short *)(aec) + i);
                    }
                    aec_data_len += ret * 3;
                }
#endif

                /* resample 16K -> 8K */
                speex_resampler_process_int(s_pcm_srs, 0, (spx_int16_t *)aec, (spx_uint32_t *)&aec_len,
                                            (spx_int16_t *)resample, (spx_uint32_t *)&resample_len);

                ret = csi_i2s_send(i2s_handle, (uint8_t *)resample, resample_len);
            }
        }
    }

    BT_DBG("captrue over");
    aos_pcm_drain(pcm);
    aos_pcm_close(pcm);
    aos_free(resample);
}

static int bt_hfp_init(void)
{
    i2s_handle = csi_i2s_initialize(0, i2s_event_cb_fun, NULL);
    if (!i2s_handle) {
        BT_ERR("i2s init error\n");
        return -1;
    }

    int32_t      ret;
    i2s_config_t i2s_config;
    memset(&i2s_config, 0, sizeof(i2s_config));

    i2s_config.cfg.left_polarity  = I2S_LEFT_POLARITY_LOW;
    i2s_config.cfg.mclk_freq      = I2S_MCLK_256FS;
    i2s_config.cfg.mode           = I2S_MODE_FULL_DUPLEX_MASTER;
    i2s_config.cfg.protocol       = I2S_PROTOCOL_PCM;
    i2s_config.cfg.sclk_freq      = I2S_SCLK_32FS;
    i2s_config.cfg.rx_mono_enable = 0;
    i2s_config.cfg.tx_mono_enable = 1;
    i2s_config.cfg.width          = I2S_SAMPLE_16BIT;
    i2s_config.rate               = 8000;
    i2s_config.rx_buf             = (uint8_t *)i2s_recv_buf;
    i2s_config.rx_buf_length      = I2S_BUF_SIZE;
    i2s_config.rx_period          = I2S_PERIOD_SIZE;
    i2s_config.tx_buf             = (uint8_t *)i2s_send_buf;
    i2s_config.tx_buf_length      = I2S_BUF_SIZE;
    i2s_config.tx_period          = I2S_PERIOD_SIZE;

    ret = csi_i2s_config(i2s_handle, &i2s_config);
    if (ret != 0) {
        BT_ERR("i2s config error %x\n", ret);
        return -1;
    }

    aos_sem_new(&g_sem_hfp_pcm, 0);

    s_pcm_srs = speex_resampler_init((spx_uint32_t)1, (spx_uint32_t)16000, (spx_uint32_t)8000, 0, &g_pcm_error);

    ret = T_Head_audio_init(1, 2, 0, 2);
    if (ret < 0) {
        BT_ERR("T_Head_audio_init FAILED!");
        return -1;
    }

    aos_task_t recv_task_handle;
    aos_task_t mic_task_handle;

    aos_task_new_ext(&recv_task_handle, "hfp_recv", hfp_pcm_recv_entry, NULL, 4096, 28);
    aos_task_new_ext(&mic_task_handle, "hfp_mic", hfp_mic_alsa_entry, NULL, 4096 * 2, AOS_DEFAULT_APP_PRI + 3);

    return 0;
}

void bt_hfp_vol(int vol)
{
    size_t     size;
    ovol_set_t para;

    size = sizeof(para);
    /** convert vol to ao range: 0 ~ 127 -> 0 ~ 255 */
    para.vol_index = vol * 255 / 15;

    if (hfp_snd && hfp_snd->ao) {

        ao_control(hfp_snd->ao, AO_CMD_VOL_SET, (void *)&para, &size);
    }
}

static int bt_hfp_deinit(void)
{
    return 0;
}

static const audio_hfp_ops_t audio_hfp_ops = {
    .init   = bt_hfp_init,
    .deinit = bt_hfp_deinit,
    .start  = start_pcm_send,
    .stop   = stop_pcm_send,
    .vol    = bt_hfp_vol,
};

const audio_hfp_ops_t *bt_hfp_audio_get_interface(void)
{
    return &audio_hfp_ops;
}
#endif

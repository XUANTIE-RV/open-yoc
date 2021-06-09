/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
 
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <drv/timer.h>
#include "silan_syscfg.h"
#include "silan_adev.h"
#include "silan_buffer.h"
//#include "silan_config.h"
#include "codec_adrb.h"
#include "silan_voice_adc.h"
#include <soc.h>

extern ADEV_I2S_HANDLE *pi2s_mic_dev;
extern uint32_t g_sram_mic_lli_size;
extern uint32_t *g_sram_read_ptr;


#if SRAM_TIMER_EN
/* 初始化 */
static timer_handle_t g_lpm_timer;
static void lpm_timer_init(void)
{
    g_lpm_timer = csi_timer_initialize(0, NULL);
    csi_timer_config(g_lpm_timer, TIMER_MODE_RELOAD);
    csi_timer_set_timeout(g_lpm_timer, 700000000);
    csi_timer_start(g_lpm_timer);
}
#endif

void voice_register_vadalg(vad_alg_t alg)
{
extern vad_alg_t g_vad_alg;
    g_vad_alg = alg;
}

void sram_init(void)
{
    extern uint32_t __data_end__, __sram_text_start__, __sram_text_end__,
                    __sram_data_start__, __sram_data_end__;

    uint32_t sram_text_len, sram_data_len;

    sram_text_len = (uint32_t)&__sram_text_end__ - (uint32_t)&__sram_text_start__;
    sram_data_len = (uint32_t)&__sram_data_end__ - (uint32_t)&__sram_data_start__;

    do {
        memcpy((void *)0x20000000, (void *)0x02000000, 0x8000);
        memcpy((void *)&__sram_text_start__, (void *)&__data_end__, sram_text_len);
    } while (memcmp((void *)&__sram_text_start__, (void *)&__data_end__, sram_text_len));
    memcpy((void *)&__sram_data_start__, (void *)((uint32_t)&__data_end__ + sram_text_len),
           sram_data_len);

#if SRAM_TIMER_EN
    lpm_timer_init();
#endif

extern void sram_irq_init(void);
    sram_irq_init();
}

void sram_mic_stop(void)
{
    silan_adev_i2s_stop(pi2s_mic_dev);
}

extern int sram_silan_adev_get_req_num(void);
extern DMA_REQUEST *g_sram_dma_req;
extern DMA_LLI *g_dma_lli;
extern buf_t *g_sram_buf_t;
extern uint32_t *g_sram_read_ptr2;
extern ADEV_MONI_INFO *g_adev_moni;
extern void sram_silan_dmac_i2s_callback(DMA_REQUEST *req);

int32_t sram_silan_adev_i2s_submit(ADEV_I2S_HANDLE *pdev)
{
    int32_t req_num;
    uint32_t *src, lli_size, width_calib, sample_per_1ms, ch_calib;
    uint8_t which_dmac, perid;
    int8_t rx_ch;
    DMA_REQUEST *rx_pdma_req;
    DMA_LLI *rx_pdma_lli;
    ADEV_CFG *padev_cfg;
    I2S_CFG *pi2s_cfg;
    buf_t *rx_pbuf_t;
    int i;

    padev_cfg = &pdev->adev_cfg;
    pi2s_cfg = &pdev->i2s_cfg;

    sample_per_1ms = padev_cfg->sample_rate/1000;
    width_calib = (padev_cfg->width == 16) ? 16 : 32;
    ch_calib = (pi2s_cfg->ch+1)*2;
    lli_size = sample_per_1ms * width_calib / 8 * ch_calib;

    if(pi2s_cfg->tr & I2S_TR_RO) {
        req_num = sram_silan_adev_get_req_num();
        if(req_num < 0) {
            return req_num;
        }
        padev_cfg->rx_num = req_num;

        rx_pdma_req = g_sram_dma_req;
        rx_pdma_lli = g_dma_lli;
        rx_pbuf_t   = g_sram_buf_t;
        memset(rx_pdma_req,        0, sizeof(DMA_REQUEST));
        memset(rx_pdma_lli,        0, sizeof(DMA_LLI)*SRAM_MIC_ADEV_LLI_NUM);
        //dcache_writeback(rx_pdma_lli, sizeof(DMA_LLI)*SRAM_MIC_ADEV_LLI_NUM);

        memset(rx_pbuf_t,        0, sizeof(buf_t));

        silan_buf_init(rx_pbuf_t, (char *)g_sram_read_ptr2, lli_size*ADEV_LLI_BUF_NUM);

        which_dmac = silan_dmac_get_perid(rx_pdma_req->srcid, &perid);
        rx_ch = silan_dmac_get_ch(which_dmac);
        if(rx_ch < 0) {
            return rx_ch;
        }

        padev_cfg->rx_info = g_adev_moni;
        rx_pdma_req->lli_size = lli_size;
        rx_pdma_req->pdev = (void *)pdev;
        rx_pdma_req->callback = (void *)sram_silan_dmac_i2s_callback;
        rx_pdma_req->dma_lli        = rx_pdma_lli;
        rx_pdma_req->burst = padev_cfg->burst;
        rx_pdma_req->result = -1;
        rx_pdma_req->lli_num = SRAM_MIC_ADEV_LLI_NUM;
        rx_pdma_req->ch = rx_ch;
        rx_pdma_req->width = (padev_cfg->width == 24) ? DMAC_WIDTH_32BIT : DMAC_WIDTH_16BIT;
        rx_pdma_req->buf_t = rx_pbuf_t;
        rx_pdma_req->srcid = pi2s_cfg->rx_srcid;
        rx_pdma_req->done_lli = rx_pdma_lli;
        rx_pdma_req->buf_pre_io_bytes = 0;
        rx_pdma_req->dma_dir = DMAC_DIR_P2M;
        src = (void *)&pi2s_cfg->regs->I2S_CHNL[0].RFR;
        which_dmac = silan_dmac_get_perid(rx_pdma_req->srcid, &perid);
        rx_pdma_req->which_dmac = which_dmac;

        for (i = 0; i < SRAM_MIC_ADEV_LLI_NUM; i++) {
            rx_pdma_lli[i].psrcaddr     = (void *)src;
            rx_pdma_lli[i].pdstaddr     = (void *)((uint32_t)rx_pbuf_t->base + rx_pdma_req->lli_size * i);
            if (i != (SRAM_MIC_ADEV_LLI_NUM - 1))
                rx_pdma_lli[i].pnextlli     = (void *)&rx_pdma_lli[i+1];
            rx_pdma_lli[i].num_xfer     = lli_size * 8 / width_calib;
            memset(&rx_pdma_lli[i].raw, 0, sizeof(DMA_RAW_LLI));
        }
        rx_pdma_lli[SRAM_MIC_ADEV_LLI_NUM-1].pnextlli = (void *)&rx_pdma_lli[0];

        pdev->adev_cfg.rx_dma_req = rx_pdma_req;
        silan_dmac_request_transfer(rx_pdma_req);
    }

    return lli_size;
}

void voice_mic_init(int boost_gain, int l_gain, int r_gain)
{
    pi2s_mic_dev->i2s_cfg.id = I2S_ID_I2;

    pi2s_mic_dev->adev_cfg.width	= 16;

    pi2s_mic_dev->i2s_cfg.ch		= I2S_CH_20;
    pi2s_mic_dev->i2s_cfg.ws		= I2S_WS_32;
    pi2s_mic_dev->i2s_cfg.prot	= I2S_PROT_I2S;
    pi2s_mic_dev->i2s_cfg.codec	= CODEC_ID_2;
    pi2s_mic_dev->codec_cfg.adc_sel= CODEC_ADSEL_MIC;
    pi2s_mic_dev->codec_cfg.mic_boost = CODEC_MICBOOST_12;
    pi2s_mic_dev->codec_cfg.adc_diff = CODEC_DIFF;

    silan_adev_i2s_open(pi2s_mic_dev);
    silan_adev_i2s_set_rate(pi2s_mic_dev, 16000);
    g_sram_mic_lli_size = sram_silan_adev_i2s_submit(pi2s_mic_dev);
    silan_adev_i2s_start(pi2s_mic_dev);
    silan_adev_i2s_set_gain(pi2s_mic_dev, 0, 0);

    set_adc2_right_channel_mic_analog_gain(r_gain);
    set_adc2_left_channel_mic_analog_gain(l_gain);
    set_adc2_right_channel_micboost_gain(boost_gain);
    set_adc2_left_channel_micboost_gain(boost_gain);

    memset(g_sram_read_ptr, 0x0, g_sram_mic_lli_size * SRAM_MIC_ADEV_LLI_NUM);
    codec_adrb_create(voice_get_mic_rb_sram(), (char *)g_sram_read_ptr, g_sram_mic_lli_size * SRAM_MIC_ADEV_LLI_NUM);
}

/******************************************
// output voice data struct
******************************************/
extern void *voice_get_ref_rb(void);
extern codec_adrb_t g_sram_mic_ring;

static volatile voice_adc_data_t g_voice_adc_data __attribute__ ((aligned(16)));

static void *voice_get_mic_rb(void)
{
    return &g_sram_mic_ring;
}

void *voice_get_adc_data(void)
{
    g_voice_adc_data.rb_mic = voice_get_mic_rb();
    g_voice_adc_data.rb_ref = voice_get_ref_rb();
    return (void *)&g_voice_adc_data;
}

void voice_set_wake_flag(int wake_flag)
{
    g_voice_adc_data.wake_flag = wake_flag;
}


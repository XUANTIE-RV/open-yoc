/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/log.h>

#include "silan_syscfg.h"
#include "silan_adev.h"
#include "silan_buffer.h"
//#include "silan_config.h"
#include "codec_adrb.h"

#include <csi_core.h>

ADEV_I2S_HANDLE ref_sound_dev;
uint32_t ref_sound_lli_size;

codec_adrb_t g_ref_sound_ring __attribute__ ((aligned(16)));

static uint32_t get_dma_destaddr(uint32_t which_dmac, uint32_t ch)
{
    extern DMAC_CORE_REGS *dmac_regs[];
    volatile DMAC_CHNL_REGS *dma_ch;
    volatile DMA_RAW_LLI *lli;

    dma_ch = &dmac_regs[which_dmac]->sDmaChannels[ch];
    lli = (DMA_RAW_LLI *)dma_ch->lli;

    return dma_to_cpu((uint32_t)lli->dstaddr);
}

void ref_sound_callback(DMA_REQUEST *req)
{
    uint8_t dma_dir = req->dma_dir;
    DMA_LLI *done_lli = req->done_lli;
    buf_t *buf = req->buf_t;
    uint32_t lli_size = req->lli_size;
    uint32_t which_dmac = req->which_dmac;
    uint32_t ch = req->ch;

    if (buf) {
        if(dma_dir == DMAC_DIR_M2P) {
        } else if(dma_dir == DMAC_DIR_P2M) {
            uint32_t write_ptr;
            uint32_t tail;

            req->done_lli = done_lli->pnextlli;
            csi_dcache_clean_invalid_range(done_lli->pdstaddr, lli_size);
            write_ptr = get_dma_destaddr(which_dmac, ch);
            tail = (uint32_t)write_ptr - (uint32_t)buf->base;
            if (tail) {
                tail -= lli_size;
            } else {
                tail = ref_sound_lli_size * (ADEV_LLI_NUM - 1);
            }
            g_ref_sound_ring.tail = tail;
        }
    }
}

void ref_sound_lli_init(DMA_REQUEST *req)
{
    int i;
    DMA_LLI *dma_lli = req->dma_lli;
    buf_t *rx_pbuf_t = req->buf_t;

    for (i = 0; i < ADEV_LLI_NUM; i++) {
        uint32_t pdstaddr = (uint32_t)rx_pbuf_t->base + req->lli_size * i;
        dma_lli[i].pdstaddr = (void *)pdstaddr;
        dma_lli[i].raw.dstaddr = cpu_to_dma(pdstaddr);
        if (i != (ADEV_LLI_NUM - 1)) {
            dma_lli[i].pnextlli = (void *)&dma_lli[i + 1];
            dma_lli[i].raw.nextlli = cpu_to_dma((uint32_t)&dma_lli[i + 1]);
        }
    }
    dma_lli[ADEV_LLI_NUM - 1].pnextlli = (void *)&dma_lli[0];
    dma_lli[ADEV_LLI_NUM - 1].raw.nextlli = cpu_to_dma((uint32_t)&dma_lli[0]);

    csi_dcache_clean_invalid();
}

void voice_ref_init(int l_gain, int r_gain)
{
    ref_sound_dev.i2s_cfg.id = I2S_ID_I1;
    ref_sound_dev.adev_cfg.width        = 16;
    ref_sound_dev.i2s_cfg.ch            = I2S_CH_20;
    ref_sound_dev.i2s_cfg.ws            = I2S_WS_32;
    ref_sound_dev.i2s_cfg.prot  = I2S_PROT_I2S;
    ref_sound_dev.i2s_cfg.codec = CODEC_ID_1;
    ref_sound_dev.codec_cfg.adc_sel= CODEC_ADSEL_AUX;
    ref_sound_dev.codec_cfg.mic_boost = CODEC_MICBOOST_12;
    ref_sound_dev.codec_cfg.adc_diff = CODEC_SINGLE;

    silan_adev_i2s_open(&ref_sound_dev);
    silan_adev_i2s_set_rate(&ref_sound_dev, 16000);
    ref_sound_lli_size = silan_adev_i2s_submit_ex(&ref_sound_dev, ref_sound_callback);
    ref_sound_lli_init((DMA_REQUEST *)ref_sound_dev.adev_cfg.rx_dma_req);
    silan_adev_i2s_start(&ref_sound_dev);
    silan_adev_i2s_set_gain(&ref_sound_dev, 0, 0);

    set_adc1_left_channel_other_analog_gain(l_gain);
    set_adc1_right_channel_other_analog_gain(r_gain);

    codec_adrb_create(&g_ref_sound_ring, (char *)ref_sound_dev.buf_align, ref_sound_lli_size * ADEV_LLI_NUM);
}


void *voice_get_ref_rb(void)
{
    return &g_ref_sound_ring;
}

void voice_ref_stop(void)
{
    silan_adev_i2s_stop(&ref_sound_dev);
}

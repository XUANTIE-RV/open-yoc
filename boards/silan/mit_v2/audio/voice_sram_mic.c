/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "silan_syscfg.h"
#include "silan_adev.h"
#include "silan_buffer.h"
//#include "silan_config.h"
#include "codec_adrb.h"
#include "silan_voice_adc.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))

typedef struct codec_buffer {
    uint8_t *buffer;
    uint32_t size;
    uint32_t write;
    uint32_t read;
} codec_buffer_t;

extern void sram_irq_init(void);
extern void *sram_memset(void *s, int c, size_t n);

SRAM_BSS uint32_t g_sram_mic_lli_size;

SRAM_BSS static ADEV_I2S_HANDLE __sram_adev_handler;
SRAM_DATA ADEV_I2S_HANDLE *pi2s_mic_dev = &__sram_adev_handler;

SRAM_BSS static DMA_REQUEST __sram_dma_req[1];
SRAM_BSS static buf_t __sram_buf_t[1];
SRAM_BSS static int __sram_mic_buf[1][ADEV_MAX_LLI_SIZE*SRAM_MIC_ADEV_LLI_NUM/sizeof(int)] __attribute__ ((aligned(16)));
//SRAM_TEXT2 static int __sram_mic_buf[1][ADEV_MAX_LLI_SIZE*SRAM_MIC_ADEV_LLI_NUM/sizeof(int)] __attribute__ ((aligned(16)));
SRAM_BSS static DMA_LLI __dma_lli[1][SRAM_MIC_ADEV_LLI_NUM] __attribute__ ((aligned(16)));
SRAM_BSS static ADEV_MONI_INFO __adev_moni[1];

SRAM_DATA uint32_t *g_sram_read_ptr = (uint32_t *)&__sram_mic_buf;
SRAM_DATA uint32_t *g_sram_read_ptr2 = (uint32_t *)&__sram_mic_buf[0][0];

SRAM_DATA uint32_t *g_sram_write_ptr = (uint32_t *)&__sram_mic_buf;

SRAM_BSS codec_adrb_t g_sram_mic_ring __attribute__ ((aligned(16)));

SRAM_DATA DMA_REQUEST *g_sram_dma_req = &__sram_dma_req[0];
SRAM_DATA DMA_LLI *g_dma_lli = &__dma_lli[0][0];
SRAM_DATA buf_t *g_sram_buf_t = &__sram_buf_t[0];
SRAM_DATA ADEV_MONI_INFO *g_adev_moni = &__adev_moni[0];

#if 0
SRAM_TEXT void reset___sram_mic_buf(void)
{
    sram_memset(&__sram_mic_buf, 0xFF, sizeof(__sram_mic_buf));
}
#endif

SRAM_TEXT int sram_silan_adev_get_req_num(void)
{
    return 0;
}

SRAM_TEXT static uint32_t get_dma_destaddr(uint32_t ch)
{
    volatile DMAC_CHNL_REGS *dma_ch;
    volatile DMA_RAW_LLI *lli;

    dma_ch = (DMAC_CHNL_REGS *)(0x43300100 + ch * 0x20);
    lli = (DMA_RAW_LLI *)dma_ch->lli;

    return lli->dstaddr;
}

SRAM_TEXT void sram_silan_dmac_i2s_callback(DMA_REQUEST *req)
{
    uint8_t dma_dir = req->dma_dir;

    DMA_LLI *done_lli;
    done_lli = req->done_lli;
    buf_t *buf = req->buf_t;
    uint32_t lli_size = req->lli_size;

    if (buf) {
        if(dma_dir == DMAC_DIR_M2P) {
        } else if(dma_dir == DMAC_DIR_P2M) {
            uint32_t write_ptr;
            uint32_t tail;
            req->done_lli = done_lli->pnextlli;
            write_ptr = get_dma_destaddr(req->ch);
            tail = (uint32_t)write_ptr - (uint32_t)buf->base;
            if (tail) {
                tail -= lli_size;
            } else {
                tail = g_sram_mic_lli_size * (SRAM_MIC_ADEV_LLI_NUM - 1);
            }
            g_sram_mic_ring.tail = tail;
            g_sram_write_ptr = (uint32_t *)((uint32_t)buf + tail);
        }
    }
}

SRAM_TEXT void *voice_get_mic_rb_sram(void)
{
    return &g_sram_mic_ring;
}

#define MODULE_NAME        "ADEV"
//#define __DEBUG__

#include "stdlib.h"
#include "string.h"
#include "silan_printf.h"
#include "silan_errno.h"
#include "silan_buffer.h"
#include "silan_adev.h"
#include "silan_syscfg.h"
#include "silan_audio_clkgen.h"
#include "silan_codec.h"
#include "silan_dmac.h"
#define I2S_BUF_STATIC (0)

#if defined(__XCC__)

#ifdef ADEV_NOT_USE_SWAP
#if (ADEV_MAX_LLI_SIZE%64)
#error DMAC_MAX_LLI_SIZE must 64B aligned!
#endif

#if (ADEV_MAX_DMAC_CHNL>ADMAC_MAX_CHANNELS)
#error ADEV_MAX_DMAC_CHNL bigger than ADMAC Max CH!
#endif

static int            __buf[ADEV_MAX_DMAC_CHNL][ADEV_MAX_LLI_SIZE*ADEV_LLI_NUM/sizeof(int)]    __attribute__ ((aligned(64)));
static DMA_LLI        __dma_lli[ADEV_MAX_DMAC_CHNL + 1][ADEV_LLI_NUM]                    __attribute__ ((aligned(64)));
#else
static int            __buf[ADEV_MAX_DMAC_CHNL][ADEV_MAX_LLI_SIZE*ADEV_LLI_NUM/sizeof(int)]    __attribute__ ((section(".dsp_dram")));
static DMA_LLI        __dma_lli[ADEV_MAX_DMAC_CHNL][ADEV_LLI_NUM]                        __attribute__ ((section(".dsp_dram")));
#endif

#endif

#if defined(__CC_ARM)
static int            __buf[ADEV_MAX_DMAC_CHNL][ADEV_MAX_LLI_SIZE*ADEV_LLI_NUM/sizeof(int)];
static DMA_LLI        __dma_lli[ADEV_MAX_DMAC_CHNL][ADEV_LLI_NUM];
#endif

#if defined(__CSKY__)
#if I2S_BUF_STATIC
static int            __buf[ADEV_MAX_DMAC_CHNL][ADEV_MAX_LLI_SIZE*ADEV_LLI_BUF_NUM/sizeof(int)]    __attribute__ ((aligned(16)));
static DMA_LLI        __dma_lli[ADEV_MAX_DMAC_CHNL + 1][ADEV_LLI_NUM]                    __attribute__ ((aligned(16)));
#endif
#endif

#define ALIGN_SIZE(size, align) (((size) + align - 1) & ~(align - 1))

static DMA_REQUEST    __dma_req[ADEV_MAX_DMAC_CHNL];
static buf_t         __buf_t[ADEV_MAX_DMAC_CHNL];

static uint8_t        __adev_req[ADEV_MAX_DMAC_CHNL];

static int            *__zero = (int *)(SRAM1_ADDR_BASE + 0x00000);
static int            *__wbuf = (int *)(SRAM1_ADDR_BASE + 0x10000);

static ADEV_I2S_HANDLE __adev_handler[I2S_ID_LMT];
// static uint32_t        __adev_handler_index = 0;

static ADEV_MONI_INFO __adev_moni[ADEV_MAX_DMAC_CHNL];

static int32_t silan_adev_get_req_num(void)
{
    int i;

    for(i=0; i<ADEV_MAX_DMAC_CHNL; i++) {
        if(__adev_req[i] == 0) {
            __adev_req[i] = 1;
            return i;
        }
    }
    SL_LOG("No more Space");
    return -ENOSPC;
}

static int32_t silan_adev_release_req_num(uint8_t req_num)
{
    if(__adev_req[req_num] == 1) {
        __adev_req[req_num] = 0;
        return ENONE;
    }

    SL_LOG("No Such Sum");
    return -EINVAL;
}

extern dma_cb_t g_dma_cb;

static void silan_dmac_i2s_callback(DMA_REQUEST *req)
{
    int *data, bytes, offset, lli_size;
    uint8_t dma_dir = req->dma_dir;

    DMA_LLI *next_lli, *done_lli;
    next_lli = silan_dmac_get_next_lli(req);
    done_lli = req->done_lli;
    buf_t *buf = req->buf_t;
    lli_size = req->lli_size;

#if defined(__XCC__)
    __adev_moni[ch].intr_total ++;
    ccount = xthal_get_ccount();
    if (__adev_moni[ch].intr_cnt == 0) {
        __adev_moni[ch].last_ccount = ccount;
        __adev_moni[ch].intr_cnt = 1;
    } else {
        __adev_moni[ch].intr_cnt++;
        if (__adev_moni[ch].intr_cnt > __adev_moni[ch].intr_num) {
            if ( ccount > __adev_moni[ch].last_ccount) {
                __adev_moni[ch].diff_ccount = ccount - __adev_moni[ch].last_ccount;
            } else {
                __adev_moni[ch].diff_ccount = 0xfffffffe - __adev_moni[ch].last_ccount + ccount;
            }
            __adev_moni[ch].last_ccount = ccount;
            __adev_moni[ch].intr_cnt = 1;
        }
    }
#endif
    if (buf) {
        if(dma_dir == DMAC_DIR_M2P) {
#if 0
            /* FIXME, try to fix dma irq lost */
            while (done_lli->pnextlli->pnextlli != next_lli) {
                if (done_lli->psrcaddr != __zero) {
                    SL_ASSERT(done_lli->psrcaddr == buf->rd);
                    SL_ASSERT(bytes == lli_size);
                }
                done_lli = done_lli->pnextlli;
                req->done_lli = done_lli;
            }
#endif

            if (done_lli->psrcaddr != __zero) {
                SL_ASSERT(done_lli->psrcaddr == buf->rd);
                bytes = silan_buf_read(buf, lli_size);
                SL_ASSERT(bytes == lli_size);
                req->buf_pre_io_bytes -= lli_size;

                done_lli->psrcaddr     = (void *)__zero;
                done_lli->raw.srcaddr = (uint32_t)ptr_cpu_to_dma((uint32_t *)__zero);
                dcache_writeback(done_lli, sizeof(*done_lli));
            }

            offset = req->buf_pre_io_bytes;
            bytes = silan_buf_pre_read(buf, (char **)&data, offset, lli_size);
            SL_ASSERT((bytes == 0) || (bytes == lli_size));
            if (bytes == lli_size) {
                int cnt = 0;
                while (cnt < ADEV_LLI_NUM) {
                    cnt++;
                    if (next_lli->psrcaddr == __zero) {
                        next_lli->psrcaddr     = (void *)data;
                        next_lli->raw.srcaddr  = (uint32_t)ptr_cpu_to_dma((uint32_t *)data);
                        req->buf_pre_io_bytes += lli_size;
                        dcache_writeback(next_lli, sizeof(*next_lli));
                        break;
                    }
                    next_lli = next_lli->pnextlli;
                }
            } else {
                //sl_printf("#");
            }
            req->done_lli = done_lli->pnextlli;
        } else if(dma_dir == DMAC_DIR_P2M) {
#if 0
            /* FIXME, check if dma irq is lost. */
            while (done_lli->pnextlli->pnextlli != next_lli) {
                if (done_lli->pdstaddr != __wbuf) {
                    SL_ASSERT(done_lli->pdstaddr == buf->wt);
                    SL_ASSERT(bytes == lli_size);
                }
                done_lli = done_lli->pnextlli;
                req->done_lli = done_lli;
            }
#endif
            if (done_lli->pdstaddr != __wbuf) {
                SL_ASSERT(done_lli->pdstaddr == buf->wt);
                bytes = silan_buf_write(buf, lli_size);
                SL_ASSERT(bytes == lli_size);
                req->buf_pre_io_bytes -= lli_size;
            }

            offset = req->buf_pre_io_bytes;
            bytes  = silan_buf_pre_write(buf, (char **)&data, offset, lli_size);

            SL_ASSERT((bytes == 0) || (bytes == lli_size));
            if (bytes == lli_size) {
                req->buf_pre_io_bytes += lli_size;
            } else {
                data = (int *)__wbuf;
            }
            next_lli->pdstaddr      = (void *)data;
            next_lli->raw.dstaddr = (uint32_t)ptr_cpu_to_dma((uint32_t *)data);
            dcache_writeback(next_lli, sizeof(*next_lli));

            req->done_lli = done_lli->pnextlli;
            if(g_dma_cb.cb != NULL)
                g_dma_cb.cb(1,g_dma_cb.priv);
        }
    }
}

dma_cb_t g_dma_cb;
void silan_dma_cb_reg(silan_dma_cb cb, void *priv)
{
    g_dma_cb.cb = cb;
    g_dma_cb.priv = priv;
}

int32_t silan_adev_i2s_set_rate(ADEV_I2S_HANDLE *pdev, int rate)
{
    uint8_t codec_id, tr;

    codec_id = pdev->i2s_cfg.codec;
    tr = pdev->i2s_cfg.tr;

    pdev->adev_cfg.sample_rate = rate;
    silan_i2s_clock_config(pdev->i2s_cfg.id, rate);

    if(codec_id & CODEC_ID_1) {
        if(tr & I2S_TR_TO)
            silan_codec_dac_config(CODEC1, rate);
        if(tr & I2S_TR_RO)
            silan_codec_adc_config(CODEC1, rate);
    }
    if(codec_id & CODEC_ID_2) {
        if(tr & I2S_TR_TO)
            silan_codec_dac_config(CODEC2, rate);
        if(tr & I2S_TR_RO)
            silan_codec_adc_config(CODEC2, rate);
    }

    return ENONE;
}

int32_t silan_adev_i2s_set_gain(ADEV_I2S_HANDLE *pdev, int32_t l_gain, int32_t r_gain)
{
    uint8_t codec_id, tr;
    int32_t codec_l, codec_r;

    codec_id = pdev->i2s_cfg.codec;
    tr = pdev->i2s_cfg.tr;

    if(tr == I2S_TR_TO) {
        if((l_gain < CODEC_DAC_GAIN_MIN) && (r_gain < CODEC_DAC_GAIN_MIN)) {
            if(codec_id & CODEC_ID_1)
                silan_codec_dac_mute(CODEC1);
            if(codec_id & CODEC_ID_2)
                silan_codec_dac_mute(CODEC2);
        } else {
            codec_l = (l_gain - CODEC_DAC_GAIN_MIN)/CODEC_DAC_GAIN_STEP;
            codec_r = (r_gain - CODEC_DAC_GAIN_MIN)/CODEC_DAC_GAIN_STEP;
            if(codec_id & CODEC_ID_1) {
                silan_codec_dac_set_gain(CODEC1, codec_l, codec_r);
                silan_codec_dac_unmute(CODEC1);
            }
            if(codec_id & CODEC_ID_2) {
                silan_codec_dac_set_gain(CODEC2, codec_l, codec_r);
                silan_codec_dac_unmute(CODEC2);
            }
        }
    }

    if(tr == I2S_TR_RO) {
        codec_l = (l_gain - CODEC_ADC_GAIN_MIN)/CODEC_ADC_GAIN_STEP;
        codec_r = (r_gain - CODEC_ADC_GAIN_MIN)/CODEC_ADC_GAIN_STEP;
        if(codec_id & CODEC_ID_1) {
            silan_codec_adc_set_gain(CODEC1, codec_l, codec_r);
        }
        if(codec_id & CODEC_ID_2) {
            silan_codec_adc_set_gain(CODEC2, codec_l, codec_r);
        }
    }

    return ENONE;
}

static int32_t _silan_adev_i2s_submit(ADEV_I2S_HANDLE *pdev, void *cb)
{
    int32_t req_num;
    uint32_t *src, *dst, lli_size, width_calib, sample_per_1ms, ch_calib;
    uint8_t which_dmac, perid;
    int8_t rx_ch, tx_ch;
    DMA_REQUEST *tx_pdma_req, *rx_pdma_req;
    DMA_LLI *tx_pdma_lli, *rx_pdma_lli;
    ADEV_CFG *padev_cfg;
    I2S_CFG *pi2s_cfg;
    buf_t *rx_pbuf_t, *tx_pbuf_t;
    char *buf, *lli_buf;
    int align_size;
    int i;

    padev_cfg = &pdev->adev_cfg;
    pi2s_cfg = &pdev->i2s_cfg;

    sample_per_1ms = padev_cfg->sample_rate/1000;
    width_calib = (padev_cfg->width == 16) ? 16 : 32;
    ch_calib = (pi2s_cfg->ch+1)*2;
    lli_size = sample_per_1ms * width_calib / 8 * ch_calib;
    SL_DBGLOG("I2S ID-%d LLI_SIZE: %d", pi2s_cfg->id, lli_size);

    if(pi2s_cfg->tr & I2S_TR_TO) {
        req_num = silan_adev_get_req_num();
        if(req_num < 0) {
            SL_LOG("No More Space.");
            return req_num;
        }
        padev_cfg->tx_num = req_num;

        tx_pdma_req = &__dma_req[req_num];

#if I2S_BUF_STATIC
        tx_pdma_lli = &__dma_lli[req_num][0];
#else
        align_size = (sizeof(DMA_LLI)*ADEV_LLI_NUM) + 32;
        lli_buf = malloc(align_size);
        if (NULL == lli_buf) {
            SL_LOG("i2s oom.");
            return -1;
        }
        tx_pdma_lli = (DMA_LLI*)ALIGN_SIZE((int)lli_buf, 16);
#endif
        tx_pbuf_t   = &__buf_t[req_num];
        memset(tx_pdma_req,        0, sizeof(DMA_REQUEST));
        memset(tx_pdma_lli,        0, sizeof(DMA_LLI)*ADEV_LLI_NUM);
        //dcache_writeback(tx_pdma_lli, sizeof(DMA_LLI)*ADEV_LLI_NUM);

        memset(tx_pbuf_t,        0, sizeof(buf_t));

#if I2S_BUF_STATIC
        silan_buf_init(tx_pbuf_t, (char *)&__buf[req_num][0], lli_size*ADEV_LLI_BUF_NUM);
#else
        align_size = (lli_size*ADEV_LLI_BUF_NUM) + 32;
        buf = malloc(align_size);
        if (NULL == buf) {
            SL_LOG("i2s oom.");
            return -1;
        }
        pdev->buf       = buf;
        pdev->buf_align = (char*)ALIGN_SIZE((int)buf, 16);
        silan_buf_init(tx_pbuf_t, pdev->buf_align, lli_size*ADEV_LLI_BUF_NUM);
#endif

        which_dmac = silan_dmac_get_perid(tx_pdma_req->srcid, &perid);
        tx_ch = silan_dmac_get_ch(which_dmac);
        if(tx_ch < 0) {
            SL_LOG("No DMAC CH.");
            return tx_ch;
        }

        padev_cfg->tx_info = &__adev_moni[tx_ch];
        tx_pdma_req->lli_size = lli_size;
        tx_pdma_req->pdev = (void *)pdev;
        if (cb) {
            tx_pdma_req->callback = (void *)cb;
        } else {
            tx_pdma_req->callback = (void *)silan_dmac_i2s_callback;
        }
#if I2S_BUF_STATIC
        tx_pdma_req->dma_lli        = tx_pdma_lli;
#else
        tx_pdma_req->dma_lli        = tx_pdma_lli;
        tx_pdma_req->dma_lli_origin = (DMA_LLI*)lli_buf;
#endif
        tx_pdma_req->burst = padev_cfg->burst;
        tx_pdma_req->result = -1;
        tx_pdma_req->lli_num = ADEV_LLI_NUM;
        tx_pdma_req->ch = tx_ch;
        tx_pdma_req->width = (padev_cfg->width == 24) ? DMAC_WIDTH_32BIT : DMAC_WIDTH_16BIT;
        tx_pdma_req->buf_t = (void *)tx_pbuf_t;
        tx_pdma_req->dma_dir = DMAC_DIR_M2P;
        tx_pdma_req->srcid = pi2s_cfg->tx_srcid;
        tx_pdma_req->done_lli = tx_pdma_lli;
        tx_pdma_req->buf_pre_io_bytes = 0;
        src = (void *)__zero;
        dst = (void *)&pi2s_cfg->regs->I2S_CHNL[0].TFR;
        tx_pdma_req->which_dmac = which_dmac;

        for (i = 0; i < ADEV_LLI_NUM; i++) {
            tx_pdma_lli[i].psrcaddr     = (void *)src;
            tx_pdma_lli[i].pdstaddr     = (void *)dst;
            if (i != (ADEV_LLI_NUM - 1))
                tx_pdma_lli[i].pnextlli     = (void *)&tx_pdma_lli[i+1];
            tx_pdma_lli[i].num_xfer     = lli_size * 8 / width_calib;
            memset(&tx_pdma_lli[i].raw, 0, sizeof(DMA_RAW_LLI));
        }
        tx_pdma_lli[ADEV_LLI_NUM-1].pnextlli = (void *)&tx_pdma_lli[0];

        pdev->adev_cfg.tx_dma_req = tx_pdma_req;
        SL_DBGLOG("TX REQ Submit: %d", req_num);
        silan_dmac_request_transfer(tx_pdma_req);
    }
    if(pi2s_cfg->tr & I2S_TR_RO) {
        req_num = silan_adev_get_req_num();
        if(req_num < 0) {
            SL_LOG("No More Space.");
            return req_num;
        }
        padev_cfg->rx_num = req_num;

        rx_pdma_req = &__dma_req[req_num];
#if I2S_BUF_STATIC
        rx_pdma_lli = &__dma_lli[req_num][0];
#else
        align_size = (sizeof(DMA_LLI)*ADEV_LLI_NUM) + 32;
        lli_buf = malloc(align_size);
        if (NULL == lli_buf) {
            SL_LOG("i2s oom.");
            return -1;
        }
        rx_pdma_lli = (DMA_LLI*)ALIGN_SIZE((int)lli_buf, 16);
#endif
        rx_pbuf_t   = &__buf_t[req_num];
        memset(rx_pdma_req,        0, sizeof(DMA_REQUEST));
        memset(rx_pdma_lli,        0, sizeof(DMA_LLI)*ADEV_LLI_NUM);
        //dcache_writeback(rx_pdma_lli, sizeof(DMA_LLI)*ADEV_LLI_NUM);

        memset(rx_pbuf_t,        0, sizeof(buf_t));


#if I2S_BUF_STATIC
        silan_buf_init(rx_pbuf_t, (char *)&__buf[req_num][0], lli_size*ADEV_LLI_BUF_NUM);
#else
        align_size = (lli_size*ADEV_LLI_BUF_NUM) + 32;
        buf        = malloc(align_size);
        if (NULL == buf) {
            SL_LOG("i2s oom.");
            return -1;
        }
        pdev->buf       = buf;
        pdev->buf_align = (char*)ALIGN_SIZE((int)buf, 16);
        silan_buf_init(rx_pbuf_t, pdev->buf_align, lli_size*ADEV_LLI_BUF_NUM);
#endif

        which_dmac = silan_dmac_get_perid(rx_pdma_req->srcid, &perid);
        rx_ch = silan_dmac_get_ch(which_dmac);
        if(rx_ch < 0) {
            SL_LOG("No DMAC CH.");
            return rx_ch;
        }

        padev_cfg->rx_info = &__adev_moni[rx_ch];
        rx_pdma_req->lli_size = lli_size;
        rx_pdma_req->pdev = (void *)pdev;
        if (cb) {
            rx_pdma_req->callback = (void *)cb;
        } else {
            rx_pdma_req->callback = (void *)silan_dmac_i2s_callback;
        }
#if I2S_BUF_STATIC
        rx_pdma_req->dma_lli        = rx_pdma_lli;
#else
        rx_pdma_req->dma_lli        = rx_pdma_lli;
        rx_pdma_req->dma_lli_origin = (DMA_LLI*)lli_buf;
#endif
        rx_pdma_req->burst = padev_cfg->burst;
        rx_pdma_req->result = -1;
        rx_pdma_req->lli_num = ADEV_LLI_NUM;
        rx_pdma_req->ch = rx_ch;
        rx_pdma_req->width = (padev_cfg->width == 24) ? DMAC_WIDTH_32BIT : DMAC_WIDTH_16BIT;
        rx_pdma_req->buf_t = rx_pbuf_t;
        rx_pdma_req->srcid = pi2s_cfg->rx_srcid;
        rx_pdma_req->done_lli = rx_pdma_lli;
        rx_pdma_req->buf_pre_io_bytes = 0;
        rx_pdma_req->dma_dir = DMAC_DIR_P2M;
        src = (void *)&pi2s_cfg->regs->I2S_CHNL[0].RFR;
        dst = (void *)__wbuf;
        which_dmac = silan_dmac_get_perid(rx_pdma_req->srcid, &perid);
        rx_pdma_req->which_dmac = which_dmac;

        for (i = 0; i < ADEV_LLI_NUM; i++) {
            rx_pdma_lli[i].psrcaddr     = (void *)src;
            rx_pdma_lli[i].pdstaddr     = (void *)dst;
            if (i != (ADEV_LLI_NUM - 1))
                rx_pdma_lli[i].pnextlli     = (void *)&rx_pdma_lli[i+1];
            rx_pdma_lli[i].num_xfer     = lli_size * 8 / width_calib;
            memset(&rx_pdma_lli[i].raw, 0, sizeof(DMA_RAW_LLI));
        }
        rx_pdma_lli[ADEV_LLI_NUM-1].pnextlli = (void *)&rx_pdma_lli[0];

        pdev->adev_cfg.rx_dma_req = rx_pdma_req;
        SL_LOG("RX REQ Submit: %d", req_num);
        silan_dmac_request_transfer(rx_pdma_req);
    }

    return lli_size;
}

int32_t silan_adev_i2s_submit(ADEV_I2S_HANDLE *pdev)
{
    return _silan_adev_i2s_submit(pdev, NULL);
}

int32_t silan_adev_i2s_submit_ex(ADEV_I2S_HANDLE *pdev, void *cb)
{
    return _silan_adev_i2s_submit(pdev, cb);
}

int32_t silan_adev_i2s_revoke(ADEV_I2S_HANDLE *pdev)
{
    DMA_LLI        *dma_lli;
    ADEV_CFG *padev_cfg = &pdev->adev_cfg;

    if(padev_cfg->tx_dma_req) {
        silan_adev_release_req_num(padev_cfg->tx_num);
        silan_dmac_release_transfer(padev_cfg->tx_dma_req);

        dma_lli = padev_cfg->tx_dma_req->dma_lli_origin;
        if (dma_lli) {
            free(dma_lli);
        }
        padev_cfg->tx_dma_req->dma_lli        = NULL;
        padev_cfg->tx_dma_req->dma_lli_origin = NULL;
    }

    if(padev_cfg->rx_dma_req) {
        silan_adev_release_req_num(padev_cfg->rx_num);
        silan_dmac_release_transfer(padev_cfg->rx_dma_req);

        dma_lli = padev_cfg->rx_dma_req->dma_lli_origin;
        if (dma_lli) {
            free(dma_lli);
        }
        padev_cfg->rx_dma_req->dma_lli        = NULL;
        padev_cfg->rx_dma_req->dma_lli_origin = NULL;
    }

    if (pdev->buf) {
        free(pdev->buf);
        pdev->buf       = NULL;
        pdev->buf_align = NULL;
    }

    return ENONE;
}

int32_t silan_adev_i2s_start(ADEV_I2S_HANDLE *pdev)
{
    uint8_t tr = pdev->i2s_cfg.tr;
    if(tr & I2S_TR_TO) {
#ifdef I2S_PCM_DEBUG
		g_pcm_size = 0;
#endif
        silan_dmac_request_start(pdev->adev_cfg.tx_dma_req);
	}
    if(tr & I2S_TR_RO)
        silan_dmac_request_start(pdev->adev_cfg.rx_dma_req);
    silan_i2s_start(&pdev->i2s_cfg);
    return ENONE;
}

int32_t silan_adev_i2s_stop(ADEV_I2S_HANDLE *pdev)
{
    uint8_t tr = pdev->i2s_cfg.tr;
    if(tr & I2S_TR_TO)
        silan_dmac_request_stop(pdev->adev_cfg.tx_dma_req);
    if(tr & I2S_TR_RO)
        silan_dmac_request_stop(pdev->adev_cfg.rx_dma_req);
    silan_i2s_stop(&pdev->i2s_cfg);

    return ENONE;
}

int32_t silan_adev_i2s_open(ADEV_I2S_HANDLE *pdev)
{
    uint8_t codec_id, tr, burst_calib;
    CODEC_CFG *pcodec;
    ADEV_CFG *padev_cfg;
    I2S_CFG *pi2s_cfg;
    int32_t ret;

    pcodec = &pdev->codec_cfg;
    padev_cfg = &pdev->adev_cfg;
    pi2s_cfg = &pdev->i2s_cfg;

    // On-chip CODEC1&CODEC2, fix master and slave mode
    if(pi2s_cfg->codec > CODEC_ID_EXT) {
        if((pi2s_cfg->id == I2S_ID_O1) || (pi2s_cfg->id == I2S_ID_O2))
            pi2s_cfg->master = CLK_MASTER;
        else if((pi2s_cfg->id == I2S_ID_I1) || (pi2s_cfg->id == I2S_ID_I2))
            pi2s_cfg->master = CLK_SLAVE;
    }

    // Default Config
    padev_cfg->burst = DMAC_BURST_SIZE_4;
    burst_calib = padev_cfg->burst + 1;
    if(burst_calib == 1)
        burst_calib = 0;
    pi2s_cfg->burst = (1 << burst_calib);
    pi2s_cfg->wlen = (padev_cfg->width == 16) ? I2S_WLEN_16 :
                     I2S_WLEN_24;

    ret = silan_i2s_route_config(pi2s_cfg->id, pi2s_cfg->codec, pi2s_cfg->master);
    if(ret != 0) {
        SL_LOG("I2S and CODEC config Error");
        return ret;
    }

    ret = silan_i2s_dev_open(pi2s_cfg);
    if(ret != 0) {
        SL_LOG("No I2S Device");
        return ret;
    }

    SL_DBGLOG("I2S Device: ID=%d", pi2s_cfg->id);
    SL_DBGLOG("  WIDTH: %d", padev_cfg->width);
    SL_DBGLOG("  CH:    %d", (pi2s_cfg->ch+1) << 1);
    SL_DBGLOG("  CODEC: %d", pi2s_cfg->codec);

    tr = pi2s_cfg->tr;
    codec_id = (pdev->i2s_cfg.codec);
    if(codec_id & CODEC_ID_1) {
        if(tr & I2S_TR_TO) {
            pcodec->dac_wl = (padev_cfg->width == 16) ? CODEC_WL_16 :
                             CODEC_WL_24;
            pcodec->dac_master = CLK_SLAVE;
            if(silan_codec_check(CODEC1) == 0) {
                silan_codec_dac_open(CODEC1);
            }
            silan_codec_dac_init(CODEC1, pcodec);
        }
        if(tr & I2S_TR_RO) {
            pcodec->adc_wl = (padev_cfg->width == 16) ? CODEC_WL_16 :
                             CODEC_WL_24;
            pcodec->adc_master = CLK_MASTER;
            silan_codec_adc_open(CODEC1);
            silan_codec_adc_init(CODEC1, pcodec);
        }
    }
    if(codec_id & CODEC_ID_2) {
        if(tr & I2S_TR_TO) {
            pcodec->dac_wl = (padev_cfg->width == 16) ? CODEC_WL_16 :
                             CODEC_WL_24;
            pcodec->dac_master = CLK_SLAVE;
            if(silan_codec_check(CODEC2) == 0) {
                silan_codec_dac_open(CODEC2);
            }
            silan_codec_dac_init(CODEC2, pcodec);
        }
        if(tr & I2S_TR_RO) {
            pcodec->adc_wl = (padev_cfg->width == 16) ? CODEC_WL_16 :
                             CODEC_WL_24;
            pcodec->adc_master = CLK_MASTER;
            silan_codec_adc_open(CODEC2);
            silan_codec_adc_init(CODEC2, pcodec);
        }
    }

    return ENONE;
}

ADEV_I2S_HANDLE *silan_adev_i2s_alloc(I2S_ID i2s)
{
    ADEV_I2S_HANDLE *pdev;
    if(i2s > (I2S_ID_LMT-1)) {
        SL_LOG("ADEV I2S alloc fail.");
        return NULL;
    }
    pdev = &__adev_handler[i2s];
    memset(pdev, 0, sizeof(ADEV_I2S_HANDLE));
    pdev->i2s_cfg.id = i2s;

    return pdev;
}

int silan_adev_get_buffer_size(void *pdev)
{
    int size;
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;
    buf_t *buf = adev_cfg->tx_dma_req->buf_t;

    size = (int)(buf->lmt - buf->base);
    return size;
}

int32_t silan_adev_get_write_size(void *pdev)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;
    buf_t *buf = adev_cfg->tx_dma_req->buf_t;
    char *rd, *wt;
    int ret = 0;

    wt = (char*)buf->wt;
    rd = (char*)buf->rd;

    if (rd < wt) {
        if (rd == buf->base) {
            ret = (int)(buf->lmt - wt - 1);
        } else {
            ret = (int)(buf->lmt - wt);
        }
    } else if (rd > wt) {
        ret = (int)(rd - wt - 1);
    } else {
        if(rd == buf->base) {
            ret = (int)(buf->lmt - wt - 1);
        } else {
            ret = (int)(buf->lmt - wt);
        }
    }

    return ret;
}

int32_t silan_adev_pre_write(void *pdev, char **write_p, int bytes)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;

    return silan_buf_pre_write(adev_cfg->tx_dma_req->buf_t, write_p, 0, bytes);
}

int32_t silan_adev_write(void *pdev, int bytes)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;

    return silan_buf_write(adev_cfg->tx_dma_req->buf_t, bytes);
}

int32_t silan_adev_get_read_size(void *pdev)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;
    buf_t *buf = adev_cfg->tx_dma_req->buf_t;
    char *rd, *wt;

    wt = (char*)buf->wt;
    rd = (char*)buf->rd;

    if (rd == wt) {
        return 0;
    } else if (rd < wt) {
        return (int)(wt - rd);
    } else { /*if (rd > wt)*/
        return (int)(buf->lmt - rd);
    }
}

int32_t silan_adev_pre_read(void *pdev, char **read_p, int bytes)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;

    return silan_buf_pre_read(adev_cfg->rx_dma_req->buf_t, read_p, 0, bytes);
}

int32_t silan_adev_read(void *pdev, int bytes)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;

    return silan_buf_read(adev_cfg->rx_dma_req->buf_t, bytes);
}

int32_t silan_adev_update_fullness(void *pdev)
{
    ADEV_CFG *adev_cfg = (ADEV_CFG *)pdev;
    DMA_REQUEST *req;
    ADEV_MONI_INFO *info;
    buf_t *req_buf;
    char *rd, *wt;
    uint32_t *src, *dst;
    uint32_t cur, pre;

    req = adev_cfg->tx_dma_req;
    info = adev_cfg->tx_info;
    if(req != NULL) {
        req_buf = (buf_t *)req->buf_t;
        rd = (char*)req_buf->rd;
        info->buf_fullness = silan_buf_get_fullness(req->buf_t);
        silan_dmac_get_cur_addr(req, &src, &dst);
        //sl_printf("tx %p, %p\n", rd, src);
        cur = (uint32_t)src;
        pre = (uint32_t)rd;
        if((cur & 0xFF001000) == (uint32_t)__zero) {
            SL_LOG("Tx Empty!");
            info->buf_fullness = 0;
        } else {
            if(cur >= pre) {
                info->buf_fullness -= (cur - pre);
            } else {
                SL_LOG("What TX!");
            }
        }
    }

    req = adev_cfg->rx_dma_req;
    info = adev_cfg->rx_info;
    if(req != NULL) {
        req_buf = (buf_t *)req->buf_t;
        wt = (char*)req_buf->wt;
        info->buf_fullness = silan_buf_get_fullness(req->buf_t);
        silan_dmac_get_cur_addr(req, &src, &dst);
        //sl_printf("rx %p, %p\n", wt, dst);
        cur = (uint32_t)dst;
        pre = (uint32_t)wt;
        if((cur & 0xFF010000) == (uint32_t)__wbuf) {
            SL_LOG("Rx Full!");
            info->buf_fullness = 0;
        } else {
            if(cur >= pre) {
                info->buf_fullness += ((cur - pre) & 0x00FFFFFF);
            } else {
                SL_LOG("What RX!");
            }
        }
    }

    return ENONE;
}

void silan_adev_init(void)
{
    int32_t i;

    memset(&__dma_req, 0, ADEV_MAX_DMAC_CHNL*sizeof(DMA_REQUEST));
    memset(&__buf_t,   0, ADEV_MAX_DMAC_CHNL*sizeof(buf_t));
#if I2S_BUF_STATIC
    memset(&__dma_lli, 0, ADEV_MAX_DMAC_CHNL*sizeof(DMA_LLI)*ADEV_LLI_NUM);
    memset(&__buf,     0, sizeof(__buf));
#endif
    memset(&__adev_req,0, sizeof(__adev_req));
    memset(&__adev_moni, 0, sizeof(__adev_moni));
    for(i=0; i<ADEV_MAX_DMAC_CHNL; i++)
        __adev_moni[i].intr_num = 1000;

    /* dac open is too slow */
    if(silan_codec_check(CODEC1) == 0) {
        silan_codec_dac_open(CODEC1);
    }

    //if(silan_codec_check(CODEC2) == 0){
    //    silan_codec_dac_open(CODEC2);
    //}
}


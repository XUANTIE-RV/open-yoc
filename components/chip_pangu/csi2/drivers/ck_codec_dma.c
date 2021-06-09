/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ck_codec_dma.c
 * @brief    CSI Source File for codec Driver
 * @version  V1.0
 * @date     25. September 2019
 * @vendor   csky
 * @name     csky-codec
 * @ip_id    0x111000010
 * @model    codec
 * @tag      DRV_CK_CODEC_TAG
 ******************************************************************************/

#include <drv/dma.h>
#include "ck_codec.h"
#include <drv/timer.h>
#include "soc.h"
#include "string.h"
#include <csi_config.h>

#define CONFIG_PER_DMAC0_CHANNEL_NUM  0//4
#define DMAC1_CFG(addr)  (*(volatile uint32_t *)((DW_DMAC1_BASE + (addr))))
#define DMA_REG_SARx            0x0
#define DMA_REG_DARx            0x8
#define DMA_REG_LLPX            0x10
#define DMA_REG_CTRLax          0x18
#define DMA_REG_CTRLbx          0x1c
#define DMA_REG_CFGax           0x40
#define DMA_REG_CFGbx           0x44

#define DMA_REG_RawTfr          0x2c0
#define DMA_REG_RawBlock        0x2c8
#define DMA_REG_RawSrcTran      0x2d0
#define DMA_REG_RawDstTran      0x2d8
#define DMA_REG_RawErr          0x2e0

#define DMA_REG_StatusTfr       0x2e8
#define DMA_REG_StatusBlock     0x2f0
#define DMA_REG_StatusSrcTran   0x2f8
#define DMA_REG_StatusDstTran   0x300
#define DMA_REG_StatusErr       0x308

#define DMA_REG_MaskTfr         0x310
#define DMA_REG_MaskBlock       0x318
#define DMA_REG_MaskSrcTran     0x320
#define DMA_REG_MaskDstTran     0x328
#define DMA_REG_MaskErr         0x330

#define DMA_REG_ClearTfr        0x338
#define DMA_REG_ClearBlock      0x340
#define DMA_REG_ClearSrcTran    0x348
#define DMA_REG_ClearDstTran    0x350
#define DMA_REG_ClearErr        0x358
#define DMA_REG_StatusInt       0x360

#define DMA_REG_ReqSrc          0x368
#define DMA_REG_ReqDst          0x370
#define DMA_REG_SglReqSrc       0x378
#define DMA_REG_SglReqDst       0x380
#define DMA_REG_LstReqSrc       0x388
#define DMA_REG_LstReqDst       0x390

#define DMA_REG_Cfg             0x398
#define DMA_REG_ChEn            0x3a0

extern ck_codec_ch_priv_t ck_codec_input_instance[];
extern ck_codec_ch_priv_t ck_codec_output_instance[];
extern csi_dev_t ck_codec_instance_v2[];
extern const uint16_t codec_tx_hs_num[];
extern const uint16_t codec_rx_hs_num[];
static uint32_t priv_adc_channel_en = 0;

typedef struct dw_lli_t {
    /* values that are not changed by hardware */
    uint32_t sar;
    uint32_t dar;
    uint32_t llp;    /* chain to next lli */
    uint32_t ctllo;
    /* values that may get written back: */
    uint32_t ctlhi;
    /* sstat and dstat can snapshot peripheral register state.
    * silicon config may discard either or both...
    */
    uint32_t sstat;
    uint32_t dstat;
} dw_lli_t;


typedef struct {
    uint8_t *dma_buf[2];
    uint32_t buf_size;
} dma_cache;

typedef struct {
    ringbuffer_t fifo;
    ck_codec_ch_priv_t *handle;
} priv_dma_input_t;

priv_dma_input_t priv_dma_input[8];

#define ADC_BUF_SIZE (1000 * 2)

static uint8_t g_adc_buf0[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf1[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf2[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf3[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf4[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf5[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf6[ADC_BUF_SIZE] __attribute__((aligned(16)));
static uint8_t g_adc_buf7[ADC_BUF_SIZE] __attribute__((aligned(16)));

static uint8_t *adc_cache_table[] = {
    g_adc_buf0, g_adc_buf1,
    g_adc_buf2, g_adc_buf3,
    g_adc_buf4, g_adc_buf5,
    g_adc_buf6, g_adc_buf7,
};

static int32_t ck_codec_input_cache_init(int idx)
{
    priv_dma_input_t *priv = &priv_dma_input[idx];

    priv->fifo.buffer = adc_cache_table[idx];
    priv->fifo.size = ADC_BUF_SIZE;
    priv->fifo.data_len = 0;
    priv->fifo.read = 0;
    priv->fifo.write = 0;
    priv->handle = &ck_codec_input_instance[idx];
    priv->handle->mv_size = 0;

    return 0;
}

int32_t ck_codec_input_dma_config(ck_codec_ch_priv_t *input_priv)
{
    csi_error_t ret = CSI_OK;
    input_priv->dma.parent = &ck_codec_instance_v2[input_priv->codec_idx];
    ret = csi_dma_ch_alloc(&input_priv->dma, -1, -1);
    if (ret < 0) {
        return -1;
    }

    csi_dma_ch_config_t  config;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw = DMA_DATA_WIDTH_32_BITS;
    config.dst_tw = DMA_DATA_WIDTH_32_BITS;
    config.group_len = 1U;
    config.trans_dir = DMA_PERH2MEM;
    config.handshake = codec_rx_hs_num[input_priv->idx];
    config.dst_reload_en = 1;
    config.src_reload_en = 0;

    csi_dma_ch_config(&input_priv->dma, &config);

    return ret;
}

static void ck_codec_input_cb_exec(int idx, int adc_event)
{
    if (ck_codec_input_instance[idx].cb != NULL && adc_event != -1) {
        ck_codec_input_instance[idx].cb(idx, adc_event, ck_codec_input_instance[idx].cb_arg);
    }
}

static csi_timer_t ad_timer_handle = {0};
uint32_t g_ad_timer_period = 8000;
#define AC_TIMER_IDX 0

static uint32_t ck_codec_input_dma_dar(int dma_ch)
{
#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif
    return DMAC1_CFG(DMA_REG_DARx + (dma_ch * 0x58));
}

static void ck_codec_input_cache_write_info_update(int idx)
{
    ringbuffer_t *fifo = &priv_dma_input[idx].fifo;
    uint32_t write_len = 0;

    uint32_t write_addr = (uint32_t)&fifo->buffer[fifo->write];
    uint32_t dma_dar_addr = ck_codec_input_dma_dar(priv_dma_input[idx].handle->dma.ch_id);

    if (dma_dar_addr > write_addr) {
        write_len = dma_dar_addr - write_addr;
    } else {
        uint32_t back_len = (uint32_t)&fifo->buffer[fifo->size] - write_addr;
        uint32_t rewind_len = dma_dar_addr - (uint32_t)fifo->buffer;
        write_len = back_len + rewind_len;
    }

    fifo->write = (fifo->write + write_len) % fifo->size;
    fifo->data_len += write_len;
}

static uint8_t adc_temp_buf[ADC_BUF_SIZE] __attribute__((aligned(16)));
void ck_codec_input_cache_read_info_update(int idx, int32_t *adc_event)
{

    priv_dma_input_t *priv = &priv_dma_input[idx];
    ringbuffer_t *cache_fifo = &priv->fifo;
    ringbuffer_t *input_ring_fifo = (ringbuffer_t *)&priv->handle->fifo;

    int out_len = ringbuffer_out(cache_fifo, adc_temp_buf, ADC_BUF_SIZE);
    int in_len = ringbuffer_in(input_ring_fifo, adc_temp_buf, out_len);

    priv->handle->mv_size = input_ring_fifo->data_len;

    if (priv->handle->mv_size >= priv->handle->period) {
        priv->handle->mv_size = 0;
        *adc_event = CODEC_EVENT_PERIOD_READ_COMPLETE;
    }

    if (in_len != out_len) {
        *adc_event = CODEC_EVENT_READ_BUFFER_FULL;
    }
}

static void ck_codec_input_timer_cb(csi_timer_t *timer_handle, void *arg)
{
    if (timer_handle->dev.idx != AC_TIMER_IDX) {
        return;
    }


    for (int i = 0; i < CK_CODEC_ADC_CHANNEL_NUM; i++) {
        int adc_event = -1;

        if ((1 << i) & priv_adc_channel_en) {
            ck_codec_input_cache_write_info_update(i);
            csi_dcache_clean_invalid(); ///<fix error
            ck_codec_input_cache_read_info_update(i, &adc_event);
            ck_codec_input_cb_exec(i, adc_event);
        }
    }

    ck_codec_gasket_error_irqhandler(0);
}

static int32_t ck_codec_input_timer_init()
{
    *((volatile uint32_t *)(0x39800008)) = 0;
    int32_t state;


    state = csi_timer_init(&ad_timer_handle, (uint32_t)0U);

    if (state < 0) {
        printf("csi_timer_init error\n");
    }

    state = csi_timer_attach_callback(&ad_timer_handle, ck_codec_input_timer_cb, NULL);

    if (state < 0) {
        printf("csi_timer_attach_callback error\n");
    }

    return state;
}

static int32_t ck_codec_input_enable_timer()
{
    if (ad_timer_handle.callback == NULL) {
        int ret = ck_codec_input_timer_init();

        if (ret != 0) {
            return -1;
        }
    }

    csi_timer_start(&ad_timer_handle, g_ad_timer_period);

    return 0;
}

int32_t ck_codec_dma_input_start(ck_codec_ch_priv_t *input_priv, ck_codec_reg_t *addr)
{
    int ret = -1;
    int dma_ch = input_priv->dma.ch_id;
    int ad_idx = input_priv->idx;

    ck_codec_input_cache_init(ad_idx);


    csi_dev_t *codec_priv = &ck_codec_instance_v2[0];
    ck_codec_path_adc_en(codec_priv, ad_idx, 1);

    ringbuffer_t *fifo = &priv_dma_input[ad_idx].fifo;

#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif
    DMAC1_CFG(DMA_REG_CTRLax + (dma_ch * 0x58)) = 0x200424;
    DMAC1_CFG(DMA_REG_SARx + (dma_ch * 0x58)) = (uint32_t)&addr->RX_FIFO[ad_idx];
    DMAC1_CFG(DMA_REG_DARx + (dma_ch * 0x58)) = (uint32_t)fifo->buffer;
    DMAC1_CFG(DMA_REG_CTRLbx + (dma_ch * 0x58)) = ADC_BUF_SIZE / 4;
    DMAC1_CFG(DMA_REG_ChEn) |= ((1 << dma_ch) << 8) | (1 << dma_ch);

    if (priv_adc_channel_en == 0) {
        ret = ck_codec_input_enable_timer();

        if (ret != 0) {
            return -1;
        }
    }

    priv_adc_channel_en |= 1 << ad_idx;

    return 0;
}

void ck_codec_input_dma_stop(ck_codec_ch_priv_t *input_priv)
{
    int idx = input_priv->idx;
    priv_adc_channel_en &= ~(1 << idx);
    csi_dma_ch_stop(&input_priv->dma);
    priv_dma_input[idx].handle->mv_size = 0;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[0];
    ck_codec_path_adc_en(codec_priv, idx, 0);

    if (priv_adc_channel_en == 0) {
        csi_timer_stop(&ad_timer_handle);
        csi_timer_detach_callback(&ad_timer_handle);
        csi_timer_uninit(&ad_timer_handle);
    }
}


#define DAC_DMA_LLP_MV_LEN 512
#define DAC_BUF_VALID_SIZE (DAC_DMA_LLP_MV_LEN * 4)
#define DMA_DAC_CACHE_SIZE (DAC_DMA_LLP_MV_LEN + DAC_BUF_VALID_SIZE) //320 is llp mv size, 2560/2 is buf used size

static uint8_t g_dac_buf0[DMA_DAC_CACHE_SIZE] __attribute__((aligned(0x1000)));
static uint8_t g_dac_buf1[DMA_DAC_CACHE_SIZE] __attribute__((aligned(0x1000)));

typedef struct {
    ck_codec_ch_priv_t *handle;
    dma_cache cache;
    dw_lli_t dma_llp0;
    dw_lli_t dma_llp1;
} priv_ck_codec_dma_ch_t;

static priv_ck_codec_dma_ch_t priv_dma_dac[CK_CODEC_DAC_CHANNEL_NUM];

static void ck_codec_set_output_dma_cache(int idx)
{
    dma_cache *cache = &priv_dma_dac[idx].cache;
    cache->dma_buf[0] = (uint8_t *)&g_dac_buf0[DAC_DMA_LLP_MV_LEN];
    cache->dma_buf[1] = (uint8_t *)&g_dac_buf1[DAC_DMA_LLP_MV_LEN];
    cache->buf_size = DAC_BUF_VALID_SIZE;
}

static void ck_codec_set_output(int idx)
{
    priv_dma_dac[idx].handle = &ck_codec_output_instance[idx];
}

static void ck_codec_set_output_llp(int idx, int dma_ch)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)ck_codec_instance_v2[0].reg_base;
    dma_cache *cache = &priv_dma_dac[idx].cache;

    dw_lli_t *llp0 = &priv_dma_dac[idx].dma_llp0;
    dw_lli_t *llp1 = &priv_dma_dac[idx].dma_llp1;

#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

    llp0->ctlhi = cache->buf_size; //no used
    llp0->ctllo = DMAC1_CFG(DMA_REG_CTRLax + (dma_ch * 0x58)) | 1;
    llp0->ctllo &= ~((7 << 11) | (7 << 14));
    llp0->dar = (uint32_t)&addr->PARA_TX_FIFO;
    llp0->sar = (uint32_t)cache->dma_buf[0];
    llp0->llp = (uint32_t)llp1;

    /*enable llp*/
    llp0->ctllo |= (1 << 28);

    *llp1 = *llp0;
    llp1->llp = (uint32_t)llp0;
    llp1->sar = (uint32_t)cache->dma_buf[1];
    csi_dcache_clean_invalid();

    DMAC1_CFG(DMA_REG_CTRLax + (dma_ch * 0x58)) |= (1 << 28);
    DMAC1_CFG(DMA_REG_LLPX + (dma_ch * 0x58)) = (uint32_t)llp0;
}

static void ck_codec_output_dma_handle_init(int idx, int dma_ch)
{
    priv_dma_dac[idx].handle = NULL;
    memset(&priv_dma_dac[idx].cache, 0, sizeof(dma_cache));
    memset(&priv_dma_dac[idx].dma_llp0, 0, sizeof(dw_lli_t));
    memset(&priv_dma_dac[idx].dma_llp1, 0, sizeof(dw_lli_t));

    ck_codec_set_output_dma_cache(idx);
    ck_codec_set_output(idx);
    ck_codec_set_output_llp(idx, dma_ch);
}

static void ck_codec_output_dma_cb(csi_dma_ch_t *dma_ch, csi_dma_event_t event, void *arg);
int32_t ck_codec_output_dma_config(ck_codec_ch_priv_t *output_priv)
{
    csi_error_t ret;
    output_priv->dma.parent = &ck_codec_instance_v2[output_priv->codec_idx];
    ret = csi_dma_ch_alloc(&output_priv->dma, -1, -1);

    if (ret == CSI_OK) {
        csi_dma_ch_attach_callback(&output_priv->dma, ck_codec_output_dma_cb, (void *)output_priv);

    } else {
        return  -1;
    }

    csi_dma_ch_config_t  config;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_tw = DMA_DATA_WIDTH_32_BITS;
    config.dst_tw = DMA_DATA_WIDTH_32_BITS;
    config.group_len = 4U;
    config.trans_dir = DMA_MEM2PERH;
    config.dst_reload_en = 1;
    config.src_reload_en = 0;
    config.handshake = codec_tx_hs_num[output_priv->idx];;

    csi_dma_ch_config(&output_priv->dma, &config);
    DMAC1_CFG(DMA_REG_CTRLax + (output_priv->dma.ch_id * 0x58)) = 0x10104924U;
    ck_codec_output_dma_handle_init(output_priv->idx, output_priv->dma.ch_id);

    return 0;
}

int32_t ck_codec_output_dma_start(int idx)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)ck_codec_instance_v2[0].reg_base;
    dma_cache *cache = &priv_dma_dac[idx].cache;
    ringbuffer_t *fifo = &priv_dma_dac[idx].handle->fifo;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[0];

    ringbuffer_out(fifo, cache->dma_buf[0], cache->buf_size);
    ringbuffer_out(fifo, cache->dma_buf[1], cache->buf_size);

    soc_dcache_clean_invalid_range((unsigned long)cache->dma_buf[0], cache->buf_size);
    soc_dcache_clean_invalid_range((unsigned long)cache->dma_buf[1], cache->buf_size);
    csi_dma_ch_start(&priv_dma_dac[idx].handle->dma, (void *)cache->dma_buf[0], (void *)&addr->PARA_TX_FIFO, cache->buf_size);
    ck_codec_path_dac_en(codec_priv, 1);
    return 0;
}

void ck_codec_output_dma_stop(int idx)
{
    dma_cache *cache = &priv_dma_dac[idx].cache;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[0];
    ck_codec_path_dac_en(codec_priv, 0);
    csi_dma_ch_stop(&priv_dma_dac[idx].handle->dma);
    ck_codec_tx_feed(codec_priv);
    memset(cache->dma_buf[0], 0, cache->buf_size);
    memset(cache->dma_buf[1], 0, cache->buf_size);
    priv_dma_dac[idx].handle->mv_size = 0;
}

void ck_codec_output_tranfer(ck_codec_ch_priv_t *output_priv)
{
    int32_t dac_event = -1;
    int idx = output_priv->idx;
    dma_cache *cache = &priv_dma_dac[idx].cache;

    priv_dma_dac[idx].handle->mv_size += cache->buf_size;

    if (priv_dma_dac[idx].handle->mv_size >= priv_dma_dac[idx].handle->period) {
        priv_dma_dac[idx].handle->mv_size = 0;
        dac_event = CODEC_EVENT_PERIOD_WRITE_COMPLETE;
    }

    ringbuffer_t *fifo = (ringbuffer_t *)&priv_dma_dac[idx].handle->fifo;
    uint8_t *out = NULL;
    int dma_ch = priv_dma_dac[idx].handle->dma.ch_id;

#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

    if (DMAC1_CFG(DMA_REG_SARx + (dma_ch * 0x58)) >= (uint32_t)cache->dma_buf[0] &&
        DMAC1_CFG(DMA_REG_SARx + (dma_ch * 0x58)) < (uint32_t)cache->dma_buf[0] + DAC_BUF_VALID_SIZE) {
        out = cache->dma_buf[1];
    } else  {
        out = cache->dma_buf[0];
    }

    int ret = ringbuffer_out(fifo, out, cache->buf_size);

    if (ret != cache->buf_size) {
        dac_event = CODEC_EVENT_WRITE_BUFFER_EMPTY;
        memset(&out[ret], 0, cache->buf_size - ret);
    }

    csi_dcache_clean_range((uint32_t *)out, cache->buf_size);

    if (priv_dma_dac[idx].handle->cb != NULL && dac_event != -1) {
        priv_dma_dac[idx].handle->cb(idx, dac_event, priv_dma_dac[idx].handle->cb_arg);
    }

}

static void ck_codec_output_dma_cb(csi_dma_ch_t *dma_ch, csi_dma_event_t event, void *arg)
{
    if (event == DMA_EVENT_TRANSFER_DONE) {
        ck_codec_ch_priv_t *output_priv = (ck_codec_ch_priv_t *)arg;

        if (output_priv->dma.ch_id != dma_ch->ch_id) {
            return;
        }

        ck_codec_output_tranfer(output_priv);
    }
}

static int ck_codec_dma_is_en(uint32_t reg_val, int ch)
{
    if (reg_val & (1 << ch)) {
        return 1;
    }

    return 0;
}

/*fix gasket bug*/
void ck_codec_dma_all_restart(void)
{
    int i = 0;
    int dma_ch = 0;
    ck_codec_ch_priv_t *in_priv = ck_codec_input_instance;
    ck_codec_ch_priv_t *out_priv = ck_codec_output_instance;
    csi_dev_t *codec_handle = &ck_codec_instance_v2[0];
    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_handle->reg_base;

    uint32_t dma_en = DMAC1_CFG(DMA_REG_ChEn);

    for (i = 0; i < CK_CODEC_ADC_CHANNEL_NUM; i++) {
        dma_ch = in_priv[i].dma.ch_id;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((in_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_input_dma_stop(&in_priv[i]);
        }
    }

    for (i = 0; i < CK_CODEC_DAC_CHANNEL_NUM; i++) {
        dma_ch = out_priv[i].dma.ch_id;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((out_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_path_dac_en(&ck_codec_instance_v2[0], 0);
            ck_codec_output_dma_stop(out_priv[i].idx);
        }
    }

    ck_codec_gasket_reset();

    ck_codec_inerrupt_disable_all(codec_handle);
    ck_codec_tx_feed(codec_handle);
    uint32_t mask = (1 << 16) | 0x1ff << 18;
    ck_codec_inerrupt_ctrl(codec_handle, mask, 1);
    ck_codec_tx_threshold_val(codec_handle, 4);
    ck_codec_rx_threshold_val(codec_handle, 4);

    for (i = 0; i < CK_CODEC_ADC_CHANNEL_NUM; i++) {
        dma_ch = in_priv[i].dma.ch_id;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((in_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_dma_input_start(&in_priv[i], addr);
        }
    }

    for (i = 0; i < CK_CODEC_DAC_CHANNEL_NUM; i++) {
        dma_ch = out_priv[i].dma.ch_id;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((out_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_output_dma_start(out_priv[i].idx);
            ck_codec_path_dac_en(&ck_codec_instance_v2[0], 1);
        }
    }
}


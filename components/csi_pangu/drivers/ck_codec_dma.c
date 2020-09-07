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

#include "dw_dmac.h"
#include "ck_codec.h"
#include "drv/dmac.h"
#include "drv/timer.h"
#include "soc.h"
#include "string.h"

#define DMAC1_CFG(addr)  (*(volatile uint32_t *)((CSKY_DMAC1_BASE + (addr))))

extern ck_codec_ch_priv_t ck_codec_input_instance[];
extern ck_codec_ch_priv_t ck_codec_output_instance[];
extern ck_codec_priv_v2_t ck_codec_instance_v2[];

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
    dev_ringbuf_t fifo;
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

/*return -1 error, else dma channel num*/
static int32_t ck_codec_input_dma_alloc()
{
    int dma_ch = -1;

#if defined(CONFIG_CHIP_PANGU)
    dma_ch = csi_dma_alloc_channel();
#else
    dma_ch = csi_dma_alloc_channel();
#endif

    return dma_ch;
}

static void ck_codec_dma_input_recv_cb(int32_t ch, dma_event_e event, void *arg);
int32_t ck_codec_input_dma_config(ck_codec_ch_priv_t *input_priv)
{
    if (input_priv->dma_ch == -1) {
        input_priv->dma_ch = ck_codec_input_dma_alloc();
    }

    if (input_priv->dma_ch == -1) {
        return -1;
    }

    dma_config_t config;
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = 4; //unit: 4 bytes
    config.dst_tw = 4; //unit: 4 bytes
    config.type   = DMA_PERH2MEM;
    config.dest_reload_en = 1;
    config.src_reload_en = 0;
    extern const uint32_t adc_hs_array[];

    config.hs_if = adc_hs_array[input_priv->idx];
    int ret = csi_dma_config_channel(input_priv->dma_ch, &config, ck_codec_dma_input_recv_cb, (void *)input_priv);

    if (ret != 0) {
        return -1;
    }

    return 0;
}

static void ck_codec_input_cb_exec(int idx, int adc_event)
{
    if (ck_codec_input_instance[idx].cb != NULL && adc_event != -1) {
        ck_codec_input_instance[idx].cb(idx, adc_event, ck_codec_input_instance[idx].cb_arg);
    }
}


/*
static void ck_codec_input_cache_write_info_update(int idx);
static void ck_codec_dma_adc_recv(int idx)
{
    int32_t adc_event =  -1;
    ck_codec_input_cache_write_info_update(idx);
    csi_dcache_clean_invalid(); //fix error
    ck_codec_input_cb_exec(idx, adc_event);
}
*/

static void ck_codec_dma_input_recv_cb(int32_t ch, dma_event_e event, void *arg)
{
    ck_codec_ch_priv_t *input_priv = (ck_codec_ch_priv_t *)arg;
    int dma_ch = input_priv->dma_ch;

    if (dma_ch != ch) {
        return;
    }

    if (event == DMA_EVENT_TRANSFER_MODE_DONE) {
        ;//ck_codec_dma_adc_recv(idx);
    }
}

static timer_handle_t ad_timer_handle = NULL;
#define AD_TIMER_PERIOD 8000 //US-30ms
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
    dev_ringbuf_t *fifo = &priv_dma_input[idx].fifo;
    uint32_t write_len = 0;

    uint32_t write_addr = (uint32_t)&fifo->buffer[fifo->write];
    uint32_t dma_dar_addr = ck_codec_input_dma_dar(priv_dma_input[idx].handle->dma_ch);

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
    dev_ringbuf_t *cache_fifo = &priv->fifo;
    dev_ringbuf_t *input_ring_fifo = (dev_ringbuf_t *)&priv->handle->fifo;

    int out_len = dev_ringbuf_out(cache_fifo, adc_temp_buf, ADC_BUF_SIZE);
    int in_len = dev_ringbuf_in(input_ring_fifo, adc_temp_buf, out_len);

    priv->handle->mv_size = input_ring_fifo->data_len;

    if (priv->handle->mv_size >= priv->handle->period) {
        priv->handle->mv_size = 0;
        *adc_event = CODEC_EVENT_PERIOD_READ_COMPLETE;
    }

    if (in_len != out_len) {
        *adc_event = CODEC_EVENT_READ_BUFFER_FULL;
    }
}

static void ck_codec_input_timer_cb(int32_t idx, timer_event_e event)
{
    if (idx != AC_TIMER_IDX) {
        return;
    }

    for (int i = 0; i < CK_CODEC_ADC_CHANNEL_NUM; i++) {
        int adc_event = -1;
        if ((1 << i) & priv_adc_channel_en) {
            ck_codec_input_cache_write_info_update(i);
            csi_dcache_clean_invalid(); //fix error
            ck_codec_input_cache_read_info_update(i, &adc_event);
            ck_codec_input_cb_exec(i, adc_event);
        }
    }
}

static int32_t ck_codec_input_timer_init()
{
    *((volatile uint32_t *)(0x39800008)) = 0;
    ad_timer_handle = csi_timer_initialize(AC_TIMER_IDX, ck_codec_input_timer_cb);

    if (ad_timer_handle == NULL) {
        return -1;
    }

    int32_t ret = csi_timer_config(ad_timer_handle, TIMER_MODE_RELOAD);
    ret += csi_timer_set_timeout(ad_timer_handle, AD_TIMER_PERIOD);

    if (ret != 0) {
        return -1;
    }

    return 0;
}

static int32_t ck_codec_input_enable_timer()
{
    if (ad_timer_handle == NULL) {
        int ret = ck_codec_input_timer_init();

        if (ret != 0) {
            return -1;
        }
    }

    csi_timer_start(ad_timer_handle);

    return 0;
}

int32_t ck_codec_dma_input_start(ck_codec_ch_priv_t *input_priv, ck_codec_reg_t *addr)
{
    int ret = -1;
    int dma_ch = input_priv->dma_ch;
    int ad_idx = input_priv->idx;

    ck_codec_input_cache_init(ad_idx);

    if (dev_ringbuf_avail(&priv_dma_input[ad_idx].handle->fifo) == 0) {
        return -1;
    }

    ck_codec_priv_v2_t *codec_priv = &ck_codec_instance_v2[0];
    ck_codec_path_adc_en(codec_priv, ad_idx, 1);

    dev_ringbuf_t *fifo = &priv_dma_input[ad_idx].fifo;
    //csi_dma_start(dma_ch, (void *)&addr->RX_FIFO[ad_idx], fifo->buffer, 2560/4);

#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif
    DMAC1_CFG(DMA_REG_CTRLax + (dma_ch * 0x58)) = 0x204C24;
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
    int dma_ch = input_priv->dma_ch;
    csi_dma_stop(dma_ch);
    priv_dma_input[idx].handle->mv_size = 0;

    ck_codec_priv_v2_t *codec_priv = &ck_codec_instance_v2[0];
    ck_codec_path_adc_en(codec_priv, idx, 0);

    if (priv_adc_channel_en == 0) {
        csi_timer_stop(ad_timer_handle);
        csi_timer_uninitialize(ad_timer_handle);
        ad_timer_handle = NULL;
    }
}

void ck_codec_dma_ch_release(int ch)
{
    if (ch == -1) {
        return;
    }

#if defined(CONFIG_CHIP_PANGU)
    csi_dma_release_channel(ch);
#else
    csi_dma_release_channel(ch);
#endif

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
    ck_codec_reg_t *addr = (ck_codec_reg_t *)ck_codec_instance_v2[0].base;
    dma_cache *cache = &priv_dma_dac[idx].cache;

    dw_lli_t *llp0 = &priv_dma_dac[idx].dma_llp0;
    dw_lli_t *llp1 = &priv_dma_dac[idx].dma_llp1;

#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

    llp0->ctlhi = cache->buf_size; //no used
    llp0->ctllo = DMAC1_CFG(DMA_REG_CTRLax + (dma_ch * 0x58)) | 1;
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
    memset(&priv_dma_dac[idx], 0, sizeof(priv_dma_dac[idx]));
    ck_codec_set_output_dma_cache(idx);
    ck_codec_set_output(idx);
    ck_codec_set_output_llp(idx, dma_ch);
}

/*return -1 error, else dma channel num*/
static int32_t ck_codec_output_dma_alloc()
{
    int dma_ch = -1;
#if defined(CONFIG_CHIP_PANGU)
    dma_ch = csi_dma_alloc_channel();
#else
    dma_ch = csi_dma_alloc_channel();
#endif

    if (dma_ch == -1) {
        return -1;
    }

    return dma_ch;
}

static void ck_codec_output_dma_cb(int32_t ch, dma_event_e event, void *arg);
int32_t ck_codec_output_dma_config(ck_codec_ch_priv_t *output_priv)
{
    if (output_priv->dma_ch == -1) {
        output_priv->dma_ch = ck_codec_output_dma_alloc();
    }

    if (output_priv->dma_ch == -1) {
        return -1;
    }

    dma_config_t config;
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = 4; //unit: 4 bytes
    config.dst_tw = 4; //unit: 4 bytes
    config.type   = DMA_MEM2PERH;
    config.hs_if = DWENUM_DMA_CODEC_DAC;
    config.dest_reload_en = 0;
    config.src_reload_en = 0;

    int ret = csi_dma_config_channel(output_priv->dma_ch, &config, ck_codec_output_dma_cb, (void *)output_priv);

    if (ret != 0) {
        return -1;
    }

    ck_codec_output_dma_handle_init(output_priv->idx, output_priv->dma_ch);

    return 0;
}

int32_t ck_codec_output_dma_start(int idx)
{
    int dma_ch = priv_dma_dac[idx].handle->dma_ch;
    ck_codec_reg_t *addr = (ck_codec_reg_t *)ck_codec_instance_v2[0].base;
    dma_cache *cache = &priv_dma_dac[idx].cache;
    dev_ringbuf_t *fifo = &priv_dma_dac[idx].handle->fifo;

    dev_ringbuf_out(fifo, cache->dma_buf[0], cache->buf_size);
    dev_ringbuf_out(fifo, cache->dma_buf[1], cache->buf_size);

    csi_dcache_clean_range((uint32_t *)cache->dma_buf[0], cache->buf_size);
    csi_dcache_clean_range((uint32_t *)cache->dma_buf[1], cache->buf_size);
    csi_dma_start(dma_ch, (void *)cache->dma_buf[0], (void *)&addr->PARA_TX_FIFO, cache->buf_size / 4);

    return 0;
}

void ck_codec_output_dma_stop(int idx)
{
    int dma_ch = priv_dma_dac[idx].handle->dma_ch;
    dma_cache *cache = &priv_dma_dac[idx].cache;

    csi_dma_stop(dma_ch);
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

    dev_ringbuf_t *fifo = (dev_ringbuf_t *)&priv_dma_dac[idx].handle->fifo;
    uint8_t *out = NULL;
    int dma_ch = priv_dma_dac[idx].handle->dma_ch;

#if defined(CONFIG_CHIP_PANGU)
    dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

    if (DMAC1_CFG(DMA_REG_SARx + (dma_ch * 0x58)) >= (uint32_t)cache->dma_buf[0] &&
        DMAC1_CFG(DMA_REG_SARx + (dma_ch * 0x58)) < (uint32_t)cache->dma_buf[0] + DAC_BUF_VALID_SIZE) {
        out = cache->dma_buf[1];
    } else  {
        out = cache->dma_buf[0];
    }

    int ret = dev_ringbuf_out(fifo, out, cache->buf_size);

    if (ret != cache->buf_size) {
        dac_event = CODEC_EVENT_WRITE_BUFFER_EMPTY;
        memset(&out[ret], 0, cache->buf_size - ret);
    }

    csi_dcache_clean_range((uint32_t *)out, cache->buf_size);

    if (priv_dma_dac[idx].handle->cb != NULL && dac_event != -1) {
        priv_dma_dac[idx].handle->cb(idx, dac_event, priv_dma_dac[idx].handle->cb_arg);
    }

}

static void ck_codec_output_dma_cb(int32_t ch, dma_event_e event, void *arg)
{
    if (event == DMA_EVENT_TRANSFER_MODE_DONE) {
        ck_codec_ch_priv_t *output_priv = (ck_codec_ch_priv_t *)arg;

        if (output_priv->dma_ch != ch) {
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
void ck_codec_dma_all_restart()
{
    int i = 0;
    int dma_ch = 0;
    ck_codec_ch_priv_t *in_priv = ck_codec_input_instance;
    ck_codec_ch_priv_t *out_priv = ck_codec_output_instance;
    ck_codec_priv_v2_t *codec_handle = &ck_codec_instance_v2[0];
    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_handle->base;

    uint32_t dma_en = DMAC1_CFG(DMA_REG_ChEn);

    for (i = 0; i < CK_CODEC_ADC_CHANNEL_NUM; i++) {
        dma_ch = in_priv[i].dma_ch;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((in_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_input_dma_stop(&in_priv[i]);
        }
    }

    for (i = 0; i < CK_CODEC_DAC_CHANNEL_NUM; i++) {
        dma_ch = out_priv[i].dma_ch;
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
    uint32_t mask = 0xff | 0x3ff << 18;
    ck_codec_inerrupt_ctrl(codec_handle, mask, 1);
    ck_codec_tx_threshold_val(codec_handle, 4);
    ck_codec_rx_threshold_val(codec_handle, 4);

    for (i = 0; i < CK_CODEC_ADC_CHANNEL_NUM; i++) {
        dma_ch = in_priv[i].dma_ch;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((in_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_dma_input_start(&in_priv[i], addr);
        }
    }

    for (i = 0; i < CK_CODEC_DAC_CHANNEL_NUM; i++) {
        dma_ch = out_priv[i].dma_ch;
#if defined(CONFIG_CHIP_PANGU)
        dma_ch -= CONFIG_PER_DMAC0_CHANNEL_NUM;
#endif

        if ((out_priv[i].priv_sta & CODEC_INIT_MASK) && ck_codec_dma_is_en(dma_en, dma_ch)) {
            ck_codec_path_dac_en(&ck_codec_instance_v2[0], 1);
            ck_codec_output_dma_start(out_priv[i].idx);
        }
    }
}


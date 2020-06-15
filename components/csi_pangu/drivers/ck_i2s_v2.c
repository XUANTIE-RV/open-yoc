/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_i2s_v2.c
 * @brief    CSI Source File for I2S Driver
 * @version  V1.2
 * @date     8.15. August 2019
 * @vendor   csky
 * @name     csky-i2s
 * @ip_id    0x111000010
 * @model    i2s
 * @tag      DRV_CK_I2S_TAG
******************************************************************************/

#include <string.h>
#include <drv/irq.h>
#include <drv/i2s.h>
#include <ck_i2s_v2.h>

extern int32_t target_i2s_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);
extern int32_t target_get_addr_space(uint32_t addr);
ck_i2s_priv_v2_t i2s_instance[CONFIG_I2S_NUM];

static void ck_i2s_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISEN;
    val &= ~I2S_MODULE_ENABLE_MASK;

    if (en) {
        val |= I2S_MODULE_ENABLE_MASK;
    }

    addr->I2S_IISEN = val;
}

/*It is impossible to write to this register when I2S is enabled.*/
static void ck_i2s_tx_mode_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_FUNCMODE;

    val |= I2S_TX_MODE_WRITE_EN_MASK;
    val &= ~I2S_TX_MODE_EN_MASK;

    if (en) {
        val |= I2S_TX_MODE_EN_MASK;
    }

    addr->I2S_FUNCMODE = val;
}

/*It is impossible to write to this register when I2S is enabled.*/
static void ck_i2s_rx_mode_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_FUNCMODE;

    val |= I2S_RX_MODE_WRITE_EN_MASK;
    val &= ~I2S_RX_MODE_EN_MASK;

    if (en) {
        val |= I2S_RX_MODE_EN_MASK;
    }

    addr->I2S_FUNCMODE = val;
}

/*It is impossible to write to this register when I2S is enabled.*/
static void ck_i2s_full_duplex_mode_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = 0xffff & ~(0x01 | 0x01 << 4);

    if (en) {
        val = 0xffff;
    }

    addr->I2S_FUNCMODE = val;
}

/*It is impossible to write to this register when I2S is enabled.
  I2S receiver s_sclk and s_ws delay level*/
static void ck_i2s_receive_delay(ck_i2s_priv_v2_t *priv, uint8_t delay)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;
    delay &= 0x03;

    val &= ~I2S_RECEIVE_DELAY_MASK;
    val |= delay << 12;

    addr->I2S_IISCNF_IN = val;
}

static void ck_i2s_slave_rx_sclk_src_select(ck_i2s_priv_v2_t *priv, i2s_sclk_clock_src_e src)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;

    val &= ~I2S_RX_CLK_SELECT_MASK;

    if (src == I2S_SCLK_SRC_MCLK_I) {
        val |= I2S_RX_CLK_SELECT_MASK;
    }

    addr->I2S_IISCNF_IN = val;
}

/*0:slave, 1 master*/
static void ck_i2s_rx_mode(ck_i2s_priv_v2_t *priv, int master_en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;

    val &= ~I2S_RX_MODE_MASK;

    if (master_en) {
        val |= I2S_RX_MODE_MASK;
    }

    addr->I2S_IISCNF_IN = val;
}

/*0:right 1 left*/
static void ck_i2s_rx_mono_channel_select(ck_i2s_priv_v2_t *priv, int left_en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;

    val &= ~I2S_RX_MONO_MODE_CHANNEL_SEL_MASK;

    if (left_en) {
        val |= I2S_RX_MONO_MODE_CHANNEL_SEL_MASK;
    }

    addr->I2S_IISCNF_IN = val;
}

static void ck_i2s_rx_mono_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;

    val &= ~I2S_RX_MONO_MODE_ENABLE_MASK;

    if (en) {
        val |= I2S_RX_MONO_MODE_ENABLE_MASK;
    }

    addr->I2S_IISCNF_IN = val;
}

static void ck_i2s_rx_left_channel_polarity(ck_i2s_priv_v2_t *priv, int left_ch_high)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;

    val &= ~I2S_RX_LEFT_CHANNEL_POLARITY_MASK;

    if (left_ch_high) {
        val |= I2S_RX_LEFT_CHANNEL_POLARITY_MASK;
    }

    addr->I2S_IISCNF_IN = val;
}

static void ck_i2s_rx_standard(ck_i2s_priv_v2_t *priv, i2s_protocol_e standard)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_IN;

    val &= ~I2S_RX_STANDARD_MASK;
    val |= standard;

    addr->I2S_IISCNF_IN = val;
}

static void ck_i2s_mclk_freq(ck_i2s_priv_v2_t *priv, i2s_mclk_freq_e freq)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_FSSTA;

    val &= ~I2S_MCLK_FREQ_MASK;

    if (freq == I2S_MCLK_384FS) {
        val |= I2S_MCLK_FREQ_MASK;
    }

    addr->I2S_FSSTA = val;
}

static void ck_i2s_sclk_freq(ck_i2s_priv_v2_t *priv, i2s_sclk_freq_e freq)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_FSSTA;

    val &= ~I2S_MCLK_FREQ_MASK;
    val |= freq << 12;

    addr->I2S_FSSTA = val;
}

static void ck_i2s_data_width_mode(ck_i2s_priv_v2_t *priv, uint8_t mode)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_FSSTA;

    val &= ~I2S_DATA_WIDTH_MODE_MASK;
    mode &= 0x0f;
    val |= mode << 8;

    addr->I2S_FSSTA = val;
}

static void ck_i2s_slave_tx_sclk_src_select(ck_i2s_priv_v2_t *priv, i2s_sclk_clock_src_e src)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_OUT;
    val &= ~I2S_TX_SCLK_SELECT_MASK;

    if (src == I2S_SCLK_SRC_MCLK_I) {
        val |= I2S_TX_SCLK_SELECT_MASK;
    }

    addr->I2S_IISCNF_OUT = val;
}

static void ck_i2s_tx_mode(ck_i2s_priv_v2_t *priv, int32_t master_mode)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_OUT;
    val &= ~I2S_TX_MODE_MASK;

    if (!master_mode) {
        val |= I2S_TX_MODE_MASK;
    }

    addr->I2S_IISCNF_OUT = val;
}

static void ck_i2s_tx_mono_enable(ck_i2s_priv_v2_t *priv, int32_t en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_OUT;
    val &= ~I2S_TX_MONO_MODE_MASK;

    if (en) {
        val |= I2S_TX_MONO_MODE_MASK;
    }

    addr->I2S_IISCNF_OUT = val;
}

static void ck_i2s_tx_left_channel_polarity(ck_i2s_priv_v2_t *priv, int left_ch_high)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_OUT;

    val &= ~I2S_TX_LEFT_CHANNEL_POLARITY_MASK;

    if (left_ch_high) {
        val |= I2S_TX_LEFT_CHANNEL_POLARITY_MASK;
    }

    addr->I2S_IISCNF_OUT = val;
}

static void ck_i2s_tx_standard(ck_i2s_priv_v2_t *priv, i2s_protocol_e standard)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IISCNF_OUT;

    val &= ~I2S_TX_STANDARD_MASK;
    val |= standard;

    addr->I2S_IISCNF_OUT = val;
}
/*FADTLR Automatic computed sample rate*/
/*TO DO To achieve it*/

/*SCCR Data compression*/
/*TO DO To achieve it*/

/*set transmit FIFO threshold*/
static void ck_i2s_tx_fifo_threshold(ck_i2s_priv_v2_t *priv, uint8_t data)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_TXFTLR;

    val &= ~I2S_TX_FIFO_THRESHOLD_MASK;
    val |= data;

    addr->I2S_TXFTLR = val;
}

/*set receive FIFO threshold*/
static void ck_i2s_rx_fifo_threshold(ck_i2s_priv_v2_t *priv, uint8_t data)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_RXFTLR;

    val &= ~I2S_RX_FIFO_THRESHOLD_MASK;
    val |= data;

    addr->I2S_RXFTLR = val;
}

static void ck_i2s_interrupt_ctrl(ck_i2s_priv_v2_t *priv, uint32_t irq_mask, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_IMR;

    val &= ~irq_mask;

    if (en) {
        val |= irq_mask;
    }

    addr->I2S_IMR = val;
}

static void ck_i2s_interrupt_imk_clear(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_IMR = 0;
}

static void ck_i2s_div0(ck_i2s_priv_v2_t *priv, uint32_t div)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_DIV0_LEVEL = div;
}

static void ck_i2s_div3(ck_i2s_priv_v2_t *priv, uint32_t div)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_DIV3_LEVEL = div;
}

/*return data see i2s status code  */
static uint32_t ck_i2s_status(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    return addr->I2S_SR;
}

static void ck_i2s_tx_mode_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    i2s_mode_e mode = config->cfg.mode;
    int master_en = 0;

    if (mode == I2S_MODE_TX_MASTER || mode == I2S_MODE_FULL_DUPLEX_MASTER) {
        master_en = 1;
    } else if (mode == I2S_MODE_TX_SLAVE || mode == I2S_MODE_FULL_DUPLEX_SLAVE) {
        master_en = 0;
    }

    ck_i2s_tx_mode(priv, master_en);
    ck_i2s_tx_mono_enable(priv, config->cfg.tx_mono_enable);
}

static void ck_i2s_rx_mode_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    i2s_mode_e mode = config->cfg.mode;
    int master_en = 0;

    if (mode == I2S_MODE_RX_MASTER || mode == I2S_MODE_FULL_DUPLEX_MASTER) {
        master_en = 1;
    } else if (mode == I2S_MODE_RX_SLAVE || mode == I2S_MODE_FULL_DUPLEX_SLAVE) {
        master_en = 0;
    }

    ck_i2s_rx_mode(priv, master_en);
    ck_i2s_receive_delay(priv, 0);
    ck_i2s_rx_mono_enable(priv, config->cfg.rx_mono_enable);

    if (config->cfg.rx_mono_enable) {
        ck_i2s_rx_mono_channel_select(priv, config->cfg.rx_mono_select_ch);
    }
}

static int32_t ck_i2s_mode_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    i2s_mode_e mode = config->cfg.mode;

    if (mode == I2S_MODE_TX_MASTER || mode == I2S_MODE_TX_SLAVE) {
        ck_i2s_tx_mode_config(priv, config);
        ck_i2s_tx_mode_enable(priv, 1);
        priv->tx_en = 1;
        priv->rx_en = 0;
    } else if (mode == I2S_MODE_RX_MASTER || mode == I2S_MODE_RX_SLAVE) {
        ck_i2s_rx_mode_config(priv, config);
        ck_i2s_rx_mode_enable(priv, 1);
        priv->rx_en = 1;
        priv->tx_en = 0;
    } else if (mode == I2S_MODE_FULL_DUPLEX_MASTER || mode == I2S_MODE_FULL_DUPLEX_SLAVE) {
        ck_i2s_tx_mode_config(priv, config);
        ck_i2s_rx_mode_config(priv, config);
        ck_i2s_full_duplex_mode_enable(priv, 1);
        priv->tx_en = 1;
        priv->rx_en = 1;
    }

    return 0;
}

static void ck_i2s_standard_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    if (config->cfg.mode == I2S_MODE_TX_MASTER || config->cfg.mode == I2S_MODE_TX_SLAVE) {
        ck_i2s_tx_standard(priv, config->cfg.protocol);
    } else {
        ck_i2s_rx_standard(priv, config->cfg.protocol);
    }
}

static int32_t ck_i2s_data_width_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    uint8_t bit_mode = 0;
    i2s_sample_width_e sample_width = config->cfg.width;

    if (sample_width == I2S_SAMPLE_16BIT) {
        bit_mode = I2S_B16_B16;
    } else if (sample_width == I2S_SAMPLE_24BIT) {
        bit_mode = I2S_B24_B24;
    } else if (sample_width == I2S_SAMPLE_32BIT) {
        bit_mode = I2S_B32_B32;
    } else {
        return -1;
    }

    ck_i2s_data_width_mode(priv, bit_mode);
    return 0;
}

static void ck_i2s_sclk_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    if (config->cfg.mode == I2S_MODE_TX_MASTER || config->cfg.mode == I2S_MODE_TX_SLAVE) {
        ck_i2s_slave_tx_sclk_src_select(priv, I2S_SCLK_SRCCLK);
    } else {
        ck_i2s_slave_rx_sclk_src_select(priv, I2S_SCLK_SRCCLK);
    }

    ck_i2s_sclk_freq(priv, config->cfg.sclk_freq);
}

static int32_t ck_i2s_sample_rate_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    uint32_t sample_rate = config->rate;
    uint32_t div0_coefficient = 0;
    uint32_t div0 = 0;

    if (config->cfg.mclk_freq != I2S_MCLK_256FS && config->cfg.mclk_freq != I2S_MCLK_384FS) {
        return -1;
    }

    div0_coefficient = I2S_SRC_CLK_FREQ / config->cfg.mclk_freq;
    div0 = (div0_coefficient + div0_coefficient % sample_rate) / sample_rate;

    if (div0 > 255) {
        return -1;
    }

    ck_i2s_div0(priv, div0);
    ck_i2s_div3(priv, 0);

    return 0;
}

static void ck_i2s_ws_polarity_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    int polarity = 0;

    if (config->cfg.left_polarity == I2S_LEFT_POLARITY_HITH) {
        polarity = 1;
    }

    if (config->cfg.mode == I2S_MODE_TX_MASTER || config->cfg.mode == I2S_MODE_TX_SLAVE) {
        ck_i2s_tx_left_channel_polarity(priv, polarity);
    } else {
        ck_i2s_rx_left_channel_polarity(priv, polarity);
    }
}

static void ck_i2s_ringbuf_config(ck_i2s_priv_v2_t *priv, i2s_config_t *config)
{
    priv->tx_period = config->tx_period;
    priv->rx_period = config->rx_period;

    if (config->cfg.mode == I2S_MODE_TX_MASTER || config->cfg.mode == I2S_MODE_TX_SLAVE) {
        if (priv->tx_ring_buf.fifo.buffer == config->tx_buf) {
            return;
        }

        priv->tx_ring_buf.fifo.buffer = config->tx_buf;
        priv->tx_ring_buf.fifo.size = config->tx_buf_length;
        priv->tx_ring_buf.mv_block_size = priv->tx_period;
        ck_i2s_tx_fifo_threshold(priv, 16);
    } else if (config->cfg.mode == I2S_MODE_RX_MASTER || config->cfg.mode == I2S_MODE_RX_SLAVE) {
        if (priv->rx_ring_buf.fifo.buffer == config->rx_buf) {
            return;
        }

        priv->rx_ring_buf.fifo.buffer = config->rx_buf;
        priv->rx_ring_buf.fifo.size = config->rx_buf_length;
        priv->rx_ring_buf.mv_block_size = priv->rx_period;
        ck_i2s_rx_fifo_threshold(priv, 16);
    } else {
        /*full duplex mode*/
        if (priv->tx_ring_buf.fifo.buffer != config->tx_buf) {
            priv->tx_ring_buf.fifo.buffer = config->tx_buf;
            priv->tx_ring_buf.fifo.size = config->tx_buf_length;
            priv->tx_ring_buf.mv_block_size = priv->tx_period;
            ck_i2s_tx_fifo_threshold(priv, 16);
        }

        if (priv->rx_ring_buf.fifo.buffer != config->rx_buf) {
            priv->rx_ring_buf.fifo.buffer = config->rx_buf;
            priv->rx_ring_buf.fifo.size = config->rx_buf_length;
            priv->rx_ring_buf.mv_block_size = priv->rx_period;
            ck_i2s_rx_fifo_threshold(priv, 16);
        }
    }
}

static uint32_t ck_i2s_module_is_enable(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;

    return addr->I2S_IISEN;
}

static int32_t ck_i2s_cfg_record_compare(i2s_config_type_t *record, i2s_config_t *config)
{
    return memcmp(record, &config->cfg, sizeof(i2s_config_type_t));
}

i2s_handle_t csi_i2s_initialize(int32_t idx, i2s_event_cb_t cb_event, void *cb_arg)
{
    if (idx >= CONFIG_I2S_NUM) {
        return NULL;
    }

    ck_i2s_priv_v2_t *handle = &i2s_instance[idx];

    int32_t ret = target_i2s_init(idx, &handle->base, &handle->irq_num, &handle->irq_handle);

    if (ret != idx) {
        return NULL;
    }

    handle->idx = idx;
    handle->cb = cb_event;
    handle->cb_arg = cb_arg;
    handle->priv_sta = I2S_READY;
    handle->tx_dma_ch = -1;
    handle->rx_dma_ch = -1;

    uint32_t irq_mask = I2S_WRONG_ADDRS_ERROR_IRQ_MASK;

    ck_i2s_interrupt_imk_clear(handle);
    ck_i2s_interrupt_ctrl(handle, irq_mask, 1);
    drv_irq_register(handle->irq_num, handle->irq_handle);
    drv_irq_enable(handle->irq_num);

    return handle;
}

int32_t csi_i2s_uninitialize(i2s_handle_t handle)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;


    csi_i2s_enable(handle, 0);
    drv_irq_unregister(priv->irq_num);
    drv_irq_disable(priv->irq_num);

    priv_stream_obj.stream_uninit(priv);
    memset(handle, 0, sizeof(ck_i2s_priv_v2_t));
    priv->rx_dma_ch = -1;
    priv->tx_dma_ch = -1;
    return 0;
}

int32_t csi_i2s_config(i2s_handle_t handle, i2s_config_t *config)
{
    I2S_NULL_PARAM_CHK(handle);
    int ret = 0;
    int cmpare_ret = 0;
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    if (config->cfg.mode == I2S_MODE_FULL_DUPLEX_MASTER
        || config->cfg.mode == I2S_MODE_FULL_DUPLEX_SLAVE) {
        if (priv->idx != 0) {
            return ERR_I2S(DRV_ERROR_UNSUPPORTED);
        }
    }

    if (config->cfg.protocol > I2S_PROTOCOL_PCM || config->cfg.protocol < I2S_PROTOCOL_I2S) {
        return ERR_I2S(I2S_ERROR_PROTOCOL);
    }

    if (config->cfg.mode > I2S_MODE_FULL_DUPLEX_SLAVE || config->cfg.mode < I2S_MODE_TX_MASTER) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    cmpare_ret = ck_i2s_cfg_record_compare(&priv->record_cfg, config);
    priv->record_cfg = config->cfg;

    if (cmpare_ret != 0) {
        uint32_t i2s_module_already_en = ck_i2s_module_is_enable(priv);
        ck_i2s_enable(priv, 0);
        ret += ck_i2s_mode_config(priv, config);
        ck_i2s_standard_config(priv, config);
        ret += ck_i2s_data_width_config(priv, config);
        ck_i2s_sclk_config(priv, config);
        ck_i2s_mclk_freq(priv, config->cfg.mclk_freq);
        ck_i2s_ws_polarity_config(priv, config);
        ck_i2s_enable(priv, i2s_module_already_en);
    }

    ret += ck_i2s_sample_rate_config(priv, config);
    ck_i2s_ringbuf_config(priv, config);

    if (ret != 0) {
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    return 0;
}

uint32_t csi_i2s_send(i2s_handle_t handle, const uint8_t *data, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);

    uint32_t len = priv_stream_obj.stream_send(handle, data, length);

    return len;
}

uint32_t csi_i2s_receive(i2s_handle_t handle, uint8_t *buf, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    uint32_t len = priv_stream_obj.stream_recv(handle, buf, length);

    return len;
}

static void ck_i2s_tx_pause(ck_i2s_priv_v2_t *priv)
{
    priv->priv_sta |= I2S_TX_PAUSE;
}

static void ck_i2s_tx_resume(ck_i2s_priv_v2_t *priv)
{
    priv->priv_sta &= ~I2S_TX_PAUSE;
    priv_stream_obj.stream_tx_start(priv);
}

static void ck_i2s_tx_stop(ck_i2s_priv_v2_t *priv)
{
    priv->priv_sta &= ~I2S_TX_RUNING;
    priv_stream_obj.stream_tx_stop(priv);
}

static void ck_i2s_tx_start(ck_i2s_priv_v2_t *priv)
{
    if (priv->priv_sta & I2S_TX_RUNING) {
        return;
    }

    priv->priv_sta |= I2S_TX_RUNING;
    priv_stream_obj.stream_tx_start(priv);
}

int32_t csi_i2s_send_ctrl(i2s_handle_t handle, i2s_ctrl_e cmd)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    switch (cmd) {
        case I2S_STREAM_PAUSE:
            ck_i2s_tx_pause(priv);
            break;

        case I2S_STREAM_RESUME:
            ck_i2s_tx_resume(priv);
            break;

        case I2S_STREAM_STOP:
            ck_i2s_tx_stop(priv);
            break;

        case I2S_STREAM_START:
            ck_i2s_tx_start(priv);
            break;

        default:
            return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    return 0;
}

static void ck_i2s_rx_pause(ck_i2s_priv_v2_t *priv)
{
    priv->priv_sta |= I2S_RX_PAUSE;
}

static void ck_i2s_rx_resume(ck_i2s_priv_v2_t *priv)
{
    priv->priv_sta &= ~I2S_RX_PAUSE;
    priv_stream_obj.stream_rx_start(priv);
}

static void ck_i2s_rx_stop(ck_i2s_priv_v2_t *priv)
{
    priv->priv_sta &= ~I2S_RX_RUNING;

    priv_stream_obj.stream_rx_stop(priv);
}

static void ck_i2s_rx_start(ck_i2s_priv_v2_t *priv)
{
    if (priv->priv_sta & I2S_RX_RUNING) {
        return;
    }

    priv->priv_sta |= I2S_RX_RUNING;
    priv_stream_obj.stream_rx_start(priv);
}

int32_t csi_i2s_receive_ctrl(i2s_handle_t handle, i2s_ctrl_e cmd)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    switch (cmd) {
        case I2S_STREAM_PAUSE:
            ck_i2s_rx_pause(priv);
            break;

        case I2S_STREAM_RESUME:
            ck_i2s_rx_resume(priv);
            break;

        case I2S_STREAM_STOP:
            ck_i2s_rx_stop(priv);
            break;

        case I2S_STREAM_START:
            ck_i2s_rx_start(priv);
            break;

        default:
            return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    return 0;
}

void csi_i2s_enable(i2s_handle_t handle, int en)
{
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;
    ck_i2s_enable(priv, en);

    if (0 == en) {
        ck_i2s_interrupt_imk_clear(priv);
    }
}

void ck_i2s_irqhandler(int32_t idx)
{
    priv_stream_obj.stream_irq_handle(idx);
}

i2s_status_t csi_i2s_get_status(i2s_handle_t handle)
{
    i2s_status_t i2s_status;
    memset(&i2s_status, 0, sizeof(i2s_status));
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    if (priv->priv_sta & I2S_TX_RUNING) {
        i2s_status.tx_runing = 1;
    }

    if (priv->priv_sta & I2S_RX_RUNING) {
        i2s_status.rx_runing = 1;
    }

    uint32_t sta = ck_i2s_status(priv);

    if (sta & I2S_RX_FIFO_FULL) {
        i2s_status.rx_fifo_full = 1;
    }

    if (sta & I2S_TX_FIFO_EMPTY) {
        i2s_status.tx_fifo_empty = 1;
    }

    return i2s_status;
}

i2s_capabilities_t csi_i2s_get_capabilities(int32_t idx)
{
    i2s_capabilities_t val;
    val.protocol_user = 0;
    val.protocol_i2s = 1;
    val.protocol_justified = 1;
    val.protocol_pcm = 1;
    val.mono_mode = 1;
    val.event_frame_error = 1;
    val.mclk_pin  = 1;
    val.full_duplex = 0;

    if (idx == 0) {
        val.mclk_pin  = 0;
        val.full_duplex = 1;
    }

    return val;
}

int32_t csi_i2s_power_control(i2s_handle_t handle, csi_power_stat_e state)
{
    I2S_NULL_PARAM_CHK(handle);
#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_I2S(DRV_ERROR_UNSUPPORTED);
#endif
}

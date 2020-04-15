/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_i2s_v2.c
 * @brief    CSI Source File for I2S Driver
 * @version  V1.2
 * @date     8.15. August 2019
 ******************************************************************************/
#include <csi_config.h>
#include <string.h>
#include <drv/irq.h>
#include <drv/i2s.h>
#include <ck_i2s_v2.h>

#if defined I2S_TRANFSER_DMAC
#include <drv/dmac.h>
#include <soc.h>

extern ck_i2s_priv_v2_t i2s_instance[];

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

static uint32_t ck_i2s_interrupt_status(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    return addr->I2S_ISR;
}

static void ck_i2s_interrupt_clear(ck_i2s_priv_v2_t *priv, uint32_t mask)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_ICR |= mask;
}

static void ck_i2s_tx_dma_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_DMACR;
    val &= ~CK_I2S_DMA_TX_EN_MASK;

    if (en) {
        val |= CK_I2S_DMA_TX_EN_MASK;
    }

    addr->I2S_DMACR = val;
}

static void ck_i2s_rx_dma_enable(ck_i2s_priv_v2_t *priv, int en)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    uint32_t val = addr->I2S_DMACR;
    val &= ~CK_I2S_DMA_RX_EN_MASK;

    if (en) {
        val |= CK_I2S_DMA_RX_EN_MASK;
    }

    addr->I2S_DMACR = val;
}

/*val 0~32*/
static void ck_i2s_tx_dma_data_level(ck_i2s_priv_v2_t *priv, uint8_t val)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_DMATDLR = val;
}

static void ck_i2s_rx_dma_data_level(ck_i2s_priv_v2_t *priv, uint8_t val)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_DMARDLR = val;
}

static void ck_i2s_tx_dma_channel_alloc(ck_i2s_priv_v2_t *priv)
{
#if defined(CONFIG_CHIP_PANGU) || defined (CONFIG_CHIP_TX216)

    if ((priv->tx_en == 0) || priv->tx_dma_ch != -1) {
        return;
    }

    int i = 0;

    if (priv->idx < 2) {
        for (i = 0; i < CONFIG_PER_DMAC0_CHANNEL_NUM; i++) {
            priv->tx_dma_ch = csi_dma_alloc_channel_ex(i);

            if (priv->tx_dma_ch != -1) {
                return;
            }
        }
    } else {
        for (i = CONFIG_PER_DMAC0_CHANNEL_NUM; i < CONFIG_DMA_CHANNEL_NUM; i++) {
            priv->tx_dma_ch = csi_dma_alloc_channel_ex(i);

            if (priv->tx_dma_ch != -1) {
                return;
            }
        }
    }

#else

    if (priv->tx_en && priv->tx_dma_ch == -1) {
        priv->tx_dma_ch = csi_dma_alloc_channel();
    }

#endif
}

static void ck_i2s_rx_dma_channel_alloc(ck_i2s_priv_v2_t *priv)
{
#if defined(CONFIG_CHIP_PANGU) || defined (CONFIG_CHIP_TX216)

    if ((priv->rx_en == 0) || priv->rx_dma_ch != -1) {
        return;
    }

    int i = 0;

    if (priv->idx < 2) {
        for (i = 0; i < CONFIG_PER_DMAC0_CHANNEL_NUM; i++) {
            priv->rx_dma_ch = csi_dma_alloc_channel_ex(i);

            if (priv->rx_dma_ch != -1) {
                return;
            }
        }
    } else {
        for (i = CONFIG_PER_DMAC0_CHANNEL_NUM; i < CONFIG_DMA_CHANNEL_NUM; i++) {
            priv->rx_dma_ch = csi_dma_alloc_channel_ex(i);

            if (priv->rx_dma_ch != -1) {
                return;
            }
        }
    }

#else

    if (priv->rx_en && priv->rx_dma_ch == -1) {
        priv->rx_dma_ch = csi_dma_alloc_channel();
    }

#endif
}

static void ck_i2s_tx_dma_event_cb_fun(int32_t ch, dma_event_e event, void *arg);

static int32_t ck_i2s_dma_tx_config(ck_i2s_priv_v2_t *priv)
{
    if (priv->tx_dma_ch != -1) {
        return 0;
    }

    dma_config_t config;
    int ret;
    extern uint32_t i2s_tx_dma_hs_if_array[];

    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = 4; //unit: 4 bytes
    config.dst_tw = 4; //unit: 4 bytes
    config.hs_if = i2s_tx_dma_hs_if_array[priv->idx];
    config.type   = DMA_MEM2PERH;
    config.src_reload_en = 0;
    config.dest_reload_en = 0;

    ck_i2s_tx_dma_channel_alloc(priv);

    if (priv->tx_dma_ch == -1) {
        return  -1;
    }

    ret = csi_dma_config_channel(priv->tx_dma_ch, &config, ck_i2s_tx_dma_event_cb_fun, (void *)priv);

    if (ret != 0) {
        csi_dma_release_channel(priv->tx_dma_ch);
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    return 0;
}

static int32_t ck_i2s_dma_transfer(ck_i2s_priv_v2_t *priv, uint8_t *data, uint32_t len)
{
    if (len == 0) {
        return len;
    }

    int ret;
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)(priv->base);

    ret = ck_i2s_dma_tx_config(priv);

    if (ret != 0) {
        return -1;
    }

    ck_i2s_tx_dma_enable(priv, 1);
    ck_i2s_tx_dma_data_level(priv, 0x7);

    if (len >= I2S_DMA_MAX_MV_SIZE) {
        len = I2S_DMA_MAX_MV_SIZE;
    }

    csi_dma_start(priv->tx_dma_ch, (void *)data, (void *)&addr->I2S_DR, len / 4);

    return len;
}

static void ck_i2s_rx_dma_event_cb_fun(int32_t ch, dma_event_e event, void *arg);

static int32_t ck_i2s_dma_rx_config(ck_i2s_priv_v2_t *priv)
{
    if (priv->rx_dma_ch != -1) {
        return 0;
    }

    dma_config_t config;
    int ret;
    extern uint32_t i2s_rx_dma_hs_if_array[];

    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_endian = DMA_ADDR_LITTLE;
    config.dst_endian = DMA_ADDR_LITTLE;
    config.src_tw = 4; //unit: 4 bytes
    config.dst_tw = 4; //unit: 4 bytes
    config.hs_if = i2s_rx_dma_hs_if_array[priv->idx];
    config.type   = DMA_PERH2MEM;
    config.src_reload_en = 0;
    config.dest_reload_en = 0;

    ck_i2s_rx_dma_channel_alloc(priv);

    if (priv->rx_dma_ch == -1) {
        return  -1;
    }

    ret = csi_dma_config_channel(priv->rx_dma_ch, &config, ck_i2s_rx_dma_event_cb_fun, (void *)priv);

    if (ret != 0) {
        csi_dma_release_channel(priv->rx_dma_ch);
        return ERR_I2S(DRV_ERROR_PARAMETER);
    }

    return 0;
}

static int32_t ck_i2s_dma_receive(ck_i2s_priv_v2_t *priv, uint8_t *rx_buf, uint32_t len)
{
    if (len == 0) {
        return len;
    }

    int ret;
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)(priv->base);

    ret = ck_i2s_dma_rx_config(priv);

    if (ret != 0) {
        return -1;
    }

    ck_i2s_rx_dma_enable(priv, 1);
    ck_i2s_rx_dma_data_level(priv, 0x10);

    if (len >= I2S_DMA_MAX_MV_SIZE) {
        len = I2S_DMA_MAX_MV_SIZE;
    }

    csi_dma_start(priv->rx_dma_ch, (void *)&addr->I2S_DR, (void *)rx_buf, len / 4);

    return len;
}

static void ck_i2s_dma_run_tx_cb(ck_i2s_priv_v2_t *priv)
{
    int i2s_event = -1;
    ringbuffer_dma_point_t *ringbuf = &priv->tx_ring_buf;

    if (ringbuf->mv_block_size == 0) {
        ringbuf->mv_block_size = priv->tx_period;
        i2s_event = I2S_EVENT_SEND_COMPLETE;
    }

    if (dev_ringbuf_len(&priv->tx_ring_buf.fifo) == 0) {
        i2s_event = I2S_EVENT_TX_BUFFER_EMPYT;
    }

    if (priv->cb != NULL && i2s_event != -1) {
        priv->cb(priv->idx, i2s_event, priv->cb_arg);
    }
}

static uint32_t ringbuffer_point_out(ringbuffer_dma_point_t *tx_ringbuf, uint32_t len)
{
    uint32_t readlen = 0, tmplen = 0;
    dev_ringbuf_t *fifo = &tx_ringbuf->fifo;

    if (dev_ringbuf_is_empty(fifo)) {
        return 0;
    }

    readlen = len > fifo->data_len ? fifo->data_len : len;
    tmplen = fifo->size - fifo->read;

    if (readlen <= tmplen) {
        tx_ringbuf->backward_len = readlen;
        tx_ringbuf->backward_point = &fifo->buffer[fifo->read];
    } else {
        tx_ringbuf->backward_len = tmplen;
        tx_ringbuf->backward_point = &fifo->buffer[fifo->read];
        tx_ringbuf->rewind_len = readlen - tmplen;

        if (tx_ringbuf->rewind_len != 0) {
            tx_ringbuf->rewind_point = fifo->buffer;
        }
    }

    return readlen;
}

static void ck_i2s_dma_run_rx_cb(ck_i2s_priv_v2_t *priv)
{
    int i2s_event = -1;
    ringbuffer_dma_point_t *ringbuf = &priv->rx_ring_buf;

    if (ringbuf->mv_block_size == 0) {
        ringbuf->mv_block_size = priv->rx_period;
        i2s_event = I2S_EVENT_RECEIVE_COMPLETE;
    }

    if (dev_ringbuf_avail(&priv->tx_ring_buf.fifo) == 0) {
        i2s_event = I2S_EVENT_RX_BUFFER_FULL;
    }

    if (priv->cb != NULL && i2s_event != -1) {
        priv->cb(priv->idx, i2s_event, priv->cb_arg);
    }
}

static uint32_t ringbuffer_point_in(ringbuffer_dma_point_t *rx_ringbuffer, uint32_t len)
{
    dev_ringbuf_t *fifo = &rx_ringbuffer->fifo;
    uint32_t writelen = 0, tmplen = 0;

    if (dev_ringbuf_is_full(fifo)) {
        return 0;
    }

    tmplen = fifo->size - fifo->data_len;
    writelen = tmplen > len ? len : tmplen;

    if (fifo->write < fifo->read) {
        rx_ringbuffer->backward_len = writelen;
        rx_ringbuffer->backward_point = &fifo->buffer[fifo->write];
    } else {
        tmplen = fifo->size - fifo->write;

        if (writelen <= tmplen) {
            rx_ringbuffer->backward_len = writelen;
            rx_ringbuffer->backward_point = &fifo->buffer[fifo->write];
        } else {
            rx_ringbuffer->backward_len = tmplen;
            rx_ringbuffer->backward_point = &fifo->buffer[fifo->write];
            rx_ringbuffer->rewind_len = writelen - tmplen;

            if (rx_ringbuffer->rewind_len) {
                rx_ringbuffer->rewind_point = fifo->buffer;
            }
        }
    }

    return writelen;
}

static uint32_t ck_i2s_ringbuf_dma_receive(ck_i2s_priv_v2_t *priv)
{
    ringbuffer_dma_point_t *ringbuf = &priv->rx_ring_buf;
    uint32_t write_len =  ringbuf->backward_len + ringbuf->rewind_len;

    if (ringbuf->backward_point == NULL && ringbuf->rewind_point == NULL) {
        ringbuf->fifo.write = (ringbuf->fifo.write + write_len) % ringbuf->fifo.size;
        ringbuf->fifo.data_len += write_len;
        ringbuf->backward_len = 0;
        ringbuf->rewind_len = 0;
        return write_len;
    }

    if (ringbuf->backward_point != NULL) {
        ck_i2s_dma_receive(priv, priv->rx_ring_buf.backward_point, ringbuf->backward_len);
        ringbuf->backward_point = NULL;
        return 0;
    }

    if (ringbuf->rewind_point != NULL) {
        ck_i2s_dma_receive(priv, priv->rx_ring_buf.rewind_point, ringbuf->rewind_len);
        ringbuf->rewind_point = NULL;
    }

    return 0;
}

static uint32_t ck_i2s_ringbuf_dma_transfer(ck_i2s_priv_v2_t *priv)
{
    ringbuffer_dma_point_t *ringbuf = &priv->tx_ring_buf;
    uint32_t readlen =  ringbuf->backward_len + ringbuf->rewind_len;

    if (ringbuf->backward_point == NULL && ringbuf->rewind_point == NULL) {
        ringbuf->fifo.read = (ringbuf->fifo.read + readlen) % ringbuf->fifo.size;
        ringbuf->fifo.data_len -= readlen;
        ringbuf->backward_len = 0;
        ringbuf->rewind_len = 0;
        return readlen;
    }

    if (ringbuf->backward_point != NULL) {
        ck_i2s_dma_transfer(priv, ringbuf->backward_point, ringbuf->backward_len);
        ringbuf->backward_point = NULL;
        return 0;
    }

    if (ringbuf->rewind_point != NULL) {
        ck_i2s_dma_transfer(priv, ringbuf->rewind_point, ringbuf->rewind_len);
        ringbuf->rewind_point = NULL;
    }

    return 0;
}

static int32_t ck_i2s_dma_cb_ringbuf_in(ck_i2s_priv_v2_t *priv)
{
    uint32_t ret_len = 0;

    ck_i2s_dma_run_rx_cb(priv);

    if (priv->priv_sta & I2S_RX_PAUSE) {
        uint32_t irq_mask = I2S_RX_INPUT_SAMPLE_FREQ_CHANGE_IRQ_MASK
                            | I2S_RX_BUSY_FLAG_REVERSE_IRQ_MASK
                            | I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK
                            | I2S_RX_OVERFLOW_ERROR_IRQ_MASK
                            | I2S_RX_UNDERFLOW_ERROR_IRQ_MASK;
        ck_i2s_interrupt_ctrl(priv, irq_mask, 0);
        return 0;
    }

    uint32_t blcok_size = min(priv->rx_ring_buf.mv_block_size, I2S_DMA_MAX_MV_SIZE);
    ret_len = ringbuffer_point_in(&priv->rx_ring_buf, blcok_size);

    if (ret_len == 0) {
        priv->priv_sta &= ~I2S_RX_RUNING;
        ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 1);
        return ret_len;
    }

    ck_i2s_ringbuf_dma_receive(priv);

    priv->rx_ring_buf.mv_block_size -= ret_len;

    return ret_len;
}

static int32_t ck_i2s_dma_cb_dev_ringbuf_out(ck_i2s_priv_v2_t *priv)
{
    uint32_t ret_len = 0;

    ck_i2s_dma_run_tx_cb(priv);

    if (priv->priv_sta & I2S_TX_PAUSE) {
        uint32_t irq_mask = I2S_TX_BUSY_FLAG_REVERSE_IRQ_MASK
                            | I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK
                            | I2S_TX_OVERFLOW_ERROR_IRQ_MASK
                            | I2S_TX_UNDERFLOW_ERROR_IRQ_MASK;
        ck_i2s_interrupt_ctrl(priv, irq_mask, 0);
        return 0;
    }

    uint32_t blcok_size = min(priv->tx_ring_buf.mv_block_size, I2S_DMA_MAX_MV_SIZE);
    ret_len = ringbuffer_point_out(&priv->tx_ring_buf, blcok_size);

    if (ret_len == 0) {
        priv->priv_sta &= ~I2S_TX_RUNING;
        ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 1);
        return ret_len;
    }

    ck_i2s_ringbuf_dma_transfer(priv);

    priv->tx_ring_buf.mv_block_size -= ret_len;

    return ret_len;
}

static void ck_i2s_tx_dma_event_cb_fun(int32_t ch, dma_event_e event, void *arg)
{
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)arg;

    if (ch != priv->tx_dma_ch) {
        return;
    }

    if (event == DMA_EVENT_TRANSFER_DONE) {
        /*if ringbuf last data not transfer complete, this func resturn 0*/
        if (ck_i2s_ringbuf_dma_transfer(priv) == 0) {
            return;
        }

        ck_i2s_dma_cb_dev_ringbuf_out(priv);
    }
}

static void ck_i2s_rx_dma_event_cb_fun(int32_t ch, dma_event_e event, void *arg)
{
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)arg;

    if (ch != priv->rx_dma_ch) {
        return;
    }

    if (event == DMA_EVENT_TRANSFER_DONE) {
        /*if ringbuf last data not transfer complete, this func resturn 0*/
        if (ck_i2s_ringbuf_dma_receive(priv) == 0) {
            return;
        }

        ck_i2s_dma_cb_ringbuf_in(priv);
    }
}

static void ck_i2s_start_dma_tx_stream(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 0);

    uint32_t blcok_size = min(priv->tx_ring_buf.mv_block_size, I2S_DMA_MAX_MV_SIZE);
    uint32_t ret_len = ringbuffer_point_out(&priv->tx_ring_buf, blcok_size);

    if (ret_len == 0) {
        priv->priv_sta &= ~I2S_TX_RUNING;
        ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 1);
        return;
    }

    ck_i2s_ringbuf_dma_transfer(priv);
    priv->tx_ring_buf.mv_block_size -= ret_len;
}

uint32_t ck_i2s_dma_send(i2s_handle_t handle, const uint8_t *data, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;
    uint32_t len = dev_ringbuf_in(&priv->tx_ring_buf.fifo, data, length);

    if ((priv->priv_sta & I2S_TX_RUNING) == 0) {
        priv->priv_sta |= I2S_TX_RUNING;
        ck_i2s_start_dma_tx_stream(priv);
    }

    return len;
}

static void ck_i2s_start_dma_rx_stream(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);

    uint32_t blcok_size = min(priv->rx_ring_buf.mv_block_size, I2S_DMA_MAX_MV_SIZE);
    uint32_t ret_len = ringbuffer_point_in(&priv->rx_ring_buf, blcok_size);

    /*data can't input to rx ringbuffer, rx ringbuffer is full*/
    if (ret_len == 0) {
        priv->priv_sta &= ~I2S_RX_RUNING;
        ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 1);
    }

    ck_i2s_ringbuf_dma_receive(priv);
    priv->rx_ring_buf.mv_block_size -= ret_len;
    ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);
}

uint32_t ck_i2s_dma_recv(i2s_handle_t handle, uint8_t *buf, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    uint32_t len = dev_ringbuf_out(&priv->rx_ring_buf.fifo, buf, length);

    if ((priv->priv_sta & I2S_RX_RUNING) == 0) {
        priv->priv_sta |= I2S_RX_RUNING;
        ck_i2s_start_dma_rx_stream(priv);
    }

    return len;
}

static void ck_i2s_dma_stream_irq_handle(int32_t idx)
{
    ck_i2s_priv_v2_t *priv = &i2s_instance[idx];
    uint32_t irq_sta =  ck_i2s_interrupt_status(priv);
    uint32_t i2s_event = 0;

    if (irq_sta & I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK) {
        ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 0);
        i2s_event |= I2S_EVENT_TX_UNDERFLOW;
    }

    if (irq_sta & I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK) {
        ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);
        i2s_event |= I2S_EVENT_RX_BUFFER_FULL;
    }

    if (priv->tx_ring_buf.mv_block_size == 0) {
        priv->tx_ring_buf.mv_block_size = priv->tx_period;
    }

    if (priv->rx_ring_buf.mv_block_size == 0) {
        priv->rx_ring_buf.mv_block_size = priv->rx_period;
    }

    if (priv->cb != NULL) {
        priv->cb(idx, i2s_event, priv->cb_arg);
    }

    ck_i2s_interrupt_clear(priv, irq_sta);

}

static void ck_i2s_dma_stream_uninit(ck_i2s_priv_v2_t *priv)
{
    if (priv->rx_dma_ch != -1) {
        csi_dma_release_channel(priv->rx_dma_ch);
    }

    if (priv->tx_dma_ch != -1) {
        csi_dma_release_channel(priv->tx_dma_ch);
    }
}

static void ck_i2s_tx_dma_stream_stop(ck_i2s_priv_v2_t *priv)
{
    csi_dma_stop(priv->tx_dma_ch);
    ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 0);
    dev_ringbuff_reset(&priv->tx_ring_buf.fifo);
}

static void ck_i2s_rx_dma_stream_stop(ck_i2s_priv_v2_t *priv)
{
    csi_dma_stop(priv->rx_dma_ch);
    ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);
    dev_ringbuff_reset(&priv->rx_ring_buf.fifo);
}

ck_i2s_stream_obj priv_stream_obj = {
    .stream_uninit = ck_i2s_dma_stream_uninit,
    .stream_send = ck_i2s_dma_send,
    .stream_recv = ck_i2s_dma_recv,
    .stream_tx_start = ck_i2s_start_dma_tx_stream,
    .stream_rx_start = ck_i2s_start_dma_rx_stream,
    .stream_tx_stop = ck_i2s_tx_dma_stream_stop,
    .stream_rx_stop = ck_i2s_rx_dma_stream_stop,
    .stream_irq_handle = ck_i2s_dma_stream_irq_handle
};

#endif

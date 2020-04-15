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
 *******************************************************************************/
#include <csi_config.h>
#include <string.h>
#include <drv/irq.h>
#include <drv/i2s.h>
#include <ck_i2s_v2.h>

#if defined (I2S_TRANFSER_INTERRUPT)
#include <soc.h>

#define I2S_HW_FIFO_DEPTH 32
extern ck_i2s_priv_v2_t i2s_instance[];

static void ck_i2s_interrupt_imk_clear(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_IMR = 0;
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

static uint32_t ck_i2s_tx_fifo_data_num(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    return (addr->I2S_TXFLR & I2S_FIFO_DATA_NUM_MASK);
}

static uint32_t ck_i2s_rx_fifo_data_num(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    return (addr->I2S_RXFLR & I2S_FIFO_DATA_NUM_MASK);
}

static uint32_t ck_i2s_read(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    return addr->I2S_DR;
}

static void ck_i2s_write(ck_i2s_priv_v2_t *priv, uint32_t data)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_DR = data;
}

static void ck_i2s_interrupt_stream_uninit(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_interrupt_imk_clear(priv);
}

static uint32_t tx_buf[I2S_HW_FIFO_DEPTH];
static uint32_t rx_buf[I2S_HW_FIFO_DEPTH];

static void ck_i2s_start_tx_interrupt_stream(ck_i2s_priv_v2_t *priv)
{
    dev_ringbuf_t *fifo = &priv->tx_ring_buf.fifo;
    uint32_t hw_tx_fifo_avail = I2S_HW_FIFO_DEPTH - ck_i2s_tx_fifo_data_num(priv) * 4;
    uint32_t out_data_len = min(dev_ringbuf_len(fifo), hw_tx_fifo_avail);
    out_data_len -= out_data_len % 4;
    int ret_len = dev_ringbuf_out(fifo, tx_buf, out_data_len);

    if (ret_len == 0) {
        priv->priv_sta &= ~I2S_TX_RUNING;
        ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 0);
        return;
    }

    int i = 0;

    while (i < ret_len / 4) {
        ck_i2s_write(priv, tx_buf[i]);
        i++;
    }

    priv->tx_ring_buf.mv_block_size -= ret_len;

    if (priv->tx_ring_buf.mv_block_size == 0) {
        priv->tx_ring_buf.mv_block_size = priv->tx_period;
    }

    ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 1);
}

static void ck_i2s_start_rx_interrupt_stream(ck_i2s_priv_v2_t *priv)
{
    dev_ringbuf_t *fifo = &priv->rx_ring_buf.fifo;

    if (dev_ringbuf_avail(fifo) == 0) {
        priv->priv_sta &= ~I2S_RX_RUNING;
        ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);
        return;
    }

    ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 1);
}

static void ck_i2s_tx_interrupt_stream_stop(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 0);
    dev_ringbuff_reset(&priv->tx_ring_buf.fifo);
}

static void ck_i2s_rx_interrupt_stream_stop(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);
    dev_ringbuff_reset(&priv->rx_ring_buf.fifo);
}


static uint8_t tx_empty_cnt[4];
static void ck_i2s_interrupt_tranfser(ck_i2s_priv_v2_t *priv)
{
    dev_ringbuf_t *fifo = &priv->tx_ring_buf.fifo;
    uint32_t event = 0;
    uint32_t hw_tx_fifo_avail = (I2S_HW_FIFO_DEPTH - ck_i2s_tx_fifo_data_num(priv));

    if (dev_ringbuf_len(fifo) == 0 || hw_tx_fifo_avail == I2S_HW_FIFO_DEPTH) {
        if (tx_empty_cnt[priv->idx] == 0) {
            event |= I2S_EVENT_TX_BUFFER_EMPYT;
        } else {
            event |= I2S_EVENT_TX_UNDERFLOW;
            priv->priv_sta &= ~I2S_TX_RUNING;
        }

        tx_empty_cnt[priv->idx]++;
        tx_empty_cnt[priv->idx] = tx_empty_cnt[priv->idx] % 2;
    } else {
        tx_empty_cnt[priv->idx] = 0;
    }

    hw_tx_fifo_avail *= 4;
    uint32_t out_data_len = min(dev_ringbuf_len(fifo), hw_tx_fifo_avail);
    out_data_len -= out_data_len % 4;
    int ret_len = dev_ringbuf_out(fifo, tx_buf, out_data_len);
    int i = 0;

    while (i < ret_len / 4) {
        ck_i2s_write(priv, tx_buf[i]);
        i++;
    }

    priv->tx_ring_buf.mv_block_size -= ret_len;

    if (priv->tx_ring_buf.mv_block_size == 0) {
        priv->tx_ring_buf.mv_block_size = priv->tx_period;
        event |= I2S_EVENT_SEND_COMPLETE;
    }

    if (priv->priv_sta & I2S_TX_PAUSE || event & I2S_EVENT_TX_UNDERFLOW) {
        ck_i2s_interrupt_ctrl(priv, I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK, 0);
    }

    if (priv->cb != NULL && event != 0) {
        priv->cb(priv->idx, event, priv->cb_arg);
    }
}

static uint8_t rx_full_cnt[4];

static void ck_i2s_interrupt_receive(ck_i2s_priv_v2_t *priv)
{
    dev_ringbuf_t *fifo = &priv->rx_ring_buf.fifo;
    uint32_t event = 0;

    uint32_t rx_hw_fifo_num = ck_i2s_rx_fifo_data_num(priv);
    uint32_t rx_ring_buf_avail = dev_ringbuf_avail(fifo);
    uint32_t rx_read_data_len = min(rx_ring_buf_avail, rx_hw_fifo_num);
    rx_read_data_len -= rx_read_data_len % 4;

    if (rx_ring_buf_avail == 0 || rx_hw_fifo_num == I2S_HW_FIFO_DEPTH) {
        if (rx_full_cnt[priv->idx] == 0) {
            event |= I2S_EVENT_RX_BUFFER_FULL;
        } else {
            event |= I2S_EVENT_RX_OVERFLOW;
            priv->priv_sta &= ~I2S_RX_RUNING;
        }

        rx_full_cnt[priv->idx]++;
    } else {
        rx_full_cnt[priv->idx] = 0;
    }

    int i = 0;

    while (i < rx_read_data_len / 4) {
        rx_buf[i] = ck_i2s_read(priv);
        i++;
    }

    int ret_len = dev_ringbuf_in(fifo, rx_buf, rx_read_data_len);
    priv->rx_ring_buf.mv_block_size -= ret_len;

    if (priv->rx_ring_buf.mv_block_size == 0) {
        priv->rx_ring_buf.mv_block_size = priv->rx_period;
        event |= I2S_EVENT_RECEIVE_COMPLETE;
    }

    if (priv->cb != NULL && event != 0) {
        priv->cb(priv->idx, event, priv->cb_arg);
    }

    if (priv->priv_sta & I2S_RX_PAUSE || event & I2S_EVENT_RX_OVERFLOW) {
        ck_i2s_interrupt_ctrl(priv, I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK, 0);
    }

}

uint32_t ck_i2s_interrupt_send(i2s_handle_t handle, const uint8_t *data, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;
    uint32_t len = dev_ringbuf_in(&priv->tx_ring_buf.fifo, data, length);

    if ((priv->priv_sta & I2S_TX_RUNING) == 0) {
        priv->priv_sta |= I2S_TX_RUNING;
        ck_i2s_start_tx_interrupt_stream(priv);
    }

    return len;
}

uint32_t ck_i2s_interrupt_recv(i2s_handle_t handle, uint8_t *buf, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    uint32_t len = dev_ringbuf_out(&priv->rx_ring_buf.fifo, buf, length);

    if ((priv->priv_sta & I2S_RX_RUNING) == 0) {
        priv->priv_sta |= I2S_RX_RUNING;
        ck_i2s_start_rx_interrupt_stream(priv);
    }

    return len;
}

static void ck_i2s_interrupt_stream_irq_handle(int idx)
{
    ck_i2s_priv_v2_t *priv = &i2s_instance[idx];
    uint32_t irq_sta =  ck_i2s_interrupt_status(priv);

    if (irq_sta & I2S_TX_FIFO_THRESHOLD_EMPTY_IRQ_MASK) {
        ck_i2s_interrupt_tranfser(priv);
    }

    if (irq_sta & I2S_RX_FIFO_THRESHOLD_FULL_IRQ_MASK) {
        ck_i2s_interrupt_receive(priv);
    }

    ck_i2s_interrupt_clear(priv, irq_sta);
}

ck_i2s_stream_obj priv_stream_obj = {
    .stream_uninit = ck_i2s_interrupt_stream_uninit,
    .stream_send = ck_i2s_interrupt_send,
    .stream_recv = ck_i2s_interrupt_recv,
    .stream_tx_start = ck_i2s_start_tx_interrupt_stream,
    .stream_rx_start = ck_i2s_start_rx_interrupt_stream,
    .stream_tx_stop = ck_i2s_tx_interrupt_stream_stop,
    .stream_rx_stop = ck_i2s_rx_interrupt_stream_stop,
    .stream_irq_handle = ck_i2s_interrupt_stream_irq_handle
};

#endif

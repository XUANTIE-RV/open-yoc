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

#if defined (I2S_TRANFSER_POLLING)
#include <soc.h>

#define I2S_HW_FIFO_DEPTH 32
extern ck_i2s_priv_v2_t i2s_instance[];

static void ck_i2s_interrupt_imk_clear(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    addr->I2S_IMR = 0;
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

static void ck_i2s_polling_stream_uninit(ck_i2s_priv_v2_t *priv)
{
    ;
}

/*return data see i2s status code  */
static uint32_t ck_i2s_status(ck_i2s_priv_v2_t *priv)
{
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;
    return addr->I2S_SR;
}

static int32_t ck_i2s_tx_fifo_not_full(ck_i2s_priv_v2_t *priv)
{
    if (ck_i2s_status(priv) & I2S_TX_FIFO_NOT_FULL) {
        return 1;
    }

    return 0;
}

static uint32_t ck_i2s_polling_send(i2s_handle_t handle, const uint8_t *data, uint32_t length)
{
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;
    uint32_t send_len = length - length % 4;
    ck_i2s_interrupt_imk_clear(priv);

    int i = 0, j = 0;
    uint32_t timer_out = I2S_TIME_OUT;
    uint32_t tx = 0;

    while (i < send_len / 4) {
        while (!ck_i2s_tx_fifo_not_full(priv) && timer_out) {
            timer_out--;
        }

        memcpy(&tx, &data[j], 4);
        ck_i2s_write(priv, tx);
        i++;
        j += 4;
    }

    return send_len;
}

static int32_t ck_i2s_rx_fifo_not_empty(ck_i2s_priv_v2_t *priv)
{
    if (ck_i2s_status(priv) & I2S_RX_FIFO_NOT_EMPTY) {
        return 1;
    }

    return 0;
}

static uint32_t ck_i2s_polling_recv(i2s_handle_t handle, uint8_t *buf, uint32_t length)
{
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;
    uint32_t read_len = length - length % 4;
    ck_i2s_interrupt_imk_clear(priv);

    int i = 0, j = 0;
    uint32_t timer_out = I2S_TIME_OUT;
    uint32_t rx = 0;

    while (i < read_len / 4) {
        while (!ck_i2s_rx_fifo_not_empty(priv) && timer_out) {
            timer_out--;
        }

        rx = ck_i2s_read(priv);
        memcpy(&buf[j], &rx, 4);
        i++;
        j += 4;
    }

    return read_len;
}

static void ck_i2s_tx_polling_stream_start(ck_i2s_priv_v2_t *priv)
{
    ;
}

static void ck_i2s_rx_polling_stream_start(ck_i2s_priv_v2_t *priv)
{
    ;
}

static void ck_i2s_tx_polling_stream_stop(ck_i2s_priv_v2_t *priv)
{
    ;
}

static void ck_i2s_rx_polling_stream_stop(ck_i2s_priv_v2_t *priv)
{
    ;
}

static void ck_i2s_polling_stream_irq_handle(int idx)
{
    ;
}

ck_i2s_stream_obj priv_stream_obj = {
    .stream_uninit = ck_i2s_polling_stream_uninit,
    .stream_send = ck_i2s_polling_send,
    .stream_recv = ck_i2s_polling_recv,
    .stream_tx_start = ck_i2s_tx_polling_stream_start,
    .stream_rx_start = ck_i2s_rx_polling_stream_start,
    .stream_tx_stop = ck_i2s_tx_polling_stream_stop,
    .stream_rx_stop = ck_i2s_rx_polling_stream_stop,
    .stream_irq_handle = ck_i2s_polling_stream_irq_handle
};

#endif

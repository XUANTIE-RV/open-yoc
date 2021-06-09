/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <csi_config.h>
#include <sys_clk.h>
#include <drv/i2s.h>
#include <drv/irq.h>
#include <wj_i2s_ll.h>

extern uint16_t i2s_tx_hs_num[];
extern uint16_t i2s_rx_hs_num[];


static int32_t i2s_send_polling(csi_i2s_t *i2s, const void *data, uint32_t num)
{
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);
    int32_t sent_num = (int32_t)num;
    uint32_t send_fifo_Level = 0U;
    uint32_t emptyfifo = 0U;
    uint16_t i = 0U;
    uint32_t *send_data = (uint32_t *)data;
    num = num / 4U;
    wj_i2s_disable(i2s_base);
    wj_i2s_set_transmit_mode(i2s_base);
    wj_i2s_enable(i2s_base);

    while (1) {
        send_fifo_Level = wj_i2s_get_transmit_fifo_level(i2s_base);
        emptyfifo = (num > (I2S_MAX_FIFO - send_fifo_Level)) ? (I2S_MAX_FIFO - send_fifo_Level) : num;

        for (i = 0U; i < emptyfifo; i++) {
            wj_i2s_transmit_data(i2s_base, *(send_data++));
        }

        while (wj_i2s_get_transmit_fifo_level(i2s_base));

        num -= emptyfifo;

        if (num == 0U) {
            break;
        }

    }

    return sent_num;
}

static int32_t i2s_receive_polling(csi_i2s_t *i2s, void *data, uint32_t num)
{
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);
    int32_t received_size = (int32_t)num;
    uint32_t receive_fifo_Level = 0U;
    uint16_t i = 0U;
    uint32_t *readdata = (uint32_t *)data;
    num = num / 4U;
    wj_i2s_disable(i2s_base);
    wj_i2s_set_receive_mode(i2s_base);
    wj_i2s_enable(i2s_base);

    /* clear fifo*/
    uint32_t fifo_num = wj_i2s_get_receive_fifo_level(i2s_base);

    for (uint8_t i = 0U; i < fifo_num; i++) {
        wj_i2s_receive_data(i2s_base);
    }

    while (1) {
        receive_fifo_Level = wj_i2s_get_receive_fifo_level(i2s_base);

        for (i = 0U; i < receive_fifo_Level; i++) {
            *(readdata++) = wj_i2s_receive_data(i2s_base);
        }

        num -= receive_fifo_Level;

        if (num == 0U) {
            break;
        }
    }

    return received_size;
}


static void wj_i2s_clear_fifo(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);
    uint32_t i, temp;

    temp = wj_i2s_get_transmit_fifo_level(i2s_base);

    if (temp < 32U) {
        for (i = 0U; i < (32U - temp); i++) {
            wj_i2s_transmit_data(i2s_base, 0U);
        }
    }

    temp = wj_i2s_get_receive_fifo_level(i2s_base);

    if (temp < 32U) {
        for (i = 0U; i < (32U - temp); i++) {
            wj_i2s_receive_data(i2s_base);
        }
    }
}

static csi_error_t wj_i2s_calc_mclk_div(csi_i2s_t *i2s, csi_i2s_format_t *format)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    CSI_PARAM_CHK(format, CSI_ERROR);
    uint32_t div;
    uint32_t mclk;
    wj_i2s_regs_t *i2s_base;
    csi_error_t ret = CSI_ERROR;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);


    mclk = 2U * (uint32_t)format->width * (uint32_t)format->rate * 8U;

    div = soc_get_i2s_freq((uint32_t)i2s->dev.idx);
    div = (div * 5U) / mclk;

    if ((div % 10U) > 4U) {
        div /= 10U;
    } else {
        div = (div / 10U) - 1U;
    }


    wj_i2s_set_div_mclk(i2s_base, div);

    return ret;
}

static csi_error_t wj_i2s_calc_fs_div(csi_i2s_t *i2s, csi_i2s_format_t *format)
{
    uint32_t div = 15U;
    wj_i2s_regs_t *i2s_base;
    csi_error_t ret = CSI_OK;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);

    if ((format->protocol == I2S_PROTOCOL_I2S) || (format->protocol == I2S_PROTOCOL_MSB_JUSTIFIED)) {
        if (format->width == I2S_SAMPLE_WIDTH_16BIT) {
            div = 15U;
        } else if (format->width == I2S_SAMPLE_WIDTH_24BIT) {
            div = 23U;
        } else {
            ret = CSI_ERROR;
        }
    } else {
        if (format->width == I2S_SAMPLE_WIDTH_16BIT) {
            div = 31U;
        } else if (format->width == I2S_SAMPLE_WIDTH_24BIT) {
            div = 47U;
        } else {
            ret = CSI_ERROR;
        }
    }

    wj_i2s_set_div_wsclk(i2s_base, div);

    return ret;
}

#ifdef CONFIG_XIP
#define I2S_CODE_IN_RAM __attribute__((section(".ram.code")))
#else
#define I2S_CODE_IN_RAM
#endif

I2S_CODE_IN_RAM void wj_i2s_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    CSI_PARAM_CHK_NORETVAL(dma);
    csi_i2s_t *i2s = (csi_i2s_t *)dma->parent;
    wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;

    if (event == DMA_EVENT_TRANSFER_ERROR) {/* DMA transfer ERROR */
        if (i2s->tx_dma->ch_id == dma->ch_id) {
            /* to do tx error action */
            i2s->state.error = 1U;

            if (i2s->callback) {
                i2s->callback(i2s, I2S_EVENT_ERROR, i2s->arg);
            }
        } else if (i2s->rx_dma->ch_id == dma->ch_id) {
            /* to do rx error action */
            i2s->state.error = 1U;

            if (i2s->callback) {
                i2s->callback(i2s, I2S_EVENT_ERROR, i2s->arg);
            }
        }

    } else if (event == DMA_EVENT_TRANSFER_DONE) {/* DMA transfer complete */
        if ((i2s->tx_dma != NULL) && (i2s->tx_dma->ch_id == dma->ch_id)) {
            /* to do tx action */
            uint32_t read_len = i2s->tx_period;

            if (i2s->tx_buf->data_len < i2s->tx_period) {
                if (i2s->callback) {
                    i2s->callback(i2s, I2S_EVENT_TX_BUFFER_EMPTY, i2s->arg);
                }
            } else {
                i2s->tx_buf->read = (i2s->tx_buf->read + read_len) % i2s->tx_buf->size;
                i2s->tx_buf->data_len -= read_len;
            }

            uint32_t tx_num = 0;
            uint32_t TX_FIFO[10];
            uint32_t send_fifo_Level = 0;
            uint32_t emptyfifo = 0;
            send_fifo_Level = wj_i2s_get_transmit_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);
            emptyfifo = (10 > (I2S_MAX_FIFO - send_fifo_Level)) ? I2S_MAX_FIFO - send_fifo_Level : 10;

            memcpy(TX_FIFO, i2s->tx_buf->buffer + i2s->tx_buf->read, emptyfifo * 4);

            for (tx_num = 0; tx_num < (emptyfifo); tx_num ++) {
                wj_i2s_transmit_data((wj_i2s_regs_t *)i2s->dev.reg_base, TX_FIFO[tx_num]);
            }

            csi_dma_ch_start(i2s->tx_dma, i2s->tx_buf->buffer + i2s->tx_buf->read  + (tx_num << 2), (void *) & (i2s_base->I2S_DR), i2s->tx_period - (tx_num << 2));

            if (i2s->callback) {
                i2s->callback(i2s, I2S_EVENT_SEND_COMPLETE, i2s->arg);
            }
        } else {
            /* to do rx action */
            uint32_t write_len = i2s->rx_period;
            i2s->rx_buf->write = (i2s->rx_buf->write + write_len) % i2s->rx_buf->size;
            i2s->rx_buf->data_len += write_len;
            i2s->rx_buf->data_len = i2s->rx_buf->data_len % i2s->rx_buf->size;

            uint32_t rx_num = 0U;
            uint32_t RX_FIFO[16];
            uint32_t receive_fifo_level = 0U;
            uint32_t emptyfifo = 0U;
            receive_fifo_level = wj_i2s_get_receive_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);

            if (receive_fifo_level > 24U) {
                emptyfifo = (16U > (I2S_MAX_FIFO - receive_fifo_level)) ? (I2S_MAX_FIFO - receive_fifo_level) : 16U;

                for (rx_num = 0U; rx_num < emptyfifo; rx_num ++) {
                    RX_FIFO[rx_num] = wj_i2s_receive_data((wj_i2s_regs_t *)i2s->dev.reg_base);
                }

                memcpy(i2s->rx_buf->buffer + i2s->rx_buf->write, RX_FIFO, emptyfifo << 2U);
                soc_dcache_clean_invalid_range((unsigned long)(i2s->rx_buf->buffer + i2s->rx_buf->write + (rx_num << 2U)), i2s->rx_period - (rx_num << 2U));
                csi_dma_ch_start(i2s->rx_dma, (void *) & (i2s_base->I2S_DR), i2s->rx_buf->buffer + i2s->rx_buf->write + (rx_num << 2U), i2s->rx_period - (rx_num << 2U));
            } else {
                soc_dcache_clean_invalid_range((unsigned long)(i2s->rx_buf->buffer + i2s->rx_buf->write), i2s->rx_period);
                csi_dma_ch_start(i2s->rx_dma, (void *) & (i2s_base->I2S_DR), i2s->rx_buf->buffer + i2s->rx_buf->write, i2s->rx_period);
            }

            if (i2s->callback && i2s->rx_buf->data_len != 0) {
                i2s->callback(i2s, I2S_EVENT_RECEIVE_COMPLETE, i2s->arg);
            } else {
                i2s->callback(i2s, I2S_EVENT_RX_BUFFER_FULL, i2s->arg);
            }
        }
    }
}

/**
  \brief       I2S init
  \param[in]   i2s i2s handle to operate.
  \param[in]   idx i2s interface idx
  \return      error code
*/
csi_error_t csi_i2s_init(csi_i2s_t *i2s, uint32_t idx)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    ret = target_get(DEV_WJ_I2S_TAG, idx, &i2s->dev);

    if (ret == CSI_OK) {
        wj_i2s_regs_t *i2s_base;
        i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);
        memset((void *)i2s_base, 0, sizeof(wj_i2s_regs_t));
    }

    i2s->rx_dma = NULL;
    i2s->tx_dma = NULL;
    i2s->rx_buf = NULL;
    i2s->tx_buf = NULL;
    i2s->state.error = 0U;
    i2s->state.readable = 0U;
    i2s->state.writeable = 0U;
    i2s->priv = (void *)0U;
    i2s->callback = NULL;
    return ret;
}

/**
  \brief       I2S uninit
  \param[in]   i2s i2s handle to operate.
  \return      none
*/
void csi_i2s_uninit(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    wj_i2s_regs_t *i2s_base;
    i2s->rx_dma = NULL;
    i2s->tx_dma = NULL;
    i2s->rx_buf = NULL;
    i2s->tx_buf = NULL;
    i2s->state.error = 0U;
    i2s->state.readable = 0U;
    i2s->state.writeable = 0U;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);
    wj_i2s_disable(i2s_base);
    memset((void *)i2s_base, 0, sizeof(wj_i2s_regs_t));
}

/**
  \brief       I2S enable
  \param[in]   i2s i2s handle to operate.
  \param[in]   en  1 enable, 0 disable
  \return      none
*/
void csi_i2s_enable(csi_i2s_t *i2s, bool enable)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);

    if (enable) {
        wj_i2s_enable(i2s_base);
    } else {
        extern void mdelay(uint32_t ms);
        mdelay(1U); ///< I2S v1 hardware bug ，must close delay 1ms
        wj_i2s_disable(i2s_base);
    }
}

/**
  \brief       I2S config
  \param[in]   i2s i2s handle to operate.
  \param[in]   config i2s config param
  \return      error code
*/
csi_error_t csi_i2s_format(csi_i2s_t *i2s, csi_i2s_format_t *format)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    CSI_PARAM_CHK(format, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);
    wj_i2s_disable(i2s_base);

    switch (format->mode) {
        case I2S_MODE_MASTER:
            wj_i2s_receive_mode_master(i2s_base);
            wj_i2s_transmit_mode_master(i2s_base);
            break;

        case I2S_MODE_SLAVE:
            wj_i2s_receive_mode_slave(i2s_base);
            wj_i2s_transmit_mode_slave(i2s_base);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    switch (format->protocol) {
        case I2S_PROTOCOL_I2S:
            wj_i2s_receive_mode_format_i2s(i2s_base);
            wj_i2s_transmit_mode_format_i2s(i2s_base);
            break;

        case I2S_PROTOCOL_MSB_JUSTIFIED:
            wj_i2s_receive_mode_format_left_justified(i2s_base);
            wj_i2s_transmit_mode_format_left_justified(i2s_base);
            break;

        case I2S_PROTOCOL_LSB_JUSTIFIED:
            wj_i2s_receive_mode_format_right_justified(i2s_base);
            wj_i2s_transmit_mode_format_right_justified(i2s_base);
            break;

        case I2S_PROTOCOL_PCM:
            ret = CSI_UNSUPPORTED;
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    switch (format->width) {
        case I2S_SAMPLE_WIDTH_16BIT:
            wj_i2s_receive_mode_16bit_input_16bit_store(i2s_base);
            break;

        case I2S_SAMPLE_WIDTH_24BIT:
            wj_i2s_receive_mode_24bit_input_24bit_store(i2s_base);
            break;

        case I2S_SAMPLE_WIDTH_32BIT:
            ret = CSI_UNSUPPORTED;
            break;

        default:
            ret = CSI_UNSUPPORTED;
            break;
    }

    if (format->mode == I2S_MODE_MASTER) {
        switch (format->rate) {
            case I2S_SAMPLE_RATE_8000:
                wj_i2s_receive_mode_audio_fundamental_rate_32k(i2s_base);
                wj_i2s_receive_mode_audio_rate_scale_factor_0_25(i2s_base);
                break;

            case I2S_SAMPLE_RATE_16000:
                wj_i2s_receive_mode_audio_fundamental_rate_32k(i2s_base);
                wj_i2s_receive_mode_audio_rate_scale_factor_0_5(i2s_base);
                break;

            case I2S_SAMPLE_RATE_32000:
                wj_i2s_receive_mode_audio_fundamental_rate_32k(i2s_base);
                break;

            case I2S_SAMPLE_RATE_44100:
                wj_i2s_receive_mode_audio_fundamental_rate_44_1k(i2s_base);
                break;

            case I2S_SAMPLE_RATE_48000:
                wj_i2s_receive_mode_audio_fundamental_rate_48k(i2s_base);
                break;

            case I2S_SAMPLE_RATE_96000:
                wj_i2s_receive_mode_audio_fundamental_rate_96k(i2s_base);
                break;

            default:
                ret = CSI_UNSUPPORTED;
                break;
        }
    } else {
        wj_i2s_receive_mode_audio_input_rate_detected_hardware(i2s_base);
    }

    if (format->polarity) {
        wj_i2s_transmit_mode_channel_polarity_high_for_left(i2s_base);
        wj_i2s_receive_mode_channel_polarity_high_for_left(i2s_base);
    } else {
        wj_i2s_transmit_mode_channel_polarity_low_for_left(i2s_base);
        wj_i2s_receive_mode_channel_polarity_low_for_left(i2s_base);
    }

    wj_i2s_calc_mclk_div(i2s, format);
    wj_i2s_calc_fs_div(i2s, format);

    return ret;
}

/**
  \brief       Set the i2s tx mono
  \param[in]   i2s i2s handle to operate.
  \param[in]   Mono channel selection.
  \param[in]   bool mono mode enable
  \return      error code
*/
csi_error_t csi_i2s_tx_select_sound_channel(csi_i2s_t *i2s, csi_i2s_sound_channel_t ch)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);

    switch (ch) {
        case I2S_LEFT_CHANNEL:
            wj_i2s_transmit_mode_source_is_single_channel(i2s_base);
            wj_i2s_transmit_mode_channel_polarity_low_for_left(i2s_base);
            break;

        case I2S_RIGHT_CHANNEL:
            wj_i2s_transmit_mode_source_is_single_channel(i2s_base);
            wj_i2s_transmit_mode_channel_polarity_high_for_left(i2s_base);
            break;

        case I2S_LEFT_RIGHT_CHANNEL:
            wj_i2s_transmit_mode_source_is_dual_channel(i2s_base);
            break;

        default:
            break;
    }

    return ret;
}

/**
  \brief       Set the i2s rx mono
  \param[in]   i2s i2s handle to operate.
  \param[in]   Mono channel selection.
  \param[in]   bool mono mode enable
  \return      error code
*/
csi_error_t csi_i2s_rx_select_sound_channel(csi_i2s_t *i2s, csi_i2s_sound_channel_t ch)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_i2s_regs_t *i2s_base;
    i2s_base = (wj_i2s_regs_t *)HANDLE_REG_BASE(i2s);

    switch (ch) {
        case I2S_LEFT_CHANNEL:
            wj_i2s_transmit_mode_source_is_single_channel(i2s_base);
            wj_i2s_transmit_mode_channel_polarity_low_for_left(i2s_base);
            break;

        case I2S_RIGHT_CHANNEL:
            wj_i2s_transmit_mode_source_is_single_channel(i2s_base);
            wj_i2s_transmit_mode_channel_polarity_high_for_left(i2s_base);
            break;

        case I2S_LEFT_RIGHT_CHANNEL:
            wj_i2s_transmit_mode_source_is_dual_channel(i2s_base);
            break;

        default:
            break;
    }

    return ret;
}

/**
  \brief       link DMA channel to i2s device
  \param[in]   i2s  i2s handle to operate.
  \param[in]   rx_dma the DMA channel  for receive, when it is NULL means to unused dma.
  \return      error code
*/
csi_error_t csi_i2s_rx_link_dma(csi_i2s_t *i2s, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    if (rx_dma != NULL) {
        rx_dma->parent = i2s;
        ret = csi_dma_ch_alloc(rx_dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(rx_dma, wj_i2s_dma_event_cb, NULL);
            i2s->rx_dma = rx_dma;
        } else {
            rx_dma->parent = NULL;
            ret = CSI_ERROR;
        }
    } else {
        if (i2s->rx_dma) {
            csi_dma_ch_detach_callback(i2s->rx_dma);
            csi_dma_ch_free(i2s->rx_dma);
            i2s->rx_dma = NULL;
        } else {
            ret = CSI_ERROR;
        }
    }

    return ret;
}

/**
  \brief       link DMA channel to i2s device
  \param[in]   i2s  i2s handle to operate.
  \param[in]   rx_dma the DMA channel  for receive, when it is NULL means to unused dma.
  \return      error code
*/
csi_error_t csi_i2s_tx_link_dma(csi_i2s_t *i2s, csi_dma_ch_t *tx_dma)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    if (tx_dma != NULL) {
        tx_dma->parent = i2s;
        ret = csi_dma_ch_alloc(tx_dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(tx_dma, wj_i2s_dma_event_cb, NULL);
            i2s->tx_dma = tx_dma;
        } else {
            tx_dma->parent = NULL;
            ret = CSI_ERROR;
        }
    } else {
        if (i2s->tx_dma) {
            csi_dma_ch_detach_callback(i2s->tx_dma);
            csi_dma_ch_free(i2s->tx_dma);
            i2s->tx_dma = NULL;
        } else {
            ret = CSI_ERROR;
        }
    }

    return ret;
}

/**
  \brief       I2S rx cache config
  \param[in]   i2s i2s handle to operate.
  \param[in]   buffer i2s rx cache
  \return      none
*/
void csi_i2s_rx_set_buffer(csi_i2s_t *i2s, ringbuffer_t *buffer)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    CSI_PARAM_CHK_NORETVAL(buffer);
    i2s->rx_buf = buffer;
}

/**
  \brief       I2S tx cache config
  \param[in]   i2s i2s handle to operate.
  \param[in]   buffer i2s tx cache
  \return      none
*/
void csi_i2s_tx_set_buffer(csi_i2s_t *i2s, ringbuffer_t *buffer)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    CSI_PARAM_CHK_NORETVAL(buffer);
    i2s->tx_buf = buffer;
}

/**
  \brief       I2S rx set period.The value of period is to report a receive completion event
  \            after each period value data is received.
  \param[in]   i2s i2s handle to operate.
  \param[in]   buffer i2s rx cache
  \return      none
*/
csi_error_t csi_i2s_rx_set_period(csi_i2s_t *i2s, uint32_t period)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    if (period == 0U) {
        ret = CSI_ERROR;
    } else {
        if ((i2s->rx_buf->size % period) != 0U) {
            ret = CSI_ERROR;
        } else {
            i2s->rx_period = period;
        }
    }

    return ret;
}

/**
  \brief       I2S tx set period.The value of period is to report a receive completion event
  \            after each period value data is send.
  \param[in]   i2s i2s handle to operate.
  \param[in]   buffer i2s tx cache
  \return      none
*/
csi_error_t csi_i2s_tx_set_period(csi_i2s_t *i2s, uint32_t period)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    if (period == 0U) {
        ret = CSI_ERROR;
    } else {
        if ((i2s->tx_buf->size % period) != 0U) {
            ret = CSI_ERROR;
        } else {
            i2s->tx_period = period;
        }
    }

    return ret;
}

/**
  \brief  Get rx ringbuffer cache free space
   \param[in]   i2s i2s handle to operate.
  \return buffer free space (bytes)
*/
uint32_t csi_i2s_rx_buffer_avail(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, 0U);
    return ringbuffer_avail(i2s->rx_buf);
}

/**
  \brief  Reset the rx ringbuffer, discard all data in the cache
  \param[in]   i2s i2s handle to operate.
  \return      error code
*/
csi_error_t csi_i2s_rx_buffer_reset(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_ERROR;
    ringbuffer_reset(i2s->rx_buf);

    if (i2s->rx_buf->buffer != NULL) {
        memset(i2s->rx_buf->buffer, 0, i2s->rx_buf->size);
    }

    if ((i2s->rx_buf->read == 0U) && (i2s->rx_buf->write == 0U)) {
        ret = CSI_OK;
    }

    return ret;
}

/**
  \brief  Get tx ringbuffer cache free space
   \param[in]   i2s i2s handle to operate.
  \return buffer free space (bytes)
*/
uint32_t csi_i2s_tx_buffer_avail(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, 0U);
    return ringbuffer_avail(i2s->tx_buf);
}

/**
  \brief  Reset the tx ringbuffer, discard all data in the cache
  \param[in]   i2s i2s handle to operate.
  \return      error code
*/
csi_error_t csi_i2s_tx_buffer_reset(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_ERROR;
    ringbuffer_reset(i2s->tx_buf);

    if (i2s->tx_buf->buffer != NULL) {
        memset(i2s->tx_buf->buffer, 0, i2s->tx_buf->size);
    }

    if ((i2s->tx_buf->read == 0U) && (i2s->tx_buf->write == 0U)) {
        ret = CSI_OK;
    }

    return ret;
}
/**
  \brief  Receive an amount of data to cache in blocking mode.
  \param[in]   i2s   operate handle.
  \param[in]   data  save receive data.
  \param[in]   size  receive data size.
  \param[in]   timeout  is the number of queries, not time
  \return      The size of data receive successfully
*/
int32_t csi_i2s_receive(csi_i2s_t *i2s, void *data, uint32_t size)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    int32_t received_size = 0;
    uint8_t *read_data = (void *)data;
    int32_t read_size = 0;

    if (i2s->rx_dma == NULL) {
        received_size = i2s_receive_polling(i2s, read_data, size);

        if (received_size == (int32_t)size) {
            read_size = (int32_t)size;
        } else {
            read_size = CSI_ERROR;
        }

    } else {

        while (1) {
            read_size += (int32_t)ringbuffer_out(i2s->rx_buf, (void *)(read_data + (uint32_t)read_size), (size - (uint32_t)read_size));

            if ((size - (uint32_t)read_size) <= 0U) {
                break;
            }
        }

    }


    return read_size;
}

/**
  \brief Write data to the cache.
  \With asynchronous sending,
  \the data is first written to the cache and then output through the I2S interface.
  \This function does not block, and the return value is the number
  \of data that was successfully written to the cache.
  \param[in]   i2s   operate handle.
  \param[in]   data  send data.
  \param[in]   size  receive data size.
  \return      The data size that write to cache
*/
uint32_t csi_i2s_send_async(csi_i2s_t *i2s, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(i2s, 0U);
    CSI_PARAM_CHK(data, 0U);
    uint32_t write_len;

    uint32_t result = csi_irq_save();
    write_len = ringbuffer_in(i2s->tx_buf, data, size);
    csi_irq_restore(result);

    if ((uint8_t *)i2s->priv) { ///< if dma is stop, then start it
        wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;
        uint32_t tx_num = 0U;
        uint32_t TX_FIFO[16];
        uint32_t send_fifo_level = 0U;
        uint32_t emptyfifo = 0U;
        send_fifo_level = wj_i2s_get_transmit_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);
        emptyfifo = (16U > (I2S_MAX_FIFO - send_fifo_level)) ? (I2S_MAX_FIFO - send_fifo_level) : 16U;
        memcpy(TX_FIFO, i2s->tx_buf->buffer + i2s->tx_buf->read, emptyfifo << 2U);

        for (tx_num = 0U; tx_num < emptyfifo; tx_num ++) {
            wj_i2s_transmit_data((wj_i2s_regs_t *)i2s->dev.reg_base, TX_FIFO[tx_num]);
        }

        wj_i2s_get_transmit_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);
        soc_dcache_clean_invalid_range((unsigned long)(i2s->tx_buf->buffer + i2s->tx_buf->read + (tx_num << 2U)), i2s->tx_period - (tx_num << 2U));
        csi_dma_ch_start(i2s->tx_dma, i2s->tx_buf->buffer + i2s->tx_buf->read + (tx_num << 2U), (void *) & (i2s_base->I2S_DR), i2s->tx_period - (tx_num << 2U));
        i2s->priv = (void *)0U;
    }

    return write_len;
}

/**
  \brief  Transmits an amount of data to cache in blocking mode.
  \param[in]   i2s   operate handle.
  \param[in]   data  send data.
  \param[in]   size  receive data size.
  \param[in]   timeout  is the number of queries, not time
  \return      The num of data witch is send successful
*/
int32_t csi_i2s_send(csi_i2s_t *i2s, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    int32_t sent_size = 0;
    int32_t write_size = 0;
    uint8_t *send_data = (void *)data;

    if (i2s->tx_dma == NULL) {
        sent_size = i2s_send_polling(i2s, send_data, size);

        if (sent_size == (int32_t)size) {
            write_size = sent_size;
        } else {
            write_size = CSI_ERROR;
        }

    } else {
        while (1) {
            write_size += (int32_t)ringbuffer_in(i2s->tx_buf, (void *)(send_data + (uint32_t)write_size), (size - (uint32_t)write_size));

            if ((uint8_t *)i2s->priv) { ///< if dma is stop, then start it
                wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;
                uint32_t tx_num = 0U;
                uint32_t TX_FIFO[10];
                uint32_t send_fifo_level = 0U;
                uint32_t emptyfifo = 0U;
                send_fifo_level = wj_i2s_get_transmit_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);
                emptyfifo = (10U > (I2S_MAX_FIFO - send_fifo_level)) ? (I2S_MAX_FIFO - send_fifo_level) : 10U;
                memcpy(TX_FIFO, i2s->tx_buf->buffer + i2s->tx_buf->read, emptyfifo << 2U);

                for (tx_num = 0U; tx_num < emptyfifo; tx_num ++) {
                    wj_i2s_transmit_data((wj_i2s_regs_t *)i2s->dev.reg_base, TX_FIFO[tx_num]);
                }

                wj_i2s_get_transmit_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);
                soc_dcache_clean_invalid_range((unsigned long)(i2s->tx_buf->buffer + i2s->tx_buf->read + (tx_num << 2U)), i2s->tx_period - (tx_num << 2U));
                csi_dma_ch_start(i2s->tx_dma, i2s->tx_buf->buffer + i2s->tx_buf->read + (tx_num << 2U), (void *) & (i2s_base->I2S_DR), i2s->tx_period - (tx_num << 2U));
                i2s->priv = (void *)0U;
            }

            if ((size - (uint32_t)write_size) <= 0U) {
                break;
            }
        }

        while (!ringbuffer_is_empty(i2s->tx_buf));
    }

    return write_size;
}

/**
  \brief Read data from the cache.
  \Using asynchronous receive, i2s writes the received data to the cache.
  \This function reads data from the cache, returns the number of successful reads,
  \and returns 0 if there is no data in the cache.
  \param[in]   i2s   operate handle.
  \param[in]   data  the buf save receive data.
  \param[in]   size  receive data size.
  \return      The size of data read successfully
*/
uint32_t csi_i2s_receive_async(csi_i2s_t *i2s, void *data, uint32_t size)
{
    CSI_PARAM_CHK(i2s, 0U);
    CSI_PARAM_CHK(data, 0U);
    uint32_t read_len;

    uint32_t result = csi_irq_save();
    read_len = ringbuffer_out(i2s->rx_buf, (void *)data, size);
    csi_irq_restore(result);
    return read_len;
}

/**
  \brief       start i2s pause asynchronous send
  \param[in]   i2s  operate handle.
  \return      error code
*/
csi_error_t csi_i2s_send_pause(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_dma_ch_stop(i2s->tx_dma);
    i2s->state.writeable = 0U;
    return ret;
}

/**
  \brief       start i2s resume asynchronous send
  \param[in]   i2s  operate handle.
  \return      error code
*/
csi_error_t csi_i2s_send_resume(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;
    uint32_t tx_num = 0U;
    uint32_t TX_FIFO[16];
    uint32_t send_fifo_level = 0U;
    uint32_t emptyfifo = 0U;
    send_fifo_level = wj_i2s_get_transmit_fifo_level((wj_i2s_regs_t *)i2s->dev.reg_base);

    if (send_fifo_level < 9U) {
        emptyfifo = (16U > (I2S_MAX_FIFO - send_fifo_level)) ? (I2S_MAX_FIFO - send_fifo_level) : 16U;
        memcpy(TX_FIFO, i2s->tx_buf->buffer + i2s->tx_buf->read, emptyfifo << 2U);

        for (tx_num = 0U; tx_num < emptyfifo; tx_num ++) {
            wj_i2s_transmit_data((wj_i2s_regs_t *)i2s->dev.reg_base, TX_FIFO[tx_num]);
        }

        soc_dcache_clean_invalid_range((unsigned long)(i2s->tx_buf->buffer + i2s->tx_buf->read + (tx_num << 2U)), i2s->tx_period - (tx_num << 2U));
        csi_dma_ch_start(i2s->tx_dma, i2s->tx_buf->buffer + i2s->tx_buf->read + (tx_num << 2U), (void *) & (i2s_base->I2S_DR), i2s->tx_period - (tx_num << 2U));
    } else {
        soc_dcache_clean_invalid_range((unsigned long)(i2s->tx_buf->buffer + i2s->tx_buf->read), i2s->tx_period);
        csi_dma_ch_start(i2s->tx_dma, (i2s->tx_buf->buffer + i2s->tx_buf->read), (void *) & (i2s_base->I2S_DR), i2s->tx_period);
        wj_i2s_get_transmit_fifo_level(i2s_base);
    }

    i2s->state.writeable = 1U;
    return ret;
}

/**
  \brief       start i2s asynchronous send
  \param[in]   i2s  operate handle.
  \return      error code
*/
csi_error_t csi_i2s_send_start(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_dma_ch_config_t config;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)i2s->tx_dma;

    wj_i2s_disable(i2s_base);
    wj_i2s_set_transmit_mode(i2s_base);
    wj_i2s_set_transmit_dma_enable(i2s_base);
    wj_i2s_set_transmit_dma_data_num_level(i2s_base, 8U);

    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_tw = DMA_DATA_WIDTH_32_BITS;
    config.dst_tw = DMA_DATA_WIDTH_32_BITS;
    /* config for wj_dma */
    config.group_len = 8U;
    config.trans_dir = DMA_MEM2PERH;
    config.half_int_en = 0U;
    /* config for etb */
    config.handshake = i2s_tx_hs_num[i2s->dev.idx];

    wj_i2s_enable(i2s_base);
    memset(i2s->tx_buf->buffer, 0, i2s->tx_buf->size);
    ret = csi_dma_ch_config(dma_ch, &config);
    soc_dcache_clean_invalid_range((unsigned long)(i2s->tx_buf->buffer + i2s->tx_buf->read), i2s->tx_period);
    csi_dma_ch_start(i2s->tx_dma, i2s->tx_buf->buffer + i2s->tx_buf->read, (void *) & (i2s_base->I2S_DR), i2s->tx_period);
    wj_i2s_clear_fifo(i2s);
    i2s->state.writeable = 1U;

    return ret;
}

/**
  \brief       start i2s asynchronous receive
  \param[in]   i2s  operate handle.
  \return      error code
*/
csi_error_t csi_i2s_receive_start(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csi_dma_ch_config_t config;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)i2s->rx_dma;

    wj_i2s_disable(i2s_base);
    wj_i2s_set_receive_mode(i2s_base);
    wj_i2s_set_receive_dma_enable(i2s_base);
    wj_i2s_set_receive_dma_data_num_level(i2s_base, 24U);

    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw = DMA_DATA_WIDTH_32_BITS;
    config.dst_tw = DMA_DATA_WIDTH_32_BITS;
    /* config for wj_dma */
    config.group_len = 8U;
    config.trans_dir = DMA_PERH2MEM;
    config.half_int_en = 0U;
    /* config for etb */
    config.handshake = i2s_rx_hs_num[i2s->dev.idx];

    wj_i2s_enable(i2s_base);

    memset(i2s->rx_buf->buffer, 0, i2s->rx_buf->size);
    ret = csi_dma_ch_config(dma_ch, &config);

    uint32_t num = wj_i2s_get_receive_fifo_level(i2s_base);

    for (uint8_t i = 0U; i < num; i++) {
        wj_i2s_receive_data(i2s_base);
    }

    soc_dcache_clean_invalid_range((unsigned long)(i2s->rx_buf->buffer + i2s->rx_buf->write), i2s->rx_period);
    csi_dma_ch_start(i2s->rx_dma, (void *) & (i2s_base->I2S_DR), i2s->rx_buf->buffer + i2s->rx_buf->write, i2s->rx_period);
    i2s->state.readable = 1U;
    return ret;
}

/**
  \brief       stop i2s asynchronous send
  \param[in]   i2s  operate handle.
*/
void csi_i2s_send_stop(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    wj_i2s_regs_t *i2s_base = (wj_i2s_regs_t *)i2s->dev.reg_base;
    csi_dma_ch_stop(i2s->tx_dma);
    ringbuffer_reset(i2s->tx_buf);
    memset(i2s->tx_buf->buffer, 0, i2s->tx_buf->size);

    while (wj_i2s_get_transmit_fifo_level(i2s_base));

    wj_i2s_clear_fifo(i2s);
    i2s->state.writeable = 0U;
}

/**
  \brief       stop i2s asynchronous receive
  \param[in]   i2s  operate handle.
  \return      error code
*/
void csi_i2s_receive_stop(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    csi_dma_ch_stop(i2s->rx_dma);
    ringbuffer_reset(i2s->rx_buf);
    memset(i2s->rx_buf->buffer, 0, i2s->rx_buf->size);
    wj_i2s_clear_fifo(i2s);
    i2s->state.readable = 0U;
}

/**
  \brief       attach the callback handler to i2s
  \param[in]   i2s  operate handle.
  \param[in]   cb    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_i2s_attach_callback(csi_i2s_t *i2s, void *callback, void *arg)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    i2s->callback = callback;
    i2s->arg = arg;
    return ret;
}

/**
  \brief       detach the callback handler
  \param[in]   i2s  operate handle.
  \return      none
*/
void csi_i2s_detach_callback(csi_i2s_t *i2s)
{
    CSI_PARAM_CHK_NORETVAL(i2s);
    i2s->callback = NULL;
    i2s->arg = NULL;
}

/**
  \brief       Get i2s status.
  \param[in]   i2s i2s handle to operate.
  \param[out]  state i2s state.
  \return      i2s error code
*/
csi_error_t csi_i2s_get_state(csi_i2s_t *i2s, csi_state_t *state)
{
    CSI_PARAM_CHK(i2s, CSI_ERROR);
    CSI_PARAM_CHK(state, CSI_ERROR);
    *state = i2s->state;
    return CSI_OK;
}

#ifdef CONFIG_PM
csi_error_t csi_i2s_enable_pm(csi_i2s_t *i2s)
{
    return CSI_UNSUPPORTED;
}

void csi_i2s_disable_pm(csi_i2s_t *i2s)
{

}
#endif

/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_spi.c
 * @brief
 * @version
 * @date     2020-02-11
 ******************************************************************************/

#include <string.h>

#include <drv/spi.h>
#include <drv/irq.h>
#include <drv/tick.h>
#include <drv/porting.h>

#include "dw_spi_ll.h"

#define DW_MAX_SPI_TXFIFO_LV       0x20U
#define DW_MAX_SPI_RXFIFO_LV       0x20U
#define DW_DEFAULT_SPI_TXFIFO_LV   0x8U
#define DW_DEFAULT_SPI_RXFIFO_LV   0x10U

#define DW_DEFAULT_TRANSCATION_TIMEOUT 200U

#define IS_8BIT_FRAME_LEN(spi)   (( uint32_t  )spi->priv <= 8U)
#define IS_16BIT_FRAME_LEN(spi)  (( ( uint32_t  )spi->priv > 8U  ) && ( ( uint32_t  )spi->priv <= 16U  ))

extern uint16_t spi_tx_hs_num[];
extern uint16_t spi_rx_hs_num[];

static csi_error_t dw_spi_send_intr(csi_spi_t *spi, const void *data, uint32_t size);
static csi_error_t dw_spi_receive_intr(csi_spi_t *spi, void *data, uint32_t size);
static csi_error_t dw_spi_send_receive_intr(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t num);

static uint8_t find_max_prime_num(uint32_t num, uint32_t limit)
{
    uint32_t i, min;

    min = (num > limit) ? limit : num;
    i = min;

    while (i > 0U) {
        if (!(num % i)) {
            break;
        }

        i--;
    }

    if (i == 0U) {
        i = min;
    }

    return (uint8_t)i;
}

static uint8_t find_group_len(uint32_t size, uint8_t width)
{
    uint8_t  prime_num;
    uint32_t limit;

    limit = 8U;

    do {
        prime_num = find_max_prime_num(size, limit);
        limit = prime_num - 1U;
    } while ((prime_num % width) != 0U);

    return prime_num;
}

static csi_error_t wait_ready_until_timeout(csi_spi_t *spi, uint32_t timeout)
{
    uint32_t timestart = 0U;
    csi_error_t    ret = CSI_OK;
    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    timestart = csi_tick_get_ms();

    while (dw_spi_get_status(spi_base) & DW_SPI_SR_BUSY) {
        if ((csi_tick_get_ms() - timestart) > timeout) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}

static void process_end_transcation(csi_spi_t *spi)
{
    uint32_t mode;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    mode = dw_spi_get_transfer_mode(spi_base);

    /* process end of transmit */
    if ((mode & DW_SPI_CTRLR0_TMOD_Msk) ==  DW_SPI_CTRLR0_TMOD_TX) {
        if (spi->tx_size == 0U) {
            if ((dw_spi_get_status(spi_base) & DW_SPI_SR_BUSY) == 0U) {
                dw_spi_disable_tx_empty_irq(spi_base);
                dw_spi_config_tx_fifo_threshold(spi_base, 0U);
                spi->state.writeable = 1U;

                if (spi->callback) {
                    spi->callback(spi, SPI_EVENT_SEND_COMPLETE, spi->arg);
                }

            }
        }
    }

    /* process end of receive */
    else if ((mode & DW_SPI_CTRLR0_TMOD_Msk) == DW_SPI_CTRLR0_TMOD_RX) {
        if (spi->rx_size == 0U) {
            if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
                dw_spi_disable_rx_fifo_full_irq(spi_base);
                dw_spi_config_rx_data_len(spi_base, 0U);
                dw_spi_config_rx_fifo_threshold(spi_base, 0U);
            }

            if (spi->callback) {
                spi->callback(spi, SPI_EVENT_RECEIVE_COMPLETE, spi->arg);
            }

            spi->state.readable = 1U;
        }
    }

    /* process end of transmit & receive */
    else if ((mode & DW_SPI_CTRLR0_TMOD_Msk) == DW_SPI_CTRLR0_TMOD_TX_RX) {
        if ((spi->rx_size == 0U) && (spi->tx_size == 0U)) {
            dw_spi_disable_tx_empty_irq(spi_base);
            dw_spi_disable_rx_fifo_full_irq(spi_base);
            dw_spi_config_tx_fifo_threshold(spi_base, 0U);
            dw_spi_config_rx_fifo_threshold(spi_base, 0U);
            spi->state.readable  = 1U;
            spi->state.writeable = 1U;

            if (spi->callback) {
                spi->callback(spi, SPI_EVENT_SEND_RECEIVE_COMPLETE, spi->arg);
            }
        } else if (spi->tx_size == 0U) {
            // reduce interrupt times
            dw_spi_disable_tx_empty_irq(spi_base);
        }
    }
}

static void spi_intr_tx_fifo_empty(csi_spi_t *spi)
{
    uint32_t remain_fifo;
    uint32_t value;
    uint32_t frame_len;
    uint8_t  *tx_data;
    uint32_t tx_size;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    remain_fifo = DW_MAX_SPI_TXFIFO_LV - dw_spi_get_tx_fifo_level(spi_base);
    frame_len   = dw_spi_get_data_frame_len(spi_base);

    /* process end of transcation */
    process_end_transcation(spi);

    /* transfer loop */
    tx_data = spi->tx_data;
    tx_size = spi->tx_size;

    if (frame_len <= 8U) {
        while (tx_size && remain_fifo) {
            /* process 4~8bit frame len */
            value = (uint32_t)(*(uint8_t *)tx_data);
            tx_data += sizeof(uint8_t);
            dw_spi_transmit_data(spi_base, value);
            remain_fifo--;
            tx_size--;
        }
    } else if ((frame_len > 8U) && (frame_len <= 16U)) {
        while (tx_size && remain_fifo) {
            /* process 8~16bit frame len */
            value = (uint32_t)(*(uint16_t *)tx_data);
            tx_data += sizeof(uint16_t);
            dw_spi_transmit_data(spi_base, value);
            remain_fifo--;
            tx_size--;
        }
    }

    spi->tx_data = tx_data;
    spi->tx_size = tx_size;
}

static void spi_intr_rx_fifo_full(csi_spi_t *spi)
{
    uint32_t fifo_size;
    uint32_t frame_len;
    uint8_t  *rx_data;
    uint32_t rx_size;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    fifo_size = dw_spi_get_rx_fifo_level(spi_base);
    frame_len = dw_spi_get_data_frame_len(spi_base);
    rx_data = spi->rx_data;
    rx_size = spi->rx_size;

    /* transfer loop */
    if (frame_len <= 8U) {
        while (rx_size && fifo_size) {
            *(uint8_t *)rx_data = (uint8_t)dw_spi_receive_data(spi_base);
            rx_data += sizeof(uint8_t);
            fifo_size--;
            rx_size--;
        }
    } else if ((frame_len > 8U) && (frame_len <= 16U)) {
        while (rx_size && fifo_size) {
            *(uint16_t *)rx_data = (uint16_t)dw_spi_receive_data(spi_base);
            rx_data += sizeof(uint16_t);
            fifo_size--;
            rx_size--;
        }
    }

    /* update rx fifo threshold when remain size less then default threshold*/
    if ((rx_size < (DW_DEFAULT_SPI_RXFIFO_LV + 1U)) && (rx_size > 0U)) {
        dw_spi_config_rx_fifo_threshold(spi_base, rx_size - 1U);
    }

    spi->rx_data = rx_data;
    spi->rx_size = rx_size;

    /* process end of transcation */
    process_end_transcation(spi);

}

static void dw_spi_irqhandler(void *args)
{
    uint32_t status;
    csi_spi_t *spi = (csi_spi_t *)args;
    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    status = dw_spi_get_interrupt_status(spi_base);

    /* process receive fifo full interrupt */
    if (status & DW_SPI_ISR_RXFIS) {
        spi_intr_rx_fifo_full(spi);
    }


    /* process transmit fifo empty interrupt */
    if (status & DW_SPI_ISR_TXEIS) {
        spi_intr_tx_fifo_empty(spi);
    }

    /* process Multi-Master contention interrupt */
    if (status & DW_SPI_ISR_MSTIS) {
        dw_spi_clr_multi_master_irq(spi_base);

        if (spi->callback) {
            spi->callback(spi, SPI_EVENT_ERROR, spi->arg);
        }
    }

    /* process receive fifo overflow interrupt */
    if (status & DW_SPI_ISR_RXOIS) {
        dw_spi_clr_rx_fifo_overflow_irq(spi_base);

        if (spi->callback) {
            spi->callback(spi, SPI_EVENT_ERROR_OVERFLOW, spi->arg);
        }
    }

    /* process transmit fifo overflow interrupt */
    if (status & DW_SPI_ISR_TXOIS) {
        dw_spi_clr_tx_fifo_overflow_irq(spi_base);

        if (spi->callback) {
            spi->callback(spi, SPI_EVENT_ERROR_OVERFLOW, spi->arg);
        }
    }

    /* process receive fifo underflow interrupt */
    if (status & DW_SPI_ISR_RXUIS) {
        dw_spi_clr_rx_fifo_underflow_irq(spi_base);

        if (spi->callback) {
            spi->callback(spi, SPI_EVENT_ERROR_UNDERFLOW, spi->arg);
        }
    }
}

static void dw_spi_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    dw_spi_regs_t *spi_base;
    csi_spi_t *spi = (csi_spi_t *)dma->parent;
    uint32_t mode;

    spi_base = (dw_spi_regs_t *)spi->dev.reg_base;
    mode = dw_spi_get_transfer_mode(spi_base);

    if (event == DMA_EVENT_TRANSFER_DONE) {
        /* process end of transmit */
        if ((spi->tx_dma != NULL) && (spi->tx_dma->ch_id == dma->ch_id)) {
            csi_dma_ch_stop(dma);
            dw_spi_disable_tx_dma(spi_base);

            if (wait_ready_until_timeout(spi, DW_DEFAULT_TRANSCATION_TIMEOUT) == CSI_OK) {

                spi->state.writeable = 1U;
                spi->tx_size = 0U;

                if ((mode & DW_SPI_CTRLR0_TMOD_Msk) == DW_SPI_CTRLR0_TMOD_TX) {
                    dw_spi_config_dma_tx_data_level(spi_base, 0U);

                    if (spi->callback) {
                        spi->callback(spi, SPI_EVENT_SEND_COMPLETE, spi->arg);
                    }
                } else {
                    if (spi->state.readable == 1U) {
                        spi->callback(spi, SPI_EVENT_SEND_RECEIVE_COMPLETE, spi->arg);
                    }
                }
            }
        } else if ((spi->rx_dma != NULL) && (spi->rx_dma->ch_id == dma->ch_id)) {
            csi_dma_ch_stop(dma);
            dw_spi_disable_rx_dma(spi_base);
            dw_spi_config_dma_rx_data_level(spi_base, 0U);
            dw_spi_config_rx_data_len(spi_base, 0U);

            spi->state.readable = 1U;
            spi->rx_size = 0U;

            if ((mode & DW_SPI_CTRLR0_TMOD_Msk) == DW_SPI_CTRLR0_TMOD_RX) {
                if (spi->callback) {
                    spi->callback(spi, SPI_EVENT_RECEIVE_COMPLETE, spi->arg);
                }
            } else {
                if (spi->state.writeable == 1U) {
                    spi->callback(spi, SPI_EVENT_SEND_RECEIVE_COMPLETE, spi->arg);
                }
            }
        }
    }
}

csi_error_t csi_spi_init(csi_spi_t *spi, uint32_t idx)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    dw_spi_regs_t *spi_base;
    csi_error_t ret = CSI_OK;

    if (target_get(DEV_DW_SPI_TAG, idx, &spi->dev) != CSI_OK) {
        ret = CSI_ERROR;
    } else {
        spi->state.writeable = 1U;
        spi->state.readable  = 1U;
        spi->state.error     = 0U;
        spi->send            = NULL;
        spi->receive         = NULL;
        spi->send_receive    = NULL;
        spi->rx_dma          = NULL;
        spi->tx_dma          = NULL;
        spi->rx_data         = NULL;
        spi->tx_data         = NULL;
        spi->callback        = NULL;
        spi->arg             = NULL;

        spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

        dw_spi_disable_all_irq(spi_base);
        dw_spi_disable(spi_base);
        spi->priv = (void *)dw_spi_get_data_frame_len(spi_base);
    }

    return ret;
}

void csi_spi_uninit(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    /* reset all registers */
    dw_spi_reset_regs(spi_base);

    /* unregister irq */
    csi_irq_disable((uint32_t)spi->dev.irq_num);
    csi_irq_detach((uint32_t)spi->dev.irq_num);
}

csi_error_t csi_spi_attach_callback(csi_spi_t *spi, void *callback, void *arg)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);

    spi->callback     = callback;
    spi->arg          = arg;
    spi->send         = NULL;
    spi->receive      = NULL;
    spi->send_receive = NULL;

    return CSI_OK;
}


void csi_spi_detach_callback(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);

    spi->callback     = NULL;
    spi->arg          = NULL;
    spi->send         = NULL;
    spi->receive      = NULL;
    spi->send_receive = NULL;
}


csi_error_t csi_spi_mode(csi_spi_t *spi, csi_spi_mode_t mode)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    dw_spi_regs_t *spi_base;
    csi_error_t   ret = CSI_OK;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    /* configure spi mode */
    switch (mode) {
        case SPI_MASTER:
            dw_spi_set_master_mode(spi_base);
            break;

        case SPI_SLAVE:
            dw_spi_set_slave_mode(spi_base);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_spi_cp_format(csi_spi_t *spi, csi_spi_cp_format_t format)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    dw_spi_regs_t *spi_base;
    csi_error_t   ret = CSI_OK;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    /* configure spi format */
    switch (format) {
        case SPI_FORMAT_CPOL0_CPHA0:
            dw_spi_set_cpol0(spi_base);
            dw_spi_set_cpha0(spi_base);
            break;

        case SPI_FORMAT_CPOL0_CPHA1:
            dw_spi_set_cpol0(spi_base);
            dw_spi_set_cpha1(spi_base);
            break;

        case SPI_FORMAT_CPOL1_CPHA0:
            dw_spi_set_cpol1(spi_base);
            dw_spi_set_cpha0(spi_base);
            break;

        case SPI_FORMAT_CPOL1_CPHA1:
            dw_spi_set_cpol1(spi_base);
            dw_spi_set_cpha1(spi_base);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

uint32_t csi_spi_baud(csi_spi_t *spi, uint32_t baud)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(baud, CSI_ERROR);

    dw_spi_regs_t *spi_base;
    uint32_t div;
    uint32_t freq = 0U;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    dw_spi_config_sclk_clock(spi_base, soc_get_spi_freq((uint32_t)spi->dev.idx), baud);

    div = dw_spi_get_sclk_clock_div(spi_base);

    if (div > 0U) {
        freq =  soc_get_spi_freq((uint32_t)spi->dev.idx) / div;
    }

    return freq;

}

csi_error_t csi_spi_frame_len(csi_spi_t *spi, csi_spi_frame_len_t length)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    dw_spi_regs_t *spi_base;
    csi_error_t ret = CSI_OK;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    if ((length < SPI_FRAME_LEN_4) || (length > SPI_FRAME_LEN_16)) {
        ret = CSI_ERROR;
    } else {

        /* configura data frame width*/
        dw_spi_config_data_frame_len(spi_base, (uint32_t)length);
        spi->priv = (void *)dw_spi_get_data_frame_len(spi_base);
    }


    return ret;
}

int32_t csi_spi_send(csi_spi_t *spi, const void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    uint32_t value;
    uint32_t timestart;
    uint32_t count = 0U;
    uint8_t *tx_data;
    uint32_t current_size;
    int32_t  ret   = CSI_OK;
    dw_spi_regs_t *spi_base;

    spi_base  = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    do {
        if ((spi->state.writeable == 0U) || (spi->state.readable == 0U)) {
            ret = CSI_BUSY;
            break;
        }

        if (IS_16BIT_FRAME_LEN(spi)) {
            if (size % sizeof(uint16_t)) {
                ret = CSI_ERROR;
                break;
            }
        }

        timestart = csi_tick_get_ms();
        spi->state.writeable = 0U;
        tx_data = (uint8_t *)data;

        // Convert byte to nums
        if (IS_16BIT_FRAME_LEN(spi)) {
            size /= 2U;
        }

        /* set tx mode */
        dw_spi_disable(spi_base);
        dw_spi_set_tx_mode(spi_base);
        dw_spi_config_tx_fifo_threshold(spi_base, DW_DEFAULT_SPI_TXFIFO_LV);
        dw_spi_enable(spi_base);

        /* transfer loop */
        if (IS_8BIT_FRAME_LEN(spi)) {
            while (size > 0U) {
                current_size = DW_MAX_SPI_TXFIFO_LV - dw_spi_get_tx_fifo_level(spi_base);

                if (current_size > size) {
                    current_size = size;

                }

                while (current_size--) {
                    value = (uint32_t)(*(uint8_t *)tx_data);
                    dw_spi_transmit_data(spi_base, value);
                    tx_data += 1;
                    count += 1U;
                    size--;
                }

                if ((csi_tick_get_ms() - timestart) > timeout) {
                    break;
                }
            }
        }

        if (IS_16BIT_FRAME_LEN(spi)) {
            while (size > 0U) {
                current_size = DW_MAX_SPI_TXFIFO_LV - dw_spi_get_tx_fifo_level(spi_base);

                if (current_size > size) {
                    current_size = size;

                }

                while (current_size--) {
                    value = (uint32_t)(*(uint16_t *)tx_data);
                    dw_spi_transmit_data(spi_base, value);
                    tx_data += 2;
                    count += 2U;
                    size--;
                }

                if ((csi_tick_get_ms() - timestart) > timeout) {
                    break;
                }
            }
        }

        // Check SR.TFE is necessary when tx size = 1, because SR.BUSY has some delay before be vaild
        while (!(dw_spi_get_status(spi_base) & DW_SPI_SR_TFE)) {
            if ((csi_tick_get_ms() - timestart) > timeout) {
                break;
            }
        }

        while ((dw_spi_get_status(spi_base) & DW_SPI_SR_BUSY)) {
            if ((csi_tick_get_ms() - timestart) > timeout) {
                break;
            }
        }
    } while (0);

    /* close spi */
    dw_spi_config_tx_fifo_threshold(spi_base, 0U);
    spi->state.writeable = 1U;

    if (ret >= 0) {
        ret = (int32_t)count;
    }

    return ret;
}

csi_error_t csi_spi_send_async(csi_spi_t *spi, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if ((spi->state.writeable == 0U) || (spi->state.readable == 0U)) {
        ret = CSI_BUSY;
    }

    if (IS_16BIT_FRAME_LEN(spi)) {
        if (size % sizeof(uint16_t)) {
            ret = CSI_ERROR;
        }
    }

    if ((ret == CSI_OK) && (spi->callback != NULL)) {
        if (spi->send) {
            spi->state.writeable = 0U;
            ret = spi->send(spi, data, size);
        } else {
            spi->state.writeable = 0U;
            csi_irq_attach((uint32_t)spi->dev.irq_num, &dw_spi_irqhandler, &spi->dev);
            csi_irq_enable((uint32_t)spi->dev.irq_num);
            ret = dw_spi_send_intr(spi, data, size);
        }
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}

static csi_error_t dw_spi_send_intr(csi_spi_t *spi, const void *data, uint32_t size)
{
    csi_error_t ret = CSI_OK;
    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    spi->tx_data = (uint8_t *)data;

    do {
        // Convert byte to nums
        if (IS_16BIT_FRAME_LEN(spi)) {
            spi->tx_size = size / 2U;
        } else if (IS_8BIT_FRAME_LEN(spi)) {
            spi->tx_size = size;
        } else {
            ret = CSI_ERROR;
            break;
        }

        /* set tx mode*/
        dw_spi_disable(spi_base);
        dw_spi_set_tx_mode(spi_base);
        dw_spi_config_tx_fifo_threshold(spi_base, DW_DEFAULT_SPI_TXFIFO_LV);
        dw_spi_enable(spi_base);
        dw_spi_enable_tx_empty_irq(spi_base);
    } while (0);

    return ret;
}

static csi_error_t dw_spi_send_dma(csi_spi_t *spi, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    csi_dma_ch_config_t config;
    dw_spi_regs_t       *spi_base;
    csi_dma_ch_t        *dma_ch;
    csi_error_t         ret = CSI_OK;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    dma_ch   = (csi_dma_ch_t *)spi->tx_dma;
    spi->tx_data = (uint8_t *)data;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));

    do {
        /* configure dma channel */
        if (IS_16BIT_FRAME_LEN(spi)) {
            spi->tx_size = size / 2U;
            config.src_tw = DMA_DATA_WIDTH_16_BITS;
            config.dst_tw = DMA_DATA_WIDTH_16_BITS;
        } else if (IS_8BIT_FRAME_LEN(spi)) {
            spi->tx_size = size;
            config.src_tw = DMA_DATA_WIDTH_8_BITS;
            config.dst_tw = DMA_DATA_WIDTH_8_BITS;
        } else {
            ret = CSI_ERROR;
            break;
        }

        config.src_inc = DMA_ADDR_INC;
        config.dst_inc = DMA_ADDR_CONSTANT;
        config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
        config.trans_dir = DMA_MEM2PERH;
        config.handshake = spi_tx_hs_num[spi->dev.idx];
        csi_dma_ch_config(dma_ch, &config);

        /* set tx mode*/
        dw_spi_disable(spi_base);
        dw_spi_set_tx_mode(spi_base);
        dw_spi_enable_tx_dma(spi_base);

        soc_dcache_clean_invalid_range((unsigned long)spi->tx_data, size);
        csi_dma_ch_start(spi->tx_dma, spi->tx_data, (void *) & (spi_base->DR), size);

        dw_spi_enable(spi_base);
    } while (0);

    return ret;
}

int32_t csi_spi_receive(csi_spi_t *spi, void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    uint32_t timestart;
    uint32_t count = 0U;
    int32_t  ret = CSI_OK;
    uint8_t *rx_data;
    uint32_t current_size;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    do {
        if ((spi->state.writeable == 0U) || (spi->state.readable == 0U)) {
            ret = CSI_BUSY;
            break;
        }

        if (IS_16BIT_FRAME_LEN(spi)) {
            if (size % sizeof(uint16_t)) {
                ret = CSI_ERROR;
                break;
            }
        }

        timestart = csi_tick_get_ms();
        spi->state.readable = 0U;
        spi->rx_data = (uint8_t *)data;

        // Convert byte to nums
        if (IS_16BIT_FRAME_LEN(spi)) {
            size = size / 2U;
        }

        rx_data = (uint8_t *)data;

        /* set rx mode*/
        if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
            dw_spi_disable(spi_base);
        }

        dw_spi_set_rx_mode(spi_base);
        dw_spi_config_rx_data_len(spi_base, size - 1U);
        dw_spi_enable(spi_base);

        if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
            dw_spi_transmit_data(spi_base, 0U);
        }

        /* transfer loop */
        if (IS_8BIT_FRAME_LEN(spi)) {
            while (size > 0U) {
                current_size = dw_spi_get_rx_fifo_level(spi_base);

                if (current_size > size) {
                    current_size = size;

                }

                while (current_size--) {
                    *(uint8_t *)rx_data = (uint8_t)dw_spi_receive_data(spi_base);
                    rx_data += 1;
                    size--;
                    count++;
                }
            }

            if ((csi_tick_get_ms() - timestart) > timeout) {
                break;
            }
        }

        if (IS_16BIT_FRAME_LEN(spi)) {
            while (size > 0U) {
                current_size = dw_spi_get_rx_fifo_level(spi_base);

                if (current_size > size) {
                    current_size = size;

                }

                while (current_size--) {
                    *(uint16_t *)rx_data = (uint16_t)dw_spi_receive_data(spi_base);
                    rx_data += 2;
                    size--;
                    count += 2U;
                }
            }

            if ((csi_tick_get_ms() - timestart) > timeout) {
                break;
            }
        }

        /* wait end of transcation */
        while ((dw_spi_get_status(spi_base) & DW_SPI_SR_BUSY)) {
            if ((csi_tick_get_ms() - timestart) > timeout) {
                break;
            }
        }
    } while (0);

    /* close spi */
    if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
        dw_spi_config_rx_data_len(spi_base, 0U);
        dw_spi_config_rx_fifo_threshold(spi_base, 0U);
    }

    spi->state.readable = 1U;

    if (ret >= 0) {
        ret = (int32_t)count;
    }

    return ret;
}


csi_error_t csi_spi_receive_async(csi_spi_t *spi, void *data, uint32_t size)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if ((spi->state.writeable == 0U) || (spi->state.readable == 0U)) {
        ret = CSI_BUSY;
    } else {
        if (IS_16BIT_FRAME_LEN(spi)) {
            if (size % sizeof(uint16_t)) {
                ret = CSI_ERROR;
            }
        }

        if ((ret == CSI_OK) && (spi->callback != NULL)) {
            if (spi->receive) {
                spi->state.readable = 0U;
                ret = spi->receive(spi, data, size);
            } else {
                spi->state.readable = 0U;
                csi_irq_attach((uint32_t)spi->dev.irq_num, &dw_spi_irqhandler, &spi->dev);
                csi_irq_enable((uint32_t)spi->dev.irq_num);
                ret = dw_spi_receive_intr(spi, data, size);
            }
        } else {
            ret = CSI_ERROR;
        }
    }

    return ret;
}

static csi_error_t dw_spi_receive_intr(csi_spi_t *spi, void *data, uint32_t size)
{
    csi_error_t ret = CSI_OK;
    uint32_t rx_fifo_lv;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    spi->rx_data = (uint8_t *)data;

    do {
        // Convert byte to nums
        if (IS_16BIT_FRAME_LEN(spi)) {
            spi->rx_size = size / 2U;
        } else if (IS_8BIT_FRAME_LEN(spi)) {
            spi->rx_size = size;
        } else {
            ret = CSI_ERROR;
            break;
        }

        /* set rx mode*/
        if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
            dw_spi_disable(spi_base);
        }

        dw_spi_set_rx_mode(spi_base);
        dw_spi_config_rx_data_len(spi_base, spi->rx_size - 1U);
        rx_fifo_lv = (spi->rx_size < DW_DEFAULT_SPI_RXFIFO_LV) ? (spi->rx_size - 1U) : DW_DEFAULT_SPI_RXFIFO_LV;
        dw_spi_config_rx_fifo_threshold(spi_base, rx_fifo_lv);
        dw_spi_enable(spi_base);
        dw_spi_enable_rx_fifo_full_irq(spi_base);

        if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
            dw_spi_transmit_data(spi_base, 0U);
        }

    } while (0);

    return ret;
}

static csi_error_t dw_spi_receive_dma(csi_spi_t *spi, void *data, uint32_t size)
{
    csi_dma_ch_config_t config;
    dw_spi_regs_t       *spi_base;
    csi_dma_ch_t        *dma_ch;
    csi_error_t         ret = CSI_OK;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    dma_ch   = (csi_dma_ch_t *)spi->rx_dma;
    spi->rx_data = (uint8_t *)data;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));

    do {
        /* configure dma channel */
        if (IS_16BIT_FRAME_LEN(spi)) {
            spi->rx_size = size / 2U;
            config.src_tw = DMA_DATA_WIDTH_16_BITS;
            config.dst_tw = DMA_DATA_WIDTH_16_BITS;
        } else if (IS_8BIT_FRAME_LEN(spi)) {
            spi->rx_size = size;
            config.src_tw = DMA_DATA_WIDTH_8_BITS;
            config.dst_tw = DMA_DATA_WIDTH_8_BITS;
        } else {
            ret = CSI_ERROR;
            break;
        }

        config.src_inc = DMA_ADDR_CONSTANT;
        config.dst_inc = DMA_ADDR_INC;
        config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
        config.trans_dir = DMA_PERH2MEM;
        config.handshake = spi_rx_hs_num[spi->dev.idx];
        csi_dma_ch_config(dma_ch, &config);

        /* set rx mode*/
        if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
            dw_spi_disable(spi_base);
        }

        dw_spi_set_rx_mode(spi_base);
        dw_spi_config_rx_data_len(spi_base, spi->rx_size - 1U);
        dw_spi_config_dma_rx_data_level(spi_base, (uint32_t)config.group_len / ((uint32_t)1U << (uint32_t)config.src_tw) - 1U);
        dw_spi_enable_rx_dma(spi_base);

        soc_dcache_clean_invalid_range((unsigned long)spi->rx_data, size);
        csi_dma_ch_start(spi->rx_dma, (void *) & (spi_base->DR), spi->rx_data, size);

        dw_spi_enable(spi_base);

        if (dw_spi_get_slave_mode(spi_base) & DW_SPI_SPIMSSEL_MASTER) {
            dw_spi_transmit_data(spi_base, 0U);
        }
    } while (0);

    return ret;
}

int32_t csi_spi_send_receive(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(spi,      CSI_ERROR);
    CSI_PARAM_CHK(data_out, CSI_ERROR);
    CSI_PARAM_CHK(data_in,  CSI_ERROR);
    CSI_PARAM_CHK(size,     CSI_ERROR);

    uint32_t value;
    uint32_t timestart;
    uint32_t count = 0U;
    int32_t  ret   = CSI_OK;
    uint32_t tx_size, rx_size;
    uint8_t  *tx_data, *rx_data;
    uint32_t current_size;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    do {

        if ((spi->state.writeable == 0U) || (spi->state.readable == 0U)) {
            ret = CSI_BUSY;
            break;
        }

        if (IS_16BIT_FRAME_LEN(spi)) {
            if (size % sizeof(uint16_t)) {
                ret = CSI_ERROR;
                break;
            }
        }

        timestart = csi_tick_get_ms();
        spi->state.writeable = 0U;
        spi->state.readable  = 0U;
        tx_data = (uint8_t *)data_out;

        if (IS_16BIT_FRAME_LEN(spi)) {
            tx_size = size / 2U;
        } else {
            tx_size = size;
        }

        rx_data = (uint8_t *)data_in;

        if (IS_16BIT_FRAME_LEN(spi)) {
            rx_size = size / 2U;
        } else {
            rx_size = size;
        }

        /* set tx rx mode*/
        dw_spi_disable(spi_base);
        dw_spi_set_tx_rx_mode(spi_base);
        dw_spi_config_tx_fifo_threshold(spi_base, DW_DEFAULT_SPI_TXFIFO_LV);
        dw_spi_config_rx_fifo_threshold(spi_base, DW_DEFAULT_SPI_RXFIFO_LV);
        dw_spi_enable(spi_base);

        /* transfer loop */
        if (IS_8BIT_FRAME_LEN(spi)) {
            while ((tx_size > 0U) || (rx_size > 0U)) {
                /* process tx fifo empty */
                if (tx_size > 0U) {
                    current_size = DW_MAX_SPI_TXFIFO_LV - dw_spi_get_tx_fifo_level(spi_base);

                    if (current_size > tx_size) {
                        current_size = tx_size;

                    }

                    while (current_size--) {
                        value = (uint32_t)(*(uint8_t *)tx_data);
                        dw_spi_transmit_data(spi_base, value);
                        tx_data += 1;
                        count += 1U;
                        tx_size--;
                    }
                }

                /* process rx fifo not empty */
                if (rx_size > 0U) {
                    current_size = dw_spi_get_rx_fifo_level(spi_base);

                    if (current_size > rx_size) {
                        current_size = rx_size;

                    }

                    while (current_size--) {
                        *(uint8_t *)rx_data = (uint8_t)dw_spi_receive_data(spi_base);
                        rx_data += 1;
                        rx_size--;
                    }
                }

                if ((csi_tick_get_ms() - timestart) > timeout) {
                    break;
                }
            }
        } else if (IS_16BIT_FRAME_LEN(spi)) {
            while ((tx_size > 0U) || (rx_size > 0U)) {
                /* process tx fifo empty */
                if (tx_size > 0U) {
                    current_size = DW_MAX_SPI_TXFIFO_LV - dw_spi_get_tx_fifo_level(spi_base);

                    if (current_size > tx_size) {
                        current_size = tx_size;

                    }

                    while (current_size--) {
                        value = (uint32_t)(*(uint16_t *)tx_data);
                        dw_spi_transmit_data(spi_base, value);
                        tx_data += 2;
                        count += 2U;
                        tx_size--;
                    }
                }

                /* process rx fifo not empty */
                if (rx_size > 0U) {
                    current_size = dw_spi_get_rx_fifo_level(spi_base);

                    if (current_size > rx_size) {
                        current_size = rx_size;

                    }

                    while (current_size--) {
                        *(uint16_t *)rx_data = (uint16_t)dw_spi_receive_data(spi_base);
                        rx_data += 2;
                        rx_size--;
                    }
                }

                if ((csi_tick_get_ms() - timestart) > timeout) {
                    break;
                }

            }
        }

        /* wait end of transcation */
        while (dw_spi_get_status(spi_base) & DW_SPI_SR_BUSY) {
            if ((csi_tick_get_ms() - timestart) > timeout) {
                break;
            }
        }
    } while (0);

    /* close spi */
    spi->state.writeable = 1U;
    spi->state.readable  = 1U;

    if (ret >= 0) {
        ret = (int32_t)count;
    }

    return ret;
}


csi_error_t csi_spi_send_receive_async(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data_out, CSI_ERROR);
    CSI_PARAM_CHK(data_in, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if ((spi->state.writeable == 0U) || (spi->state.readable == 0U)) {
        ret = CSI_BUSY;
    }

    if (IS_16BIT_FRAME_LEN(spi)) {
        if (size % sizeof(uint16_t)) {
            ret = CSI_ERROR;
        }
    }

    if ((ret == CSI_OK) && (spi->callback != NULL)) {
        if (spi->send_receive) {
            spi->state.readable  = 0U;
            spi->state.writeable = 0U;
            ret = spi->send_receive(spi, data_out, data_in, size);
        } else {
            spi->state.readable  = 0U;
            spi->state.writeable = 0U;
            csi_irq_attach((uint32_t)spi->dev.irq_num, &dw_spi_irqhandler, &spi->dev);
            csi_irq_enable((uint32_t)spi->dev.irq_num);
            ret = dw_spi_send_receive_intr(spi, data_out, data_in, size);
        }
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}

static csi_error_t dw_spi_send_receive_intr(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    csi_error_t ret = CSI_OK;
    uint32_t rx_fifo_lv;

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);

    spi->tx_data = (uint8_t *)data_out;
    spi->rx_data = (uint8_t *)data_in;

    do {
        if (IS_16BIT_FRAME_LEN(spi)) {
            spi->tx_size = size / 2U;
            spi->rx_size = size / 2U;
        } else if (IS_8BIT_FRAME_LEN(spi)) {
            spi->tx_size = size;
            spi->rx_size = size;
        } else {
            ret = CSI_ERROR;
            break;
        }

        /* set tx rx mode*/
        dw_spi_disable(spi_base);
        dw_spi_set_tx_rx_mode(spi_base);
        dw_spi_config_rx_data_len(spi_base, spi->rx_size - 1U);
        dw_spi_config_tx_fifo_threshold(spi_base, DW_DEFAULT_SPI_TXFIFO_LV);
        rx_fifo_lv = (spi->rx_size < (DW_DEFAULT_SPI_RXFIFO_LV + 1U)) ? (spi->rx_size - 1U) : DW_DEFAULT_SPI_RXFIFO_LV;
        dw_spi_config_rx_fifo_threshold(spi_base, rx_fifo_lv);
        dw_spi_enable(spi_base);
        dw_spi_enable_rx_fifo_full_irq(spi_base);
        dw_spi_enable_tx_empty_irq(spi_base);

    } while (0);

    return ret;
}

static csi_error_t dw_spi_send_receive_dma(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    csi_dma_ch_config_t config;
    dw_spi_regs_t       *spi_base;
    csi_dma_ch_t        *dma_ch;
    csi_error_t         ret = CSI_OK;

    spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    spi->tx_data = (uint8_t *)data_out;
    memset(&config, 0, sizeof(csi_dma_ch_config_t));

    do {

        if (IS_16BIT_FRAME_LEN(spi)) {
            spi->tx_size = size / 2U;
            spi->rx_size = size / 2U;
            config.src_tw = DMA_DATA_WIDTH_16_BITS;
            config.dst_tw = DMA_DATA_WIDTH_16_BITS;
        } else if (IS_8BIT_FRAME_LEN(spi)) {
            spi->tx_size = size;
            spi->rx_size = size;
            config.src_tw = DMA_DATA_WIDTH_8_BITS;
            config.dst_tw = DMA_DATA_WIDTH_8_BITS;
        } else {
            ret = CSI_ERROR;
            break;
        }

        spi->rx_data = (uint8_t *)data_in;

        /* configure tx dma channel */
        dma_ch   = (csi_dma_ch_t *)spi->tx_dma;
        config.src_inc = DMA_ADDR_INC;
        config.dst_inc = DMA_ADDR_CONSTANT;
        config.group_len = DW_DEFAULT_SPI_TXFIFO_LV;
        config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
        config.trans_dir = DMA_MEM2PERH;
        config.handshake = spi_tx_hs_num[spi->dev.idx];
        csi_dma_ch_config(dma_ch, &config);

        /* configure dma rx channel */
        dma_ch   = (csi_dma_ch_t *)spi->rx_dma;
        config.src_inc = DMA_ADDR_CONSTANT;
        config.dst_inc = DMA_ADDR_INC;
        config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
        config.trans_dir = DMA_PERH2MEM;
        config.handshake = spi_rx_hs_num[spi->dev.idx];
        csi_dma_ch_config(dma_ch, &config);

        /* set tx_rx mode*/
        dw_spi_disable(spi_base);
        dw_spi_set_tx_rx_mode(spi_base);
        dw_spi_config_rx_data_len(spi_base, spi->rx_size - 1U);
        dw_spi_config_dma_rx_data_level(spi_base, (uint32_t)config.group_len / ((uint32_t)1U << (uint32_t)config.src_tw) - 1U);
        dw_spi_enable_rx_dma(spi_base);
        dw_spi_enable_tx_dma(spi_base);

        soc_dcache_clean_invalid_range((unsigned long)spi->tx_data, size);
        soc_dcache_clean_invalid_range((unsigned long)spi->rx_data, size);

        csi_dma_ch_start(spi->rx_dma, (void *) & (spi_base->DR), spi->rx_data, size);
        csi_dma_ch_start(spi->tx_dma, spi->tx_data, (void *) & (spi_base->DR), size);
    } while (0);

    dw_spi_enable(spi_base);
    return ret;
}

csi_error_t csi_spi_get_state(csi_spi_t *spi, csi_state_t *state)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(state, CSI_ERROR);

    *state = spi->state;
    return CSI_OK;
}

csi_error_t csi_spi_link_dma(csi_spi_t *spi, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (tx_dma != NULL) {
        tx_dma->parent = spi;
        ret = csi_dma_ch_alloc(tx_dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(tx_dma, dw_spi_dma_event_cb, NULL);
            spi->tx_dma = tx_dma;
            spi->send = dw_spi_send_dma;
        } else {
            tx_dma->parent = NULL;
        }
    } else {
        if (spi->tx_dma) {
            csi_dma_ch_detach_callback(spi->tx_dma);
            csi_dma_ch_free(spi->tx_dma);
            spi->tx_dma = NULL;
        }

        spi->send = NULL;
    }

    if (ret == CSI_OK) {
        if (rx_dma != NULL) {
            rx_dma->parent = spi;
            ret = csi_dma_ch_alloc(rx_dma, -1, -1);

            if (ret == CSI_OK) {
                csi_dma_ch_attach_callback(rx_dma, dw_spi_dma_event_cb, NULL);
                spi->rx_dma = rx_dma;
                spi->receive = dw_spi_receive_dma;
            } else {
                rx_dma->parent = NULL;
            }
        } else {
            if (spi->rx_dma) {
                csi_dma_ch_detach_callback(spi->rx_dma);
                csi_dma_ch_free(spi->rx_dma);
                spi->rx_dma = NULL;
            }

            spi->receive = NULL;
        }
    }


    if (ret == CSI_OK) {
        if ((tx_dma != NULL) && (rx_dma != NULL)) {
            spi->send_receive =  dw_spi_send_receive_dma;
        } else {
            spi->send_receive = NULL;
        }
    }

    return ret;
}

void csi_spi_select_slave(csi_spi_t *spi, uint32_t slave_num)
{
    CSI_PARAM_CHK_NORETVAL(spi);

    dw_spi_regs_t *spi_base = (dw_spi_regs_t *)HANDLE_REG_BASE(spi);
    dw_spi_enable_slave(spi_base, slave_num);
}

#ifdef CONFIG_PM
csi_error_t dw_spi_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;

    switch (action) {
        case PM_DEV_SUSPEND:
            csi_pm_dev_save_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 7U);
            csi_pm_dev_save_regs(pm_dev->reten_mem + 7U, (uint32_t *)(dev->reg_base + 44U), 1U);
            csi_pm_dev_save_regs(pm_dev->reten_mem + 8U, (uint32_t *)(dev->reg_base + 76U), 3U);
            csi_pm_dev_save_regs(pm_dev->reten_mem + 11U, (uint32_t *)(dev->reg_base + 160U), 1U);
            break;

        case PM_DEV_RESUME:
            csi_pm_dev_restore_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 2U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 3U, (uint32_t *)dev->reg_base + 12U, 4U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 7U, (uint32_t *)(dev->reg_base + 44U), 1U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 8U, (uint32_t *)(dev->reg_base + 76U), 3U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 11U, (uint32_t *)(dev->reg_base + 160U), 1U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 2U, (uint32_t *)(dev->reg_base + 8U), 1U);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_spi_enable_pm(csi_spi_t *spi)
{
    return csi_pm_dev_register(&spi->dev, dw_spi_pm_action, 36U, 0U);
}

void csi_spi_disable_pm(csi_spi_t *spi)
{
    csi_pm_dev_unregister(&spi->dev);
}
#endif

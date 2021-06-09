/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_usi_spi.c
 * @brief    header file for usi ll driver
 * @version  V1.0
 * @date     1. Mon 2020
 ******************************************************************************/
#include <drv/irq.h>
#include <drv/spi.h>
#include <wj_usi_ll.h>
#include <drv/usi_spi.h>
#include <drv/common.h>
#include <drv/tick.h>
#include "wj_usi_com.h"

#define DW_DEFAULT_TRANSCATION_TIMEOUT 200U
#define IS_8BIT_FRAME_LEN(addr)   (wj_usi_get_spi_ctrl_data_size(addr) < 8U)
#define IS_16BIT_FRAME_LEN(addr)  (( wj_usi_get_spi_ctrl_data_size(addr) >= 8U ) && ( wj_usi_get_spi_ctrl_data_size(addr) < 16U ))

extern void mdelay(uint32_t ms);
extern void wj_usi_irq_handler(void *arg);

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
    uint32_t prime_num;
    uint32_t limit;

    limit = 8U;

    do {
        prime_num = find_max_prime_num(size, limit);
        limit = prime_num - 1U;
    } while ((prime_num % width) != 0U);

    return (uint8_t)prime_num;
}

static csi_error_t wait_ready_until_timeout(csi_spi_t *usi, uint32_t timeout)
{
    uint32_t timestart = 0U;
    csi_error_t    ret = CSI_OK;
    ck_usi_regs_t *regs;

    regs = (ck_usi_regs_t *)HANDLE_REG_BASE(usi);

    timestart = csi_tick_get_ms();

    while (wj_usi_get_fifo_sta_tx_num(regs) > 0U) {
        if ((csi_tick_get_ms() - timestart) > timeout) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}

static void drv_usi_spi_clear_rx_buf(ck_usi_regs_t *addr);

static void ck_usi_spi_intr_transmit(csi_spi_t *usi)
{
    CSI_PARAM_CHK_NORETVAL(usi);
    ck_usi_regs_t *addr;
    uint8_t txdata_num, txfifo_num;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(usi);
    txfifo_num = wj_usi_get_fifo_sta_tx_num(addr);

    txdata_num = (usi->tx_size > (USI_TX_MAX_FIFO - txfifo_num)) ? (USI_TX_MAX_FIFO - txfifo_num) : usi->tx_size;

    if (wj_usi_get_spi_ctrl_tmode(addr) == WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE) {
        txdata_num = txdata_num > (USI_TX_MAX_FIFO / 2) ? (USI_TX_MAX_FIFO / 2) : txdata_num;
    }

    for (uint16_t i = 0U; i < txdata_num; i++) {
        addr->USI_TX_RX_FIFO = *((uint8_t *)usi->tx_data);
        usi->tx_data++;
        usi->tx_size--;
        wj_usi_get_tx_rx_fifo(addr);
    }

    if ((usi->tx_size == 0U) && (wj_usi_get_fifo_sta_tx_num(addr) == 0)) {
        wj_usi_en_tx_thold_mask(addr);
        wj_usi_dis_intr_tx_thold(addr);

        if (usi->callback) {

            if (( wj_usi_get_spi_ctrl_tmode(addr) == WJ_USI_SPI_CTRL_TMOD_SEND_ONLY ) ||
               ( !usi->rx_data && ( wj_usi_get_spi_ctrl_tmode(addr) == WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE ) )) {
                wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
                drv_usi_spi_clear_rx_buf(addr);
                usi->callback(usi, SPI_EVENT_SEND_COMPLETE, usi->arg);
            }
        }
    }

}

/**
  \brief        interrupt service function for receiver data available.
  \param[in]   uart usart private to operate.
*/
static void ck_usi_spi_intr_receive(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);
    ck_usi_regs_t *addr;
    uint8_t rxfifo_num, rxdata_num, i;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    rxfifo_num = wj_usi_get_fifo_sta_rx_num(addr);
    rxdata_num = (rxfifo_num > spi->rx_size) ? spi->rx_size : rxfifo_num;

    for (i = 0U; i < rxdata_num; i++) {
        *((uint8_t *)spi->rx_data) = (uint8_t) wj_usi_get_tx_rx_fifo(addr);
        spi->rx_size--;
        spi->rx_data++;
    }

    if ((spi->rx_size == 0U) || (spi->rx_data == NULL)) {
        wj_usi_dis_intr_rx_thold(addr);
        wj_usi_en_rx_thold_mask(addr);

        if (wj_usi_get_spi_mode(addr) == WJ_USI_SPI_MODE_MASTER) {
            while (wj_usi_get_spi_sta_working(addr) > 0);
        }

        if (wj_usi_get_spi_ctrl_tmode(addr) == WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE) {
            wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
            drv_usi_spi_clear_rx_buf(addr);
            spi->callback(spi, SPI_EVENT_SEND_RECEIVE_COMPLETE, spi->arg);
        } else {
            wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
            drv_usi_spi_clear_rx_buf(addr);
            spi->callback(spi, SPI_EVENT_RECEIVE_COMPLETE, spi->arg);
        }
    } else {
        wj_usi_set_rx_fifo_th(addr, usi_find_max_prime_num(spi->rx_size));
    }
}

void ck_usi_spi_irqhandler(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    uint32_t intr_state =  wj_usi_get_intr_sta(addr);


    if (intr_state & WJ_USI_INTR_STA_TX_EMPTY_Msk) {
        ck_usi_spi_intr_transmit(spi);
    }

    if (intr_state & WJ_USI_INTR_STA_TX_THOLD_Msk) {
        ck_usi_spi_intr_transmit(spi);
    }

    if (intr_state & WJ_USI_INTR_STA_RX_EMPTY_Msk) {
        uint32_t tmp = wj_usi_get_fifo_sta_rx_num(addr);

        if (tmp > 0U) {
            ck_usi_spi_intr_receive(spi);
        } else {
            /* code */
        }

    }

    if (intr_state & WJ_USI_INTR_STA_RX_THOLD_Msk) {
        uint32_t tmp = wj_usi_get_fifo_sta_rx_num(addr);

        if (tmp > 0U) {
            ck_usi_spi_intr_receive(spi);
        } else {
            /* code */
        }

    }

    wj_usi_set_intr_clr(addr, intr_state);
}

/**
  \brief       Initialize SPI Interface.
               Initializes the resources needed for the SPI instance
  \param[in]   spi      spi handle
  \param[in]   idx      spi instance index
  \return      error code
*/
csi_error_t csi_usi_spi_init(csi_spi_t *spi, uint32_t idx)
{
    ck_usi_regs_t *addr;

    CSI_PARAM_CHK(spi, CSI_ERROR);

    target_get(DEV_WJ_USI_TAG, idx, &spi->dev);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    spi->rx_size = 0U;
    spi->tx_size = 0U;
    spi->rx_data = NULL;
    spi->tx_data = NULL;
    spi->state.writeable = 1;
    spi->state.readable = 1;
    spi->state.error = 0;
    spi->send = NULL;
    spi->receive = NULL;
    spi->send_receive = NULL;
    spi->arg = NULL;
    spi->tx_dma = NULL;
    spi->rx_dma = NULL;
    spi->callback = NULL;
    wj_usi_set_ctrl(addr, 0);
    wj_usi_set_mode_sel(addr, WJ_USI_MODE_SEL_SPI);
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
    wj_usi_set_intr_en(addr, 0U);
    wj_usi_set_intr_unmask(addr, 0U);
    wj_usi_set_ctrl(addr, WJ_USI_CTRL_USI_EN | WJ_USI_CTRL_FM_EN | WJ_USI_CTRL_TX_FIFO_EN | WJ_USI_CTRL_RX_FIFO_EN);
    return CSI_OK;
}

/**
  \brief       De-initialize SPI Interface
               stops operation and releases the software resources used by the spi instance
  \param[in]   spi spi handle
  \return      none
*/
void    csi_usi_spi_uninit(csi_spi_t *spi)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK_NORETVAL(spi);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    spi->rx_size = 0U;
    spi->tx_size = 0U;
    spi->rx_data = NULL;
    spi->tx_data = NULL;
    spi->state.writeable = 1;
    spi->state.readable = 1;
    spi->state.error = 0;
    spi->send = NULL;
    spi->receive = NULL;
    spi->send_receive = NULL;
    spi->arg = NULL;
    spi->tx_dma = NULL;
    spi->rx_dma = NULL;
    spi->callback = NULL;

    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
    drv_usi_spi_clear_rx_buf(addr);
    wj_usi_rst_regs(addr);
    spi->callback   = NULL;
}

/**
  \brief       Attach the callback handler to SPI
  \param[in]   spi  operate handle.
  \param[in]   callback    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_usi_spi_attach_callback(csi_spi_t *spi, void *callback, void *arg)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    spi->callback  = callback;
    spi->arg = arg;
    csi_irq_attach((uint32_t)spi->dev.irq_num, &wj_usi_irq_handler, &spi->dev);
    csi_irq_enable((uint32_t)spi->dev.irq_num);
    return CSI_OK;
}

/**
  \brief       Detach the callback handler
  \param[in]   spi  operate handle.
  \return      none
*/
void        csi_usi_spi_detach_callback(csi_spi_t *spi)
{
    CSI_PARAM_CHK_NORETVAL(spi);
    csi_irq_detach((uint32_t)spi->dev.irq_num);
    csi_irq_disable((uint32_t)spi->dev.irq_num);
}

/**
  \brief       Config spi mode (master or slave).
  \param[in]   spi    spi handle
  \param[in]   mode   the mode of spi (master or slave)
  \return      error code
*/
csi_error_t csi_usi_spi_mode(csi_spi_t *spi, csi_spi_mode_t mode)
{
    csi_error_t ret = CSI_OK;
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK(spi, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    if (mode == SPI_MASTER) {
        wj_usi_set_spi_mode_master(addr);
    } else if (mode == SPI_SLAVE) {
        wj_usi_set_spi_mode_slave(addr);
    } else {
        ret = CSI_ERROR;/* code */
    }

    return ret;
}

/**
  \brief       Config spi frame len.
  \param[in]   spi       spi handle
  \param[in]   length    spi frame len
  \return      error code
*/
csi_error_t csi_usi_spi_frame_len(csi_spi_t *spi, csi_spi_frame_len_t length)
{
    csi_error_t ret = CSI_OK;
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK(spi, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    if ((length >= SPI_FRAME_LEN_4) && (length <= SPI_FRAME_LEN_16)) {
        wj_usi_set_spi_ctrl_data_size(addr, (uint32_t)length - 1U);
    } else {
        ret = CSI_ERROR;
    }

    return ret;
}

int32_t wj_usi_spi_config_baudrate(ck_usi_regs_t *addr, uint32_t freq, uint32_t speed)
{
    uint32_t div;
    div = (freq / speed) - 1U;
    wj_usi_set_clk_div0(addr, div);
    return 0;
}


/**
  \brief       Config spi work frequence.
  \param[in]   spi     spi handle
  \param[in]   baud    spi work baud
  \return      the actual config frequency
*/
uint32_t csi_usi_spi_baud(csi_spi_t *spi, uint32_t baud)
{
    ck_usi_regs_t *addr;
    uint32_t freq;
    CSI_PARAM_CHK(spi, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    freq = soc_get_usi_freq((uint32_t)spi->dev.idx);//
    wj_usi_spi_config_baudrate(addr, freq, baud);
    ///<set tmode to send and receive mode
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
    wj_usi_dis_spi_ctrl_nss_toggle(addr);
    wj_usi_dis_nss(addr);
    return baud;
}

/**
  \brief       Config spi cp format.
  \param[in]   spi       spi handle
  \param[in]   format    spi cp format
  \return      error code
*/
csi_error_t csi_usi_spi_cp_format(csi_spi_t *spi, csi_spi_cp_format_t format)
{
    ck_usi_regs_t *addr;
    csi_error_t ret = CSI_OK;
    CSI_PARAM_CHK(spi, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    switch (format) {
        case SPI_FORMAT_CPOL0_CPHA0:
            wj_usi_dis_spi_ctrl_cpol(addr);
            wj_usi_dis_spi_ctrl_cpha(addr);
            break;

        case SPI_FORMAT_CPOL0_CPHA1:
            wj_usi_dis_spi_ctrl_cpol(addr);
            wj_usi_en_spi_ctrl_cpha(addr);
            break;

        case SPI_FORMAT_CPOL1_CPHA0:
            wj_usi_en_spi_ctrl_cpol(addr);
            wj_usi_dis_spi_ctrl_cpha(addr);
            break;

        case SPI_FORMAT_CPOL1_CPHA1:
            wj_usi_en_spi_ctrl_cpol(addr);
            wj_usi_en_spi_ctrl_cpha(addr);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

void  drv_usi_nss_select(csi_spi_t *spi)
{
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    wj_usi_dis_spi_nss_data(addr);
}

void  drv_usi_nss_unselect(csi_spi_t *spi)
{
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    wj_usi_en_spi_nss_data(addr);
}

static void drv_usi_spi_clear_rx_buf(ck_usi_regs_t *addr)
{
    if (wj_usi_get_spi_mode(addr) == WJ_USI_SPI_MODE_MASTER){
        while (wj_usi_get_spi_sta_working(addr) > 0);
    }
    while (wj_usi_get_fifo_sta_rx_empty(addr) != WJ_USI_FIFO_STA_RX_EMPTY) { 
        wj_usi_get_tx_rx_fifo(addr);
    }
}

static csi_error_t drv_usi_common_send1(ck_usi_regs_t *addr, const uint8_t *data, uint32_t  num, uint32_t  timeout)
{
    uint32_t send_num = 0U;
    uint32_t timestart = csi_tick_get_ms() ;
    csi_error_t ret = CSI_OK;
    CSI_PARAM_CHK(addr, CSI_ERROR);

    while (send_num < num) {
        if (wj_usi_get_fifo_sta_tx_full(addr) != WJ_USI_FIFO_STA_TX_FULL) {
            wj_usi_set_tx_rx_fifo(addr, (uint32_t)(*data));
            send_num++;
            data++;
            wj_usi_get_tx_rx_fifo(addr);
        }

        if ((csi_tick_get_ms() - timestart) >= timeout) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    while (wj_usi_get_fifo_sta_tx_num(addr) > 0U) {
        if ((csi_tick_get_ms() - timestart) >= timeout) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}

csi_error_t drv_usi_common_send_receive(ck_usi_regs_t *addr, const uint8_t *data_out,  uint8_t *data_in, uint32_t size, uint32_t timeout)
{
    uint32_t timestart = csi_tick_get_ms() ;
    uint32_t current_size;
    uint32_t tx_size, rx_size;

    csi_error_t ret = CSI_OK;

    CSI_PARAM_CHK(addr, CSI_ERROR);

    tx_size = size;
    rx_size = size;
    while ((tx_size > 0U) || (rx_size >0U)) {
        if (tx_size > 0U) {
            current_size = WJ_USI_FIFO_MAX - wj_usi_get_fifo_sta_tx_num(addr);

            if (current_size > tx_size) {
                current_size = tx_size;
            }

            while (current_size--) {
                wj_usi_set_tx_rx_fifo(addr, (uint32_t)(*data_out++));
                tx_size--;
            }
        }

        if (rx_size > 0U) {
            current_size = wj_usi_get_fifo_sta_rx_num(addr);

            if (current_size > rx_size) {
                current_size = rx_size;
            }

            while (current_size--) {
                *data_in = (uint8_t)wj_usi_get_tx_rx_fifo(addr);
                data_in++;
                rx_size--;
            }
        }

        if ((csi_tick_get_ms() - timestart) >= timeout) {
            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}

static csi_error_t drv_usi_common_receive1(ck_usi_regs_t *addr, uint8_t *data, uint32_t  num, uint32_t  timeout)
{
    uint32_t send_num = 0U;
    uint32_t timestart = csi_tick_get_ms();

    csi_error_t ret = CSI_OK;

    CSI_PARAM_CHK(addr, CSI_ERROR);

    drv_usi_spi_clear_rx_buf(addr);

    while (send_num < num) {
        if (wj_usi_get_fifo_sta_rx_num(addr) > 0U) {
            *data = (uint8_t)wj_usi_get_tx_rx_fifo(addr);
            data++;
            send_num++;
        } else {
            if (wj_usi_get_fifo_sta_tx_empty(addr) == WJ_USI_FIFO_STA_TX_EMPTY_Msk) {
                wj_usi_set_tx_rx_fifo(addr, 0U);
            }
        }

        if ((csi_tick_get_ms() - timestart) >= timeout) {

            ret = CSI_TIMEOUT;
            break;
        }
    }

    return ret;
}
/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               blocking mode ,return unti all data has been sent or err happened
  \param[in]   spi   handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter.
  \param[in]   size  Number of data to send(byte)
  \param[in]   timeout  unit in mini-second
  \return      if send success, this function shall return the num of data witch is sent successful
               otherwise, the function shall return error code
*/
int32_t csi_usi_spi_send(csi_spi_t *spi, const void *data, uint32_t size, uint32_t timeout)
{
    ck_usi_regs_t *addr;
    uint32_t intr_en_status;
    csi_error_t ret = CSI_OK;
    int32_t ret1 = CSI_TIMEOUT;
    /* check data and uart */
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        mdelay(1);
    }
    /* store the status of intr */
    intr_en_status = wj_usi_get_intr_en(addr);
    wj_usi_set_intr_en(addr, 0U); ///<disable all interrupt
    ret = drv_usi_common_send1(addr, data, size, timeout);
    wj_usi_set_intr_en(addr, intr_en_status);

    if (ret == CSI_OK) {
        ret1 = (int32_t)size;
    }

    return ret1;
}


csi_error_t usi_spi_send_async(csi_spi_t *spi, const void *data, uint32_t size)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    spi->tx_size = size;
    spi->tx_data = (uint8_t *)data;
    spi->rx_size = 0;
    spi->rx_data = NULL;
    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_set_tx_fifo_th(addr, WJ_USI_INTR_CTRL_TX_FIFO_TH_7BYTE);
    wj_usi_set_rx_edge_le(addr);
    wj_usi_dis_tx_thold_mask(addr);
    wj_usi_en_intr_tx_thold(addr);

    return  CSI_OK;
}

/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               non-blocking mode,transfer done event will be signaled by driver
  \param[in]   spi   handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter.
  \param[in]   size  Number of data items to send(byte)
  \return      error code
*/
csi_error_t csi_usi_spi_send_async(csi_spi_t *spi, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        mdelay(1);
    }

    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_ONLY);

    if (spi->send) {
        spi->send(spi, data, size);
    } else {
        csi_irq_enable((uint32_t)spi->dev.irq_num);
        wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
        usi_spi_send_async(spi, data, size);
    }

    return ret;
}


/**
  \brief       receiving data from SPI receiver.
               blocking mode, return untill curtain data items are readed
  \param[in]   spi   handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   size  Number of data items to receive(byte)
  \param[in]   timeout  unit in mini-second
  \return      if receive success, this function shall return the num of data witch is received successful
               otherwise, the function shall return error code
*/
int32_t csi_usi_spi_receive(csi_spi_t *spi, void *data, uint32_t size, uint32_t timeout)
{
    uint32_t intr_en_status;
    ck_usi_regs_t *addr;
    csi_error_t ret = CSI_OK;
    int32_t ret1 = CSI_TIMEOUT;

    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        mdelay(1);
    }
    intr_en_status =   wj_usi_get_intr_sta(addr);

    wj_usi_set_intr_en(addr, 0U); //disable all interrupt

    ret = drv_usi_common_receive1(addr, data, size, timeout);

    wj_usi_set_intr_en(addr, intr_en_status);

    if (ret == CSI_OK) {
        ret1 = (int32_t)size;
    }

    return ret1;

}

csi_error_t usi_spi_receive_async(csi_spi_t *spi, void *data, uint32_t size)
{

    ck_usi_regs_t *addr ;

    spi->rx_size = size;
    spi->rx_data = data;

    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    spi->rx_data = data;
    spi->rx_size = size;
    spi->tx_data = NULL;
    spi->tx_size = 0;
    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_set_rx_fifo_th(addr, usi_find_max_prime_num(size));
    wj_usi_set_rx_edge_ge(addr);
    wj_usi_dis_rx_thold_mask(addr);
    wj_usi_en_intr_rx_thold(addr);
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_RECEIVE_ONLY);
    return  CSI_OK;
}

/**
  \brief       receiving data from SPI receiver.
               not-blocking mode, event will be signaled when receive done or err happend
  \param[in]   spi   handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   size  Number of data items to receive(byte)
  \return      error code
*/
csi_error_t csi_usi_spi_receive_async(csi_spi_t *spi, void *data, uint32_t size)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        mdelay(1);
    }

    drv_usi_spi_clear_rx_buf(addr);

    if (spi->receive) {
        spi->receive(spi, data, size);
    } else {
        csi_irq_enable((uint32_t)spi->dev.irq_num);
        usi_spi_receive_async(spi, data, size);
    }

    return ret;
}

/**
  \brief       dulplex,sending and receiving data at the same time
               \ref csi_spi_event_t is signaled when operation completes or error happens.
               \ref csi_spi_get_state can get operation status.
               blocking mode, this function returns after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   size      data size(byte)
  \return      if transfer success, this function shall return the num of data witch is transfer successful
               otherwise, the function shall return error code
*/
int32_t csi_usi_spi_send_receive(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size, uint32_t timeout)
{
    ck_usi_regs_t *addr;
    uint32_t intr_en_status;
    csi_error_t ret = CSI_OK;
    int32_t ret1 = CSI_TIMEOUT;
    /* check data and uart */
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data_out, CSI_ERROR);
    CSI_PARAM_CHK(data_in, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        mdelay(1);
    }
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);

    /* store the status of intr */
    intr_en_status = wj_usi_get_intr_sta(addr);

    wj_usi_set_intr_en(addr, 0U); ///<disable all interrupt

    ret = drv_usi_common_send_receive(addr, data_out, data_in, size, timeout);

    wj_usi_set_intr_en(addr, intr_en_status);

    if (ret == CSI_OK) {
        ret1 = (int32_t)size;
    }

    return ret1;

}

void  usi_spi_send_receive_async(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    ck_usi_regs_t *addr;
    CSI_PARAM_CHK_NORETVAL(spi);
    CSI_PARAM_CHK_NORETVAL(data_out);
    CSI_PARAM_CHK_NORETVAL(data_in);

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        mdelay(1);
    }
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
    spi->tx_size = size;
    spi->tx_data = (uint8_t *)data_out;
    spi->rx_size = size;
    spi->rx_data = (uint8_t *)data_in;
    wj_usi_set_intr_clr(addr, ~0U);
    wj_usi_set_tx_fifo_th(addr, WJ_USI_INTR_CTRL_TX_FIFO_TH_7BYTE);
    wj_usi_dis_tx_thold_mask(addr);
    wj_usi_en_intr_tx_thold(addr);
    wj_usi_set_rx_fifo_th(addr, usi_find_max_prime_num(size));
    wj_usi_set_rx_edge_ge(addr);
    wj_usi_en_intr_rx_thold(addr);
    wj_usi_dis_rx_thold_mask(addr);
}
/**
  \brief       transmit first then receive ,receive will begin after transmit is done
               if non-blocking mode, this function only starts the transfer,
               \ref csi_spi_event_t is signaled when operation completes or error happens.
               \ref csi_spi_get_state can get operation status.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   size      data size(byte)
  \return      error code
*/
csi_error_t csi_usi_spi_send_receive_async(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);
    CSI_PARAM_CHK(data_out, CSI_ERROR);
    CSI_PARAM_CHK(data_in, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);

    if (spi->send) {
        spi->send_receive(spi, data_out, data_in, size);
    } else {

        csi_irq_enable((uint32_t)spi->dev.irq_num);
        usi_spi_send_receive_async(spi, data_out, data_in, size);
    }

    return ret;

}

/*
  \brief       set slave select num. Only valid for master
  \param[in]   handle  spi handle to operate.
  \param[in]   slave_num  spi slave num.
  \return      none
 */
void csi_usi_spi_select_slave(csi_spi_t *spi, uint32_t slave_num)
{
    ck_usi_regs_t *addr ;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    wj_usi_set_spi_mode_slave(addr);
}

static csi_error_t dw_usi_spi_send_dma(csi_spi_t *spi, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(spi,  CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    csi_dma_ch_config_t config;
    ck_usi_regs_t       *addr;
    csi_dma_ch_t        *dma_ch;

    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    dma_ch   = (csi_dma_ch_t *)spi->tx_dma;
    spi->tx_data = (uint8_t *)data;

    ///< Convert byte to nums
    if (IS_16BIT_FRAME_LEN(addr)) {
        spi->tx_size = size / 2U;
    } else {
        spi->tx_size = size;
    }

    /* configure dma channel */
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;

    if (IS_8BIT_FRAME_LEN(addr)) {
        config.src_tw = DMA_DATA_WIDTH_8_BITS;
        config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    } else if (IS_16BIT_FRAME_LEN(addr)) {
        config.src_tw = DMA_DATA_WIDTH_16_BITS;
        config.dst_tw = DMA_DATA_WIDTH_16_BITS;
    }

    config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
    config.trans_dir = DMA_MEM2PERH;
    config.handshake = usi_tx_hs_num[spi->dev.idx];
    csi_dma_ch_config(dma_ch, &config);
    /* set tx mode*/
    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)spi->dev.irq_num);
    wj_usi_set_dma_threshold_tx(addr, ((uint32_t)config.group_len / ((uint32_t)1U << (uint32_t)config.src_tw)));//WJ_USI_DMA_THRESHOLD_TX_DMA_TH_1);
    wj_usi_en_dma_ctrl_tx(addr);

    soc_dcache_clean_invalid_range((unsigned long)spi->tx_data, size);

    csi_dma_ch_start(spi->tx_dma, spi->tx_data, (uint8_t *) & (addr->USI_TX_RX_FIFO),  size);
    return CSI_OK;
}

static void dw_usi_spi_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    ck_usi_regs_t *addr;
    csi_spi_t *spi = (csi_spi_t *)dma->parent;

    addr = (ck_usi_regs_t *)spi->dev.reg_base;

    if (event == DMA_EVENT_TRANSFER_DONE) {
        /* process end of transmit */
        if ((spi->tx_dma != NULL) && (spi->tx_dma->ch_id == dma->ch_id)) {
            csi_dma_ch_stop(dma);
            wj_usi_dis_dma_ctrl_tx(addr);

            spi->state.writeable = 1U;
            spi->tx_size = 0U;

            if(wait_ready_until_timeout(spi, DW_DEFAULT_TRANSCATION_TIMEOUT) == CSI_OK){
                if (spi->callback) {
                    if (wj_usi_get_spi_ctrl_tmode(addr) == WJ_USI_SPI_CTRL_TMOD_SEND_ONLY) {
                        wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
                        drv_usi_spi_clear_rx_buf(addr);
                        spi->callback(spi, SPI_EVENT_SEND_COMPLETE, spi->arg);
                    }
                }	
            }
        } else if ((spi->rx_dma != NULL) && (spi->rx_dma->ch_id == dma->ch_id)) {
            csi_dma_ch_stop(dma);
            wj_usi_dis_dma_ctrl_rx(addr);

            spi->state.readable = 1U;
            spi->rx_size = 0U;

            if (spi->callback) {
                soc_dcache_clean_invalid_range((unsigned long)spi->rx_data, spi->rx_size);

                if (wj_usi_get_spi_ctrl_tmode(addr) == WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE) {
                    if(spi->state.writeable == 1U){
                        wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
                        drv_usi_spi_clear_rx_buf(addr);
                        spi->callback(spi, SPI_EVENT_SEND_RECEIVE_COMPLETE, spi->arg);
                    }
                } else {
                    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE);
                    drv_usi_spi_clear_rx_buf(addr);
                    spi->callback(spi, SPI_EVENT_RECEIVE_COMPLETE, spi->arg);
                }
            }
        }
    }
}

static csi_error_t dw_usi_spi_send_receive_dma(csi_spi_t *spi, const void *data_out, void *data_in, uint32_t size)
{
    csi_dma_ch_config_t config;
    ck_usi_regs_t       *addr;
    csi_dma_ch_t        *dma_ch;

    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);
    spi->tx_data = (uint8_t *)data_out;

    if (IS_16BIT_FRAME_LEN(addr)) {
        spi->tx_size = size / 2U;
        spi->rx_size = size / 2U;
        config.src_tw = DMA_DATA_WIDTH_16_BITS;
        config.dst_tw = DMA_DATA_WIDTH_16_BITS;
    } else if (IS_8BIT_FRAME_LEN(addr)) {
        spi->tx_size = size;
        spi->rx_size = size;
        config.src_tw = DMA_DATA_WIDTH_8_BITS;
        config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    }

    spi->rx_data = (uint8_t *)data_in;

    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)spi->dev.irq_num);

    /* configure tx dma channel */
    dma_ch   = (csi_dma_ch_t *)spi->tx_dma;
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
    config.trans_dir = DMA_MEM2PERH;
    config.handshake = usi_tx_hs_num[spi->dev.idx];
    csi_dma_ch_config(dma_ch, &config);
    wj_usi_set_dma_threshold_tx(addr, ((uint32_t)config.group_len / ((uint32_t)1U << (uint32_t)config.src_tw)));
    wj_usi_en_dma_ctrl_tx(addr);

    /* configure dma rx channel */
    dma_ch   = (csi_dma_ch_t *)spi->rx_dma;
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
    if (IS_16BIT_FRAME_LEN(addr)) {
	    config.group_len = 2;
    } else if (IS_8BIT_FRAME_LEN(addr)) {
	    config.group_len = 1;
    }
    config.trans_dir = DMA_PERH2MEM;
    config.handshake = usi_rx_hs_num[spi->dev.idx];
    csi_dma_ch_config(dma_ch, &config);
    wj_usi_set_dma_threshold_rx(addr, ((uint32_t)config.group_len / ((uint32_t)1U << (uint32_t)config.src_tw)));
    wj_usi_en_dma_ctrl_rx(addr);

    csi_dma_ch_start(spi->rx_dma, (uint8_t *) & (addr->USI_TX_RX_FIFO), spi->rx_data, size);
    csi_dma_ch_start(spi->tx_dma, spi->tx_data, (uint8_t *) & (addr->USI_TX_RX_FIFO), size);

    return CSI_OK;
}

static csi_error_t dw_usi_spi_receive_dma(csi_spi_t *spi,  void *data, uint32_t size)
{

    csi_dma_ch_config_t config = {0};
    ck_usi_regs_t       *addr;
    csi_dma_ch_t        *dma_ch;

    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(spi);

    if (IS_16BIT_FRAME_LEN(addr)) {

        spi->rx_size = size / 2U;
        config.src_tw = DMA_DATA_WIDTH_16_BITS;
        config.dst_tw = DMA_DATA_WIDTH_16_BITS;
    } else if (IS_8BIT_FRAME_LEN(addr)) {
        spi->rx_size = size;
        config.src_tw = DMA_DATA_WIDTH_8_BITS;
        config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    }

    spi->rx_data = (uint8_t *)data;

    /* configure dma rx channel */
    dma_ch   = (csi_dma_ch_t *)spi->rx_dma;
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.group_len = find_group_len(size, 1U << (uint8_t)config.src_tw);
    if (wj_usi_get_spi_mode(addr) != WJ_USI_SPI_MODE_MASTER){
        if (IS_16BIT_FRAME_LEN(addr)) {
            config.group_len = 2;
        } else if (IS_8BIT_FRAME_LEN(addr)) {
            config.group_len = 1;
        }
    }
    config.trans_dir = DMA_PERH2MEM;
    config.handshake = usi_rx_hs_num[spi->dev.idx];
    csi_dma_ch_config(dma_ch, &config);

    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)spi->dev.irq_num);
    wj_usi_set_dma_threshold_rx(addr, ((uint32_t)config.group_len / ((uint32_t)1U << (uint32_t)config.src_tw)));
    wj_usi_en_dma_ctrl_rx(addr);
    soc_dcache_clean_invalid_range((unsigned long)spi->rx_data, size);
    csi_dma_ch_start(spi->rx_dma, (uint8_t *) & (addr->USI_TX_RX_FIFO), spi->rx_data, size);
    wj_usi_set_spi_ctrl_tmode(addr, WJ_USI_SPI_CTRL_TMOD_RECEIVE_ONLY);
    return CSI_OK;
}

/**
  \brief       link DMA channel to spi device
  \param[in]   spi  spi handle to operate.
  \param[in]   tx_dma the DMA channel handle for send, when it is NULL means to unlink the channel
  \param[in]   rx_dma the DMA channel handle for receive, when it is NULL means to unlink the channel
  \return      error code
*/
csi_error_t csi_usi_spi_link_dma(csi_spi_t *spi, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(spi, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (tx_dma != NULL) {
        tx_dma->parent = spi;
        ret = csi_dma_ch_alloc(tx_dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(tx_dma, dw_usi_spi_dma_event_cb, NULL);
            spi->tx_dma = tx_dma;
            spi->send = dw_usi_spi_send_dma;
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
                csi_dma_ch_attach_callback(rx_dma, dw_usi_spi_dma_event_cb, NULL);
                spi->rx_dma = rx_dma;
                spi->receive = dw_usi_spi_receive_dma;
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
            spi->send_receive =  dw_usi_spi_send_receive_dma;
        }

        if ((tx_dma == NULL) || (rx_dma == NULL)) {
            spi->send_receive = (spi->callback != NULL) ? csi_usi_spi_send_receive_async : NULL;
        }
    }

    return CSI_OK;
}

/**
  \brief       get the state of spi device
  \param[in]   spi  spi handle to operate.
  \param[out]  state the state of spi device
  \return      error code
*/
csi_error_t csi_usi_spi_get_state(csi_spi_t *spi, csi_state_t *state)
{
    *state = spi->state;
    return CSI_OK;
}

#ifdef CONFIG_PM
/**
  \brief       enable spi power manage
  \param[in]   spi  spi handle to operate.
  \return      error code
*/
csi_error_t csi_usi_spi_enable_pm(csi_spi_t *spi)
{
    usi_enable_pm(&spi->dev);
    return CSI_OK;
}

/**
  \brief       disable spi power manage
  \param[in]   spi  spi handle to operate.
  \return      error code
*/
void csi_usi_spi_disable_pm(csi_spi_t *spi)
{
    usi_disable_pm(&spi->dev);
}
#endif

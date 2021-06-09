/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_usi_usart.c
 * @brief    header file for usi ll driver
 * @version  V1.0
 * @date     1. Mon 2020
 ******************************************************************************/

#include <drv/irq.h>
#include <drv/uart.h>
#include <wj_usi_ll.h>
#include <drv/usi_usart.h>
#include <drv/tick.h>
#include "wj_usi_com.h"

extern csi_error_t dw_usi_uart_send_intr(csi_uart_t *uart, const void *data, uint32_t size);
extern csi_error_t dw_usi_uart_receive_intr(csi_uart_t *uart, void *data, uint32_t size);

/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usi usart private to operate.
*/
static void ck_usi_usart_intr_transmit(csi_uart_t *usi)
{
    CSI_PARAM_CHK_NORETVAL(usi);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(usi);

    if (usi->tx_size == 0U) {
        wj_usi_en_tx_thold_mask(addr);
        wj_usi_dis_intr_tx_thold(addr);
        wj_usi_en_tx_empty_mask(addr);
        wj_usi_dis_intr_tx_empty(addr);

        if (usi->callback) {
            usi->callback(usi, UART_EVENT_SEND_COMPLETE, usi->arg);
        }
    } else {

        uint32_t txdata_num = (usi->tx_size > (uint32_t)(USI_TX_MAX_FIFO - 1U)) ? (uint32_t)(USI_TX_MAX_FIFO - 1U) : usi->tx_size;
        mdelay(1U);

        for (uint16_t i = 0U; i < txdata_num; i++) {

            addr->USI_TX_RX_FIFO = *((uint8_t *)usi->tx_data);
            usi->tx_data++;
            usi->tx_size--;

        }

        wj_usi_set_intr_clr_tx_empty_clr(addr);
    }
}

/**
  \brief        interrupt service function for receiver data available.
  \param[in]   uart usart private to operate.
*/
static void ck_usi_usart_intr_receive(csi_uart_t *uart)
{
    CSI_PARAM_CHK_NORETVAL(uart);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    if ((uart->rx_size == 0U) || (uart->rx_data == NULL)) {
        wj_usi_en_rx_thold_mask(addr);
        wj_usi_dis_intr_rx_thold(addr);
        wj_usi_en_rx_full_mask(addr);
        wj_usi_dis_intr_rx_empty(addr);
        uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, uart->arg);
    } else {
        uint32_t rxfifo_num = wj_usi_get_fifo_sta_rx_num(addr);
        uint32_t rxdata_num = (rxfifo_num > uart->rx_size) ? uart->rx_size : rxfifo_num;
        uint32_t i;

        for (i = 0U; i < rxdata_num; i++) {
            *((uint8_t *)uart->rx_data) = (uint8_t) wj_usi_get_tx_rx_fifo(addr);

            uart->rx_size--;
            uart->rx_data++;
        }

        if (uart->rx_size == 0U) {
            if (uart->callback) {

                uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, uart->arg);
            }
        }
    }
}

/**
  \brief        interrupt service function for character timeout.
  \param[in]   usart_priv usart private to operate.
*/
static void ck_usi_usart_intr_char_timeout(csi_uart_t *uart)
{
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    if ((uart->rx_size != 0U) && (uart->rx_data != NULL)) {
        ck_usi_usart_intr_receive(uart);
    } else {
        if (uart->callback) {
            uart->callback(uart, UART_EVENT_RECEIVE_FIFO_READABLE, uart->arg);
        } else {
            wj_usi_en_ctrl_rx_fifo_en(addr);
        }
    }
}

static void ck_usart_intr_recv_line(csi_uart_t *uart)
{
    CSI_PARAM_CHK_NORETVAL(uart);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    wj_usi_dis_ctrl_rx_fifo_en(addr);
    wj_usi_en_ctrl_rx_fifo_en(addr);

    if (uart->callback) {
        uart->callback(uart, UART_EVENT_ERROR_PARITY, uart->arg);
    }
}

void ck_usi_uart_irqhandler(csi_uart_t *uart)
{
    CSI_PARAM_CHK_NORETVAL(uart);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);
    uint32_t intr_state =  wj_usi_get_intr_sta(addr);

    if (intr_state & WJ_USI_INTR_STA_TX_EMPTY_Msk) {
        ck_usi_usart_intr_transmit(uart);
    }

    if (intr_state & WJ_USI_INTR_STA_TX_THOLD_Msk) {
        ck_usi_usart_intr_transmit(uart);
    }

    if (intr_state & WJ_USI_INTR_STA_RX_THOLD_Msk) {
        if (wj_usi_get_fifo_sta_rx_num(addr) > 0U) {
            ck_usi_usart_intr_receive(uart);
        } else {
            /* code */
        }
    }

    if (intr_state & WJ_USI_INTR_STA_UART_RX_STOP_Msk) {
        if (wj_usi_get_fifo_sta_rx_num(addr) > 0U) {
            ck_usi_usart_intr_char_timeout(uart);     //receive small data
        }
    }

    if (intr_state & WJ_USI_INTR_STA_UART_PERR_Msk) {
        ck_usart_intr_recv_line(uart);
    }

    wj_usi_set_intr_clr(addr, intr_state);
}

/**
  \brief       Initialize UART Interface. 1. Initializes the resources needed for the UART interface 2.registers event callback function
  \param[in]   uart      operate handle.
  \param[in]   idx       the device idx
  \param[in]   cb_event  event call back function \ref uart_event_cb_t
  \param[in]   arg       user can define it by himself
  \return      error code
*/
csi_error_t csi_usi_uart_init(csi_uart_t *uart, uint32_t idx)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    ck_usi_regs_t *addr;
    target_get(DEV_WJ_USI_TAG, idx, &uart->dev);
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);
    uart->rx_size = 0U;
    uart->rx_data = NULL;
    uart->tx_size = 0U;
    uart->tx_data = NULL;
    wj_usi_set_mode_sel(addr, WJ_USI_MODE_SEL_UART);
    wj_usi_set_ctrl(addr, WJ_USI_CTRL_USI_EN | WJ_USI_CTRL_FM_EN | WJ_USI_CTRL_TX_FIFO_EN | WJ_USI_CTRL_RX_FIFO_EN);
    return CSI_OK;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   uart  operate handle.
  \return      error code
*/
void csi_usi_uart_uninit(csi_uart_t *uart)
{
    CSI_PARAM_CHK_NORETVAL(uart);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    csi_irq_detach((uint32_t)uart->dev.irq_num);
    csi_irq_disable((uint32_t)uart->dev.irq_num);
    wj_usi_set_ctrl(addr, 0U);
    uart->callback    = NULL;
    wj_usi_set_intr_en(addr, 0U);

}

/**
  \brief       attach the callback handler to UART
  \param[in]   uart  operate handle.
  \param[in]   cb    callback function
  \param[in]   arg   user can define it by himself as callback's param
  \return      error code
*/
csi_error_t csi_usi_uart_attach_callback(csi_uart_t *uart, void *cb, void *arg)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    uart->callback   = cb;
    uart->arg = arg;
    csi_irq_attach((uint32_t)uart->dev.irq_num, &wj_usi_irq_handler, &uart->dev);
    csi_irq_enable((uint32_t)uart->dev.irq_num);
    uart->send = dw_usi_uart_send_intr;
    uart->receive = dw_usi_uart_receive_intr;

    if (cb != NULL) {
        wj_usi_set_intr_unmask(addr, WJ_USI_INTR_UNMASK_UART_STOP_MASK);
        wj_usi_en_intr_uart_rx_stop(addr);
    } else {
        wj_usi_dis_intr_rx_full(addr);
        wj_usi_dis_intr_rx_thold(addr);
    }

    return CSI_OK;
}

/**
  \brief       detach the callback handler
  \param[in]   uart  operate handle.
*/
void csi_usi_uart_detach_callback(csi_uart_t *uart)
{
    CSI_PARAM_CHK_NORETVAL(uart);
    ck_usi_regs_t *addr;

    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);
    csi_irq_detach((uint32_t)uart->dev.irq_num);
    csi_irq_disable((uint32_t)uart->dev.irq_num);
    uart->callback    = NULL;
    wj_usi_set_intr_en(addr, 0U);

}

/**
  \brief       config the baudrate.
  \param[in]   uart  uart handle to operate.
  \param[in]   baud  uart baudrate
  \return      error code
*/
csi_error_t csi_usi_uart_baud(csi_uart_t *uart, uint32_t baud)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    int32_t ret1;
    ck_usi_regs_t *uart_base;

    uart_base = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    ret1 = wj_usi_uart_config_baudrate(uart_base, uart->dev.idx, baud, soc_get_uart_freq((uint32_t)uart->dev.idx));

    if (ret1 != 0) {
        ret = CSI_ERROR;
    }
    return ret;
}

/**
  \brief       config the uart format.
  \param[in]   uart      uart handle to operate.
  \param[in]   data_bit  uart data bits
  \param[in]   parity    uart data parity
  \param[in]   stop_bit  uart stop bits
  \return      error code
*/
csi_error_t csi_usi_uart_format(csi_uart_t *uart,  csi_uart_data_bits_t data_bits,
                                csi_uart_parity_t parity, csi_uart_stop_bits_t stop_bits)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    int32_t ret1 = 0;
    ck_usi_regs_t *uart_base;

    uart_base = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    switch (data_bits) {
        case UART_DATA_BITS_5:
            wj_usi_uart_config_data_bits(uart_base, 5U);
            break;

        case UART_DATA_BITS_6:
            wj_usi_uart_config_data_bits(uart_base, 6U);
            break;

        case UART_DATA_BITS_7:
            wj_usi_uart_config_data_bits(uart_base, 7U);
            break;

        case UART_DATA_BITS_8:
            wj_usi_uart_config_data_bits(uart_base, 8U);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    if (ret == CSI_OK) {
        switch (parity) {
            case UART_PARITY_NONE:
                ret1 = wj_usi_uart_config_parity_none(uart_base);
                break;

            case UART_PARITY_ODD:
                ret1 = wj_usi_uart_config_parity_odd(uart_base);
                break;

            case UART_PARITY_EVEN:
                ret1 = wj_usi_uart_config_parity_even(uart_base);
                break;

            default:
                ret = CSI_ERROR;
                break;
        }

        if (ret == CSI_OK) {
            switch (stop_bits) {
                case UART_STOP_BITS_1:
                case UART_STOP_BITS_2:
                case UART_STOP_BITS_1_5:
                    ret1 = wj_usi_uart_config_stop_bits(uart_base, (uint32_t)stop_bits);
                    break;

                default:
                    ret = CSI_ERROR;
                    break;
            }
        }
    }

    if (ret1 != 0) {
        ret = CSI_ERROR;
    }

    return ret;
}

/**
  \brief       config the uart flow control.
  \param[in]   uart      uart handle to operate.
  \param[in]   flowctrl  uart flow control
  \return      error code
*/
csi_error_t csi_usi_uart_flowctrl(csi_uart_t *uart,  csi_uart_flowctrl_t flowctrl)
{
    return CSI_UNSUPPORTED;
}

/**
  \brief       Start sending data to UART transmitter.
  \param[in]   uart     uart handle to operate.
  \param[in]   data     Pointer to buffer with data to send to UART transmitter. data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[in]   num      Number of data items to send (byte)
  \param[in]   timeout  is the number of queries, not time
  \return      the num of data witch is send successful
*/
int32_t csi_usi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    ck_usi_regs_t *uart_base;
    uint8_t *ch = (uint8_t *)data;
    uint32_t send_num = 0U;
    uint32_t timecount = csi_tick_get_ms() + timeout ;
    uint32_t intr_en_status;
    uint32_t  timeout_flag;
    uart_base = (ck_usi_regs_t *)HANDLE_REG_BASE(uart);

    /* check data and uart */
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    /* store the status of intr */
    intr_en_status = wj_usi_get_intr_sta(uart_base);
    wj_usi_set_intr_en(uart_base, 0U); //disable all interrupt

    while (send_num < size) {
        while (wj_usi_get_fifo_sta_tx_num(uart_base) > 0U) {
            if ((csi_tick_get_ms() >= timecount)) {
                timeout_flag = 1U;
                break;
            }
        }

        if (timeout_flag == 1U) {
            break;
        } else {
            wj_usi_set_tx_rx_fifo(uart_base, (uint32_t)*ch++);
            send_num++;
        }
    }

    wj_usi_set_intr_en(uart_base, intr_en_status);
    return (int32_t)send_num;
}

csi_error_t dw_usi_uart_send_intr(csi_uart_t *uart, const void *data, uint32_t size)
{
    ck_usi_regs_t *uart_base = (ck_usi_regs_t *)uart->dev.reg_base;

    uart->tx_size = size;
    uart->tx_data = (uint8_t *)data;

    wj_usi_set_intr_clr(uart_base, ~0U);
    wj_usi_set_tx_fifo_th(uart_base, WJ_USI_INTR_CTRL_TX_FIFO_TH_4BYTE);
    wj_usi_set_rx_edge_le(uart_base);
    wj_usi_en_intr_tx_empty(uart_base);
    wj_usi_en_intr_tx_thold(uart_base);
    wj_usi_dis_tx_empty_mask(uart_base);
    wj_usi_dis_tx_thold_mask(uart_base);
    return CSI_OK;
}
/**
  \brief       Start sending data to UART transmitter (interrupt mode).
  \param[in]   uart   uart handle to operate.
  \param[in]   data     Pointer to buffer with data to send to UART transmitter. data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[in]   num      Number of data items to send
  \return      the status of send func
*/
csi_error_t csi_usi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(uart->send, CSI_ERROR);
    csi_error_t ret;
    ret = uart->send(uart, data, size);

    if (ret == CSI_OK) {
        uart->state.writeable = 0U;
    }

    return ret;
}

/**
  \brief       Get the num of data in RX_FIFO.
  \param[in]   uart   uart handle to operate.
  \return      the num of data in RX_FIFO
*/
uint32_t csi_usi_uart_get_recvfifo_waiting_num(csi_uart_t *uart)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    ck_usi_regs_t *uart_base = (ck_usi_regs_t *)uart->dev.reg_base;
    return wj_usi_get_recvfifo_waiting_num(uart_base);
}
csi_error_t dw_usi_uart_receive_intr(csi_uart_t *uart, void *data, uint32_t size)
{
    ck_usi_regs_t *uart_base = (ck_usi_regs_t *)(uart->dev.reg_base);
    uart->rx_data = data;
    uart->rx_size = size;

    wj_usi_set_intr_clr(uart_base, ~0U);

    wj_usi_set_rx_fifo_th(uart_base, WJ_USI_INTR_CTRL_RX_FIFO_TH_0BYTE);
    wj_usi_set_rx_edge_ge(uart_base);
    wj_usi_dis_rx_thold_mask(uart_base);

    wj_usi_en_intr_rx_thold(uart_base);//maybe the reg describtion give a incorrect  info.//

    return CSI_OK;
}

/**
  \brief       Start receiving data from UART receiver. \n
               This function is non-blocking,\ref uart_event_e is signaled when operation completes or error happens.
               \ref csi_uart_get_status can get operation status.
  \param[in]   uart  uart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver.data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[in]   num   Number of data items to receive
  \return      error code
*/
csi_error_t csi_usi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(uart->receive, CSI_ERROR);
    csi_error_t ret;
    ret = uart->receive(uart, data, size);

    if (ret == CSI_OK) {
        uart->state.writeable = 0U;
    }

    return ret;
}

/**
  \brief       query data from UART receiver FIFO.
  \param[in]   uart  uart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   timeout  is the number of queries, not time
  \return      fifo data num to receive
*/
int32_t csi_usi_uart_receive(csi_uart_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    uint32_t recv_num = 0U;
    uint32_t timecount = csi_tick_get_ms() + timeout ;
    uint32_t intr_en;
    uint8_t   timeout_flag = 0U;
    uint8_t *pdata = (uint8_t *)data;

    ck_usi_regs_t *uart_base = (ck_usi_regs_t *)uart->dev.reg_base;
    intr_en =   wj_usi_get_intr_en(uart_base);
    wj_usi_set_intr_en(uart_base, 0U); //disable all interrupt

    while (recv_num < size) {
        while (wj_usi_get_fifo_sta_rx_empty(uart_base) != 0U) {
            if ((csi_tick_get_ms() >= timecount)) {
                timeout_flag = 1U;
                break;
            }
        }

        if (timeout_flag == 1U) {
            break;
        } else {
            *pdata = (uint8_t) wj_usi_get_tx_rx_fifo(uart_base);
            pdata++;
            recv_num++;
        }
    }

    wj_usi_set_intr_en(uart_base, intr_en);
    return (int32_t)recv_num;
}

/**
  \brief       get character in query mode.
  \param[in]   uart  uart handle to operate.
  \param[out]  ch the pointer to the received character.
  \return      error code
*/
uint8_t  csi_usi_uart_getchar(csi_uart_t *uart)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    ck_usi_regs_t *addr = (ck_usi_regs_t *)(uart->dev.reg_base);

    while (wj_usi_get_fifo_sta_rx_num(addr) == 0U); //while

    return (uint8_t) wj_usi_get_tx_rx_fifo(addr);
}

/**
  \brief       transmit character in query mode.
  \param[in]   uart  uart handle to operate.
  \param[in]   ch  the input character
  \return      error code
*/
void csi_usi_uart_putchar(csi_uart_t *uart, uint8_t ch)
{
    CSI_PARAM_CHK_NORETVAL(uart);
    ck_usi_regs_t *addr = (ck_usi_regs_t *)(uart->dev.reg_base);

    wj_usi_set_tx_rx_fifo(addr, (uint32_t)ch);

    while (!(wj_usi_get_fifo_sta_tx_empty(addr)&  WJ_USI_FIFO_STA_TX_EMPTY));
}

static void dw_usi_uart_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    csi_uart_t *uart = (csi_uart_t *)dma->parent;

    if (event == DMA_EVENT_TRANSFER_ERROR) {/* DMA transfer ERROR */
        if ((uart->tx_dma != NULL) && (uart->tx_dma->ch_id == dma->ch_id)) {
            csi_dma_ch_stop(dma);

            uart->state.writeable = 1U;

            if (uart->callback) {
                uart->callback(uart, UART_EVENT_ERROR_OVERFLOW, uart->arg);
            }
        } else {
            csi_dma_ch_stop(dma);
            /* enable received data available */
            csi_irq_enable((uint32_t)uart->dev.irq_num);

            uart->state.readable = 1U;

            if (uart->callback) {
                uart->callback(uart, UART_EVENT_ERROR_FRAMING, uart->arg);
            }
        }
    } else if (event == DMA_EVENT_TRANSFER_DONE) {/* DMA transfer complete */
        if ((uart->tx_dma != NULL) && (uart->tx_dma->ch_id == dma->ch_id)) {

            csi_dma_ch_stop(dma);
            uart->state.writeable = 1U;

            if (uart->callback) {
                uart->callback(uart, UART_EVENT_SEND_COMPLETE, uart->arg);
            }
        } else {
            csi_dma_ch_stop(dma);

            /* enable received data available */
            csi_irq_enable((uint32_t)uart->dev.irq_num);
            uart->state.readable = 1U;

            if (uart->callback) {
                uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, uart->arg);
            }
        }
    }

}

csi_error_t dw_usi_uart_send_dma(csi_uart_t *uart, const void *data, uint32_t size)
{

    csi_dma_ch_config_t config;
    ck_usi_regs_t *addr = (ck_usi_regs_t *)(uart->dev.reg_base);
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)uart->tx_dma;

    uart->tx_data = (void *)data;
    uart->tx_size = size;

    // close interrupt
    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)uart->dev.irq_num);
    wj_usi_set_intr_en(addr, 0U);
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    /* config for wj_dma */
    config.group_len = 1;
    config.trans_dir = DMA_MEM2PERH;
    /* config for etb */
    config.handshake = usi_tx_hs_num[uart->dev.idx];
    wj_usi_en_dma_ctrl_tx(addr);
    wj_usi_set_dma_threshold_tx(addr, WJ_USI_DMA_THRESHOLD_TX_DMA_TH_1);
    csi_dma_ch_config(dma_ch, &config);

    csi_dma_ch_start(uart->tx_dma, uart->tx_data, (uint8_t *) & (addr->USI_TX_RX_FIFO), uart->tx_size);
    return CSI_OK;

}

csi_error_t dw_usi_uart_receive_dma(csi_uart_t *uart, void *data, uint32_t size)
{

    csi_dma_ch_config_t config;
    csi_error_t ret;
    ck_usi_regs_t *addr = (ck_usi_regs_t *)(uart->dev.reg_base);
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)uart->rx_dma;
    uart->rx_data = (void *)data;
    uart->rx_size = size;
    wj_usi_set_intr_clr(addr, ~0U);
    csi_irq_disable((uint32_t)uart->dev.irq_num);
    wj_usi_set_intr_en(addr, 0U);
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    config.group_len = 1;
    config.trans_dir = DMA_PERH2MEM;
    config.handshake = usi_rx_hs_num[uart->dev.idx];

    wj_usi_en_dma_ctrl_rx(addr);
    wj_usi_set_dma_threshold_rx(addr, WJ_USI_DMA_THRESHOLD_RX_DMA_TH_1);
    ret = csi_dma_ch_config(dma_ch, &config);

    if (ret == CSI_OK) {
        csi_dma_ch_start(uart->rx_dma, (uint8_t *) & (addr->USI_TX_RX_FIFO), uart->rx_data, uart->rx_size);
    }

    return ret;
}

csi_error_t csi_usi_uart_link_dma(csi_uart_t *uart, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    if (tx_dma != NULL) {
        tx_dma->parent = uart;
        ret = csi_dma_ch_alloc(tx_dma, -1, -1);

        if (ret == CSI_OK) {
            csi_dma_ch_attach_callback(tx_dma, dw_usi_uart_dma_event_cb, NULL);
            uart->tx_dma = tx_dma;
            uart->send = dw_usi_uart_send_dma;
        } else {
            tx_dma->parent = NULL;
        }
    } else {
        if (uart->tx_dma) {
            csi_dma_ch_detach_callback(uart->tx_dma);
            csi_dma_ch_free(uart->tx_dma);
            uart->tx_dma = NULL;
        }

        uart->send = dw_usi_uart_send_intr;
    }

    if (ret == CSI_OK) {
        if (rx_dma != NULL) {
            rx_dma->parent = uart;
            ret = csi_dma_ch_alloc(rx_dma, -1, -1);

            if (ret == CSI_OK) {
                csi_dma_ch_attach_callback(rx_dma, dw_usi_uart_dma_event_cb, NULL);
                uart->rx_dma = rx_dma;
                uart->receive = dw_usi_uart_receive_dma;
            } else {
                rx_dma->parent = NULL;
            }
        } else {
            if (uart->rx_dma) {
                csi_dma_ch_detach_callback(uart->rx_dma);
                csi_dma_ch_free(uart->rx_dma);
                uart->rx_dma = NULL;
            }

            uart->receive = dw_usi_uart_receive_intr;
        }
    }

    return ret;
}

#ifdef CONFIG_PM
/**
  \brief       Enable uart power manage.
  \param[in]   uart   uart handle to operate.
  \return      error code.
*/
csi_error_t csi_usi_uart_enable_pm(csi_uart_t *uart)
{
    usi_enable_pm(&uart->dev);
    return CSI_OK;
}

/**
  \brief       Disable uart power manage.
  \param[in]   uart   uart handle to operate.
*/
void csi_usi_uart_disable_pm(csi_uart_t *uart)
{
    usi_disable_pm(&uart->dev);
}

#endif

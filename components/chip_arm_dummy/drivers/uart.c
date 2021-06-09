/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <csi_core.h>
#include <drv/uart.h>
#include <drv/dma.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <soc.h>
#include <cmsdk_uart_ll.h>

void serial_init(CMSDK_UART_TypeDef *uart_base)
{
    uart_base->CTRL = 0x00;    // Disable UART when changing configuration
    uart_base->CTRL |= 0x01;  // TX enable
    uart_base->CTRL |= 0x02;  // RX enable
}

void serial_baud(CMSDK_UART_TypeDef *uart_base, uint32_t uart_freq, int baudrate)
{
    int div = 0;
    div = uart_freq / baudrate;
    if (baudrate >= 16) {
       uart_base->BAUDDIV = div;
    }
}

int serial_writable(CMSDK_UART_TypeDef *uart_base)
{
    return !(uart_base->STATE & 0x1);
}

int serial_readable(CMSDK_UART_TypeDef *uart_base)
{
    return uart_base->STATE & 0x2;
}

void serial_putc(CMSDK_UART_TypeDef *uart_base, int c)
{
    while (serial_writable(uart_base) == 0);
    uart_base->DATA = c;
}

int serial_getc(CMSDK_UART_TypeDef *uart_base)
{
    while (serial_readable(uart_base) == 0);
    int data = uart_base->DATA;
    return data;
}

csi_error_t csi_uart_init(csi_uart_t *uart, uint32_t idx)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    CMSDK_UART_TypeDef *uart_base;

    ret = target_get(DEV_CMSDK_UART_TAG, idx, &uart->dev);

    if (ret == CSI_OK) {
        uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);
        uart->rx_size = 0U;
        uart->tx_size = 0U;
        uart->rx_data = NULL;
        uart->tx_data = NULL;
        uart->tx_dma  = NULL;
        uart->rx_dma  = NULL;

        serial_init(uart_base);
    }

    return ret;
}

void csi_uart_uninit(csi_uart_t *uart)
{
}

csi_error_t csi_uart_baud(csi_uart_t *uart, uint32_t baud)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    serial_baud(uart_base, soc_get_uart_freq(uart->dev.idx), baud);

    return ret;
}

csi_error_t csi_uart_format(csi_uart_t *uart,  csi_uart_data_bits_t data_bits,
                            csi_uart_parity_t parity, csi_uart_stop_bits_t stop_bits)
{
    return CSI_OK;
}

void csi_uart_putc(csi_uart_t *uart, uint8_t ch)
{
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    serial_putc(uart_base, ch);
}

int32_t csi_uart_receive(csi_uart_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    uint8_t *temp_data = (uint8_t *)data;
    int32_t recv_num = 0;

    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    while (recv_num < (int32_t)size) {
        if (serial_readable(uart_base)) {
            *temp_data = (uint8_t)uart_base->DATA;
            temp_data++;
            recv_num++;
        } else {
            break;
        }
    }

    return recv_num;
}

csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
    CSI_PARAM_CHK(uart->receive, CSI_ERROR);

    csi_error_t ret;

    ret = uart->receive(uart, data, size);

    if (ret == CSI_OK) {
        uart->state.readable = 0U;
    }

    return ret;
}

int32_t csi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    return 0;
}

csi_error_t mps2_uart_send_intr(csi_uart_t *uart, const void *data, uint32_t size)
{
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    uart->tx_data = (uint8_t *)data;
    uart->tx_size = size;
    uart_base->CTRL |= CMSDK_UART_CTRL_TXIRQEN_Msk;

    return CSI_OK;
}

csi_error_t csi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
    CSI_PARAM_CHK(uart->send, CSI_ERROR);

    /* sync simulate async */
    for (int i = 0; i < size; i++) {
        csi_uart_putc(uart, *(uint8_t *)((uint32_t)data + i));
    }

    if (uart->callback) {
        uart->callback(uart, UART_EVENT_SEND_COMPLETE, uart->arg);
    }

    return CSI_OK;
}

void mps2_uart_intr_recv_data(csi_uart_t *uart)
{
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    if ((uart->rx_data == NULL) || (uart->rx_size == 0U)) {
        if (uart->callback) {
            uart->callback(uart, UART_EVENT_RECEIVE_FIFO_READABLE, uart->arg);
        }
    }

    uart_base->INTCLEAR |= 0x02;
}

void mps2_uart_irq_handler(void *arg)
{
    csi_uart_t *uart = (csi_uart_t *)arg;

    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    uint32_t intstatus;

    intstatus = uart_base->INTSTATUS & 0x3;

    switch (intstatus) {
        case 2:
            mps2_uart_intr_recv_data(uart);
            break;
        default:
            break;
    }
}

csi_error_t csi_uart_attach_callback(csi_uart_t *uart, void  *callback, void *arg)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    CMSDK_UART_TypeDef *uart_base;
    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);
    uart_base->CTRL |= CMSDK_UART_CTRL_RXIRQEN_Msk;

    uart->callback = callback;
    uart->arg = arg;
    uart->send = mps2_uart_send_intr;
    uart->receive = NULL;

    csi_irq_attach((uint32_t)(uart->dev.irq_num), &mps2_uart_irq_handler, &uart->dev);
    csi_irq_attach((uint32_t)(uart->dev.irq_num + 1), &mps2_uart_irq_handler, &uart->dev);
    csi_irq_enable((uint32_t)(uart->dev.irq_num));
    csi_irq_enable((uint32_t)(uart->dev.irq_num + 1));

    return CSI_OK;
}

void csi_uart_detach_callback(csi_uart_t *uart)
{
}

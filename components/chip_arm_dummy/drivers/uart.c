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
    ///< 获取中断号、基地址等相关信息
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
    CSI_PARAM_CHK_NORETVAL(uart);

    uart->rx_size = 0U;
    uart->tx_size = 0U;
    uart->rx_data = NULL;
    uart->tx_data = NULL;

    ///< TODO：关闭发送中断
    ///< TODO：关闭接收中断
    
    ///< 禁止中断控制器的对应的中断，注销中断服务函数
    csi_irq_disable((uint32_t)(uart->dev.irq_num));
    csi_irq_detach((uint32_t)(uart->dev.irq_num));
}

csi_error_t csi_uart_baud(csi_uart_t *uart, uint32_t baud)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    serial_baud(uart_base, soc_get_uart_freq(uart->dev.idx), baud); ///< TODO：配置串口的波特率

    return ret;
}

csi_error_t csi_uart_format(csi_uart_t *uart,  csi_uart_data_bits_t data_bits,
                            csi_uart_parity_t parity, csi_uart_stop_bits_t stop_bits)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);

    csi_error_t csi_ret = CSI_OK;

    ///< TODO：设置数据宽度

    ///< TODO：设置校验位

    ///< TODO：设置停止位

    return csi_ret;
}

csi_error_t csi_uart_flowctrl(csi_uart_t *uart,  csi_uart_flowctrl_t flowctrl)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    csi_error_t csi_ret = CSI_OK;

    switch (flowctrl) {
        case UART_FLOWCTRL_CTS:
            ///< TODO：设置发送自动流控
            break;

        case UART_FLOWCTRL_RTS_CTS:
            ///< TODO：设置接收自动流控
            ///< TODO：设置发送自动流控
            break;

        case UART_FLOWCTRL_RTS:
            ///< TODO：设置接收自动流控
            break;
        case UART_FLOWCTRL_NONE:
            ///< TODO：关闭自动流控
            break;

        default:
            csi_ret = CSI_UNSUPPORTED;
            break;
    }

    return csi_ret;
}

void csi_uart_putc(csi_uart_t *uart, uint8_t ch)
{
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    serial_putc(uart_base, ch); ///< TODO：发送数据
}

uint8_t csi_uart_getc(csi_uart_t *uart)
{
    CSI_PARAM_CHK(uart, 0U);
    uint8_t rece_data = 0U;

    ///< TODO：有数据来了，读取一个字符到rece_data

    return rece_data; 
}

int32_t csi_uart_receive(csi_uart_t *uart, void *data, uint32_t size, uint32_t timeout)
{
    uint8_t *temp_data = (uint8_t *)data;
    int32_t recv_num = 0;

    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    while (recv_num < (int32_t)size) {
        if (serial_readable(uart_base)) {
            *temp_data = (uint8_t)uart_base->DATA; ///< TODO：读数据寄存器
            temp_data++;
            recv_num++;
        } else {
            break;
        }
    }

    return recv_num;
}

csi_error_t mps2_uart_receive_intr(csi_uart_t *uart, void *data, uint32_t num)
{
    uart->rx_data = (uint8_t *)data;
    uart->rx_size = num;

    ///< TODO：使能接收数据中断

    return CSI_OK;
}

csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
    CSI_PARAM_CHK(uart->receive, CSI_ERROR);

    csi_error_t ret;

    ///< 调用中断或dma接收接口
    ret = uart->receive(uart, data, size);

    if (ret == CSI_OK) {
        uart->state.readable = 0U;
    }

    return ret;
}

int32_t csi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    ///< check data and uart
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);

    int32_t trans_num = 0;

    ///< TODO：关闭发送数据中断

    ///< TODO：在一定时间内串口不繁忙则写数据寄存器发送数据，发送的长度记为trans_num；超时则超时退出

    return trans_num;
}

csi_error_t mps2_uart_send_intr(csi_uart_t *uart, const void *data, uint32_t size)
{
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    uart->tx_data = (uint8_t *)data;
    uart->tx_size = size;
    uart_base->CTRL |= CMSDK_UART_CTRL_TXIRQEN_Msk; ///< TODO：开启发送数据中断

    return CSI_OK;
}

csi_error_t csi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
    CSI_PARAM_CHK(uart->send, CSI_ERROR);

    ///< sync simulate async
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

    ///< TODO：接收数据

    if ((uart->rx_data == NULL) || (uart->rx_size == 0U)) {
        if (uart->callback) {
            uart->callback(uart, UART_EVENT_RECEIVE_FIFO_READABLE, uart->arg);
        }
    }

    uart_base->INTCLEAR |= 0x02;
}

void mps2_uart_intr_send_data(csi_uart_t *uart)
{
    CMSDK_UART_TypeDef *uart_base;

    uart_base = (CMSDK_UART_TypeDef *)HANDLE_REG_BASE(uart);

    ///< TODO：发送数据

    if ((uart->rx_data == NULL) || (uart->rx_size == 0U)) {
        if (uart->callback) {
            uart->callback(uart, UART_EVENT_SEND_COMPLETE, uart->arg);
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
        case 1:
            mps2_uart_intr_send_data(uart);
            break;

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
    CSI_PARAM_CHK_NORETVAL(uart);

    uart->callback  = NULL;
    uart->arg = NULL;
    uart->send = NULL;
    uart->receive = NULL;
    ///< TODO：关uart接收中断
    ///< 关闭中断控制器中对应的中断，注销中断服务函数
    csi_irq_disable((uint32_t)(uart->dev.irq_num));
    csi_irq_disable((uint32_t)(uart->dev.irq_num + 1));
    csi_irq_detach((uint32_t)(uart->dev.irq_num));
    csi_irq_detach((uint32_t)(uart->dev.irq_num + 1));
}

static void dw_uart_dma_event_cb(csi_dma_ch_t *dma, csi_dma_event_t event, void *arg)
{
    csi_uart_t *uart = (csi_uart_t *)dma->parent;

    ///< DMA 发送错误 处理
    if (event == DMA_EVENT_TRANSFER_ERROR) { /// DMA transfer ERROR
        if ((uart->tx_dma != NULL) && (uart->tx_dma->ch_id == dma->ch_id)) {
            ///< 关闭DMA通道
            csi_dma_ch_stop(dma);
            ///< TODO：关闭串口发送数据的ETB功能
            ///< TODO：串口fifo的初始化

            uart->state.writeable = 1U;

            ///< 调用用户回调，反馈结果给上层调用
            if (uart->callback) {
                uart->callback(uart, UART_EVENT_ERROR_OVERFLOW, uart->arg);
            }
        } else {
            ///< 关闭DMA通道
            csi_dma_ch_stop(dma);
            ///< TODO：关闭串口接收数据的ETB功能
            ///< TODO：串口fifo的初始化

            ///< TODO：使能接收数据中断

            uart->state.readable = 1U;

            ///< 调用用户回调，反馈结果给上层调用
            if (uart->callback) {
                uart->callback(uart, UART_EVENT_ERROR_FRAMING, uart->arg);
            }
        }
    } else if (event == DMA_EVENT_TRANSFER_DONE) { //// DMA transfer complete
        if ((uart->tx_dma != NULL) && (uart->tx_dma->ch_id == dma->ch_id)) {
            ///< TODO：等待数据发送完成

            ///< TODO：关闭串口发送数据的ETB功能
            ///< 关闭DMA通道
            csi_dma_ch_stop(dma);
            ///< TODO：关闭串口接收数据的ETB功能

            uart->state.writeable = 1U;

            ///< 调用用户回调，反馈结果给上层调用
            if (uart->callback) {
                uart->callback(uart, UART_EVENT_SEND_COMPLETE, uart->arg);
            }
        } else {
            ///< TODO：关闭串口接收数据的ETB功能
            //< 关闭DMA通道
            csi_dma_ch_stop(dma);
            ///< TODO：关闭串口接收数据的ETB功能
            ///< TODO：使能接收数据中断

            uart->state.readable = 1U;

            ///< 调用回调，反馈结果给上层调用
            if (uart->callback) {
                uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, uart->arg);
            }

        }
    }
}

csi_error_t dw_uart_send_dma(csi_uart_t *uart, const void *data, uint32_t num)
{
    csi_dma_ch_config_t config = {0};
    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    csi_dma_ch_t *dma_ch = (csi_dma_ch_t *)uart->tx_dma;

    uart->tx_data = (uint8_t *)data;
    uart->tx_size = num;
    ///< TODO：关闭串口收发中断

    ///< 设置DMA 参数
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_CONSTANT;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    config.trans_dir = DMA_MEM2PERH;
    ///< 配置串口DMA的硬件握手号config.handshake

    csi_dma_ch_config(dma_ch, &config);

    soc_dcache_clean_invalid_range((unsigned long)uart->tx_data, uart->tx_size);
    ///< TODO：设置FIFO的触发级别
    ///< TODO：调用csi_dma_ch_start开始DMA传输 

    return CSI_OK;
}

csi_error_t dw_uart_receive_dma(csi_uart_t *uart, void *data, uint32_t num)
{
    csi_dma_ch_config_t config = {0};
    memset(&config, 0, sizeof(csi_dma_ch_config_t));
    csi_error_t ret = CSI_OK;
    csi_dma_ch_t *dma = (csi_dma_ch_t *)uart->rx_dma;
    ///< TODO：关闭串口收发中断
    
    ///< 设置DMA 参数
    uart->rx_data = (uint8_t *)data;
    uart->rx_size = num;
    config.src_inc = DMA_ADDR_CONSTANT;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    config.trans_dir = DMA_PERH2MEM;
    ///< 配置串口DMA的硬件握手号config.handshake

    csi_dma_ch_config(dma, &config);

    soc_dcache_clean_invalid_range((unsigned long)uart->rx_data, uart->rx_size);
    ///< TODO：设置FIFO的触发级别
    ///< TODO：调用csi_dma_ch_start开始DMA传输

    return ret;
}

csi_error_t csi_uart_link_dma(csi_uart_t *uart, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    if (tx_dma != NULL) {
        ///< 获取一个dma通道到tx_dma
        tx_dma->parent = uart;
        ret = csi_dma_ch_alloc(tx_dma, -1, -1);

        if (ret == CSI_OK) {
            ///< 获取一个dma通道成功后设置DMA回调函数及dma发送函数
            csi_dma_ch_attach_callback(tx_dma, dw_uart_dma_event_cb, NULL);
            uart->tx_dma = tx_dma;
            uart->send = dw_uart_send_dma;
        } else {
            tx_dma->parent = NULL;
        }
    } else {
        if (uart->tx_dma) {
            ///< 注销DMA传输完成回调函数
            csi_dma_ch_detach_callback(uart->tx_dma);
            ///< 释放dma 通道
            csi_dma_ch_free(uart->tx_dma);
            uart->tx_dma = NULL;
        }

        ///< 设置 uart  发送接口为中断发送接口
        uart->send = mps2_uart_send_intr;
    }

    if (ret != CSI_ERROR) {
        if (rx_dma != NULL) {
            rx_dma->parent = uart;
            ret = csi_dma_ch_alloc(rx_dma, -1, -1);

            if (ret == CSI_OK) {
                csi_dma_ch_attach_callback(rx_dma, dw_uart_dma_event_cb, NULL);
                uart->rx_dma = rx_dma;
                uart->receive = dw_uart_receive_dma;
            } else {
                rx_dma->parent = NULL;
            }
        } else {
            if (uart->rx_dma) {
                csi_dma_ch_detach_callback(uart->rx_dma);
                csi_dma_ch_free(uart->rx_dma);
                uart->rx_dma = NULL;
            }

            uart->receive = mps2_uart_receive_intr;
        }
    }

    return ret;
}

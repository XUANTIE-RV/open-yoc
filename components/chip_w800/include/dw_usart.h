/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     dw_usart.h
 * @brief    header file for usart driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef __DW_USART_H
#define __DW_USART_H

#include <soc.h>


#define UART_MAX_FIFO          32

#define UART_RX_INT_FLAG (UIS_RX_FIFO | UIS_RX_FIFO_TIMEOUT | UIS_BREAK |\
                          UIS_OVERRUN | UIS_FRM_ERR | UIS_PARITY_ERR)
#define UART_RX_ERR_INT_FLAG (UIS_BREAK | UIS_FRM_ERR | \
                              UIS_PARITY_ERR)

#define UART_TX_INT_FLAG (UIS_TX_FIFO | UIS_TX_FIFO_EMPTY)

#define UART_TXEN_BIT           (0x40)
#define UART_RXEN_BIT           (0x80)
#define UART_PARITYEN_BIT       (0x08)
#define UART_PARITYODD_BIT      (0x10)
#define UART_BITSTOP_VAL        (0x03)                  /// 1 stop-bit; no crc; 8 data-bits

#define ERR_UART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

enum {
    WM_UART_0 = 0,
    WM_UART_1 = 1,
    WM_UART_2 = 2,
    WM_UART_3 = 3,
    WM_UART_4 = 4,
    WM_UART_MAX = 5,
};


typedef struct {
    __IOM uint32_t UR_LC;
    __IOM uint32_t UR_FC;
    __IOM uint32_t UR_DMAC;
    __IOM uint32_t UR_FIFOC;
    __IOM uint32_t UR_BD;
    __IOM uint32_t UR_INTM;
    __IOM uint32_t UR_INTS;
    __IOM uint32_t UR_FIFOS;
    __IOM uint32_t UR_TXW;                    /**< tx windows register */
    __IOM uint32_t UR_RES0;
    __IOM uint32_t UR_RES1;
    __IOM uint32_t UR_RES2;
    __IOM uint32_t UR_RXW;
} dw_usart_reg_t;

typedef struct {
    uint32_t base;
    uint32_t irq;
    usart_event_cb_t cb_event;           ///< Event callback
    int32_t idx;

    uint32_t rx_total_num;
    uint32_t tx_total_num;

    uint8_t *rx_buf;
    uint8_t *tx_buf;

    volatile uint32_t rx_cnt;
    volatile uint32_t tx_cnt;

    volatile uint32_t tx_busy;
    volatile uint32_t rx_busy;

    uint32_t last_tx_num;
    uint32_t last_rx_num;
} dw_usart_priv_t;



#endif /* __DW_USART_H */


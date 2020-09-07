/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _SILAN_USART_H_
#define _SILAN_USART_H_

#include <soc.h>
#include <addrspace.h>

#define UART_BUSY_TIMEOUT      1000000

#define SILAN_UART_MAX_FIFO    16 //UART1 has 64 bytes depth

#define SILAN_UARTFR_TXFE                 0x80
#define SILAN_UARTFR_RXFF                 0x40
#define SILAN_UARTFR_TXFF                 0x20
#define SILAN_UARTFR_RXFE                 0x10
#define SILAN_UARTFR_BUSY                 0x08

#define SILAN_UARTLCR_H_WLEN_8            (3 << 5)
#define SILAN_UARTLCR_H_WLEN_7            (2 << 5)
#define SILAN_UARTLCR_H_WLEN_6            (1 << 5)
#define SILAN_UARTLCR_H_WLEN_5            (0 << 5)
#define SILAN_UARTLCR_H_FEN               (1 << 4)
#define SILAN_UARTLCR_H_STP2              (1 << 3)
#define SILAN_UARTLCR_H_EPS               (1 << 2)
#define SILAN_UARTLCR_H_PEN               (1 << 1)
#define SILAN_UARTLCR_H_BRK               (1 << 0)

#define SILAN_UARTCR_RXE                  (1 << 9)
#define SILAN_UARTCR_TXE                  (1 << 8)
#define SILAN_UARTCR_SIREN                (1 << 1)
#define SILAN_UARTCR_UARTEN               (1 << 0)

/*UART_RSR*/
#define SILAN_UART_RSR_OE                 (1 << 0)
#define SILAN_UART_RSR_BE                 (1 << 1)
#define SILAN_UART_RSR_PE                 (1 << 2)
#define SILAN_UART_RSR_FE                 (1 << 3)


/*UART_FR*/
#define SILAN_UART_FR_CTS                 (1 << 0)
#define SILAN_UART_FR_DSR                 (1 << 1)
#define SILAN_UART_FR_DCD                 (1 << 2)
#define SILAN_UART_FR_BUSY                (1 << 3)
#define SILAN_UART_FR_RXFE                (1 << 4)
#define SILAN_UART_FR_TXFF                (1 << 5)
#define SILAN_UART_FR_RXFF                (1 << 6)
#define SILAN_UART_FR_TXFE                (1 << 7)
#define SILAN_UART_FR_RI                  (1 << 8)


/*UART_LCRH*/
#define SILAN_UART_LCRH_BRK               (1 << 0)
#define SILAN_UART_LCRH_PEN               (1 << 1)
#define SILAN_UART_LCRH_EPS               (1 << 2)
#define SILAN_UART_LCRH_STP2              (1 << 3)
#define SILAN_UART_LCRH_FEN               (1 << 4)
#define SILAN_UART_LCRH_WLEN_5            (0 << 5)
#define SILAN_UART_LCRH_WLEN_6            (1 << 5)
#define SILAN_UART_LCRH_WLEN_7            (2 << 5)
#define SILAN_UART_LCRH_WLEN_8            (3 << 5)
#define SILAN_UART_LCRH_SPS               (1 << 7)

/*UART_CR*/
#define SILAN_UART_CR_UARTEN              (1 << 0)
#define SILAN_UART_CR_SIREN               (1 << 1)
#define SILAN_UART_CR_SIRLP               (1 << 2)
#define SILAN_UART_CR_LPE                 (1 << 7)
#define SILAN_UART_CR_TXE                 (1 << 8)
#define SILAN_UART_CR_RXE                 (1 << 9)
#define SILAN_UART_CR_DTS                 (1 << 10)
#define SILAN_UART_CR_RTS                 (1 << 11)
#define SILAN_UART_CR_OUT1                (1 << 12)
#define SILAN_UART_CR_OUT2                (1 << 13)
#define SILAN_UART_CR_RTSEN               (1 << 14)
#define SILAN_UART_CR_CTSEN               (1 << 15)


/*UART_FLS*/
#define SILAN_UART_FLS_TXIFLSEL_1         (0 << 0)
#define SILAN_UART_FLS_TXIFLSEL_2         (1 << 0)
#define SILAN_UART_FLS_TXIFLSEL_4         (2 << 0)
#define SILAN_UART_FLS_TXIFLSEL_6         (3 << 0)
#define SILAN_UART_FLS_TXIFLSEL_7         (4 << 0)
#define SILAN_UART_FLS_RXIFLSEL_1         (0 << 3)
#define SILAN_UART_FLS_RXIFLSEL_2         (1 << 3)
#define SILAN_UART_FLS_RXIFLSEL_4         (2 << 3)
#define SILAN_UART_FLS_RXIFLSEL_6         (3 << 3)
#define SILAN_UART_FLS_RXIFLSEL_7         (4 << 3)

#define SILAN_UART_FLS_TX_RX_SEL          (0x3f)

#define SILAN_UART_INT_OE                 (1 << 10)
#define SILAN_UART_INT_BE                 (1 << 9)
#define SILAN_UART_INT_PE                 (1 << 8)
#define SILAN_UART_INT_FE                 (1 << 7)
#define SILAN_UART_INT_RT                 (1 << 6)
#define SILAN_UART_INT_TX                 (1 << 5)
#define SILAN_UART_INT_RX                 (1 << 4)
#define SILAN_UART_INT_DSR                (1 << 3)
#define SILAN_UART_INT_DCD                (1 << 2)
#define SILAN_UART_INT_CTS                (1 << 1)
#define SILAN_UART_INT_RI                 (1 << 0)

typedef struct {
    __IOM uint32_t UARTDR;              /* Offset: 0x000 (R/W) uart data register */
    __IOM uint32_t UARTRSR;             /* Offset: 0x004 (R/W) receive status register */
    uint32_t RESERVED1[4];
    __IM uint32_t UARTFR;               /* Offset: 0x018 (R/) uart flag register */
    uint32_t RESERVED2[1];
    __IOM uint32_t UARTILPR;            /* Offset: 0x020 (R/W) lrDA low-power conter register */
    __IOM uint32_t UARTIBRD;            /* Offset: 0x024 (R/W) interger baud rate register */
    __IOM uint32_t UARTFBRD;            /* Offset: 0x028 (R/W) fractional baud rate register */
    __IOM uint32_t UARTLCR_H;           /* Offset: 0x02c (R/W) uart line control register */
    __IOM uint32_t UARTCR;              /* Offset: 0x030 (R/W) uart control register */
    __IOM uint32_t UARTIFLS;            /* Offset: 0x034 (R/W) uart interrupt FIFO level select register */
    __IOM uint32_t UARTIMSC;            /* Offset: 0x038 (R/W) uart interrupt mask setting and clearing register */
    __IM uint32_t UARTRIS;             /* Offset: 0x03C (R/W) uart raw interrupt status register */
    __IM uint32_t UARTMIS;             /* Offset: 0x03C (R/W) uart interrupt status which was masked register */
    __OM uint32_t UARTICR;             /* Offset: 0x040 (R/W) uart interrupt clear register */
} silan_usart_reg_t;

#endif /* _SILAN_USART_H_ */

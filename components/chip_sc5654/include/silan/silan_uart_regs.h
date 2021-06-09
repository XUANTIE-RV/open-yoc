/*
 * silan_uart_regs.h
 *
 * Modify Date: 2016-4-28 18:10
 */
#ifndef __SILAN_UART_REGS_H__
#define __SILAN_UART_REGS_H__

#include "silan_types.h"
#include "ap1508.h"

#define SILAN_UART1_PHY_BASE        SPER_APB1_UARTHS_BASE
#define SILAN_UART2_PHY_BASE        SPER_APB1_UART2_BASE
#define SILAN_UART3_PHY_BASE        SPER_APB1_UART3_BASE
#define SILAN_UART4_PHY_BASE        SPER_APB1_UART4_BASE

#define UART1_BASE                  SILAN_UART1_PHY_BASE
#define UART2_BASE                  SILAN_UART2_PHY_BASE
#define UART3_BASE                  SILAN_UART3_PHY_BASE
#define UART4_BASE                  SILAN_UART4_PHY_BASE

#define SILAN_UART_BASE(x)  		(SPER_APB1_UARTHS_BASE + (x-1)*0x10000)

#define UART_DR                     0x00     /*  Data read or written from the interface. */
#define UART_RSR                    0x04     /*  Receive status register (Read). */
#define UART_FR                     0x18     /*  Flag register (Read only). */
#define UART_ILPR                   0x20
#define UART_IBRD                   0X24
#define UART_FBRD                   0X28
#define UART_LCRH                   0X2C
#define UART_CR                     0X30
#define UART_FLS                    0X34
#define UART_INT				    0X38
#define UART_RIS				    0X3C
#define UART_INT_STA			    0X40
#define UART_INT_CLR			    0X44
#define UART_DMA_CR			        0X48
#define UART_TCR			        0X80
#define UART_TITIP			        0X84
#define UART_TITOP			        0X88
#define UART_TDR			        0X8C
#define UART_PERIPHID0			    0XFE0
#define UART_PERIPHID1			    0XFE4
#define UART_PERIPHID2			    0XFE8
#define UART_PERIPHID3			    0XFEC
#define UART_PCELLID0			    0XFF0
#define UART_PCELLID1			    0XFF4
#define UART_PCELLID2			    0XFF8
#define UART_PCELLID3			    0XFFC

/*UART_RSR*/
#define UART_RSR_OE                 (1 << 0)
#define UART_RSR_BE                 (1 << 1)
#define UART_RSR_PE                 (1 << 2)
#define UART_RSR_FE                 (1 << 3)


/*UART_FR*/
#define UART_FR_CTS                 (1 << 0)
#define UART_FR_DSR                 (1 << 1)
#define UART_FR_DCD                 (1 << 2)
#define UART_FR_BUSY                (1 << 3)
#define UART_FR_RXFE                (1 << 4)
#define UART_FR_TXFF                (1 << 5)
#define UART_FR_RXFF                (1 << 6)
#define UART_FR_TXFE                (1 << 7)
#define UART_FR_RI                  (1 << 8)


/*UART_LCRH*/
#define UART_LCRH_BRK               (1 << 0)
#define UART_LCRH_PEN               (1 << 1)
#define UART_LCRH_EPS               (1 << 2)
#define UART_LCRH_STP2              (1 << 3)
#define UART_LCRH_FEN               (1 << 4)
#define UART_LCRH_WLEN_5            (0 << 5)
#define UART_LCRH_WLEN_6            (1 << 5)
#define UART_LCRH_WLEN_7            (2 << 5)
#define UART_LCRH_WLEN_8            (3 << 5)
#define UART_LCRH_SPS               (1 << 7)

/*UART_CR*/
#define UART_CR_UARTEN              (1 << 0)
#define UART_CR_SIREN               (1 << 1)
#define UART_CR_SIRLP               (1 << 2)
#define UART_CR_LPE                 (1 << 7)
#define UART_CR_TXE                 (1 << 8)
#define UART_CR_RXE                 (1 << 9)
#define UART_CR_DTS                 (1 << 10)
#define UART_CR_RTS                 (1 << 11)
#define UART_CR_OUT1                (1 << 12)
#define UART_CR_OUT2                (1 << 13)
#define UART_CR_RTSEN               (1 << 14)
#define UART_CR_CTSEN               (1 << 15)


/*UART_FLS*/
#define UART_FLS_TXIFLSEL_1         (0 << 0)
#define UART_FLS_TXIFLSEL_2         (1 << 0)
#define UART_FLS_TXIFLSEL_4         (2 << 0)
#define UART_FLS_TXIFLSEL_6         (3 << 0)
#define UART_FLS_TXIFLSEL_7         (4 << 0)
#define UART_FLS_RXIFLSEL_1         (0 << 3)
#define UART_FLS_RXIFLSEL_2         (1 << 3)
#define UART_FLS_RXIFLSEL_4         (2 << 3)
#define UART_FLS_RXIFLSEL_6         (3 << 3)
#define UART_FLS_RXIFLSEL_7         (4 << 3)

#define UART_INT_OE                 (1 << 10)
#define UART_INT_BE                 (1 << 9)
#define UART_INT_PE                 (1 << 8)
#define UART_INT_FE                 (1 << 7)
#define UART_INT_RT                 (1 << 6)
#define UART_INT_TX                 (1 << 5)
#define UART_INT_RX                 (1 << 4)
#define UART_INT_DSR                (1 << 3)
#define UART_INT_DCD                (1 << 2)
#define UART_INT_CTS                (1 << 1)
#define UART_INT_RI                 (1 << 0)

#endif //__SILAN_UART_REGS_H__

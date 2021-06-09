/*
 * silan_uart.h
 *
 * Modify Date: 2016-4-28 18:10
 */
 
#ifndef __SILAN_UART_H__
#define __SILAN_UART_H__

#include "silan_types.h"
#include "silan_uart_regs.h"

#define UART_1 				1
#define UART_2 				2
#define UART_3 				3
#define UART_4 				4

#define SILAN_UART1			1
#define SILAN_UART2			2
#define SILAN_UART3			3
#define SILAN_UART4			4

#define BAUDRATE_9600       9600

#define BAUDRATE_115200     115200
#define BAUDRATE_230400     230400

/*
 *silan_uart_putc
 *uart_addr = UART1_BASE or UART2_BASE or UART3_BASE or UART3_BASE
 *c         = char
 *example:
    silan_uart_putc(UART2_BASE, 'a');
 */
extern int silan_uart_putc(uint32_t uart_addr, char c);

/*
 *silan_uart_getc
 *uart_addr = UART1_BASE or UART2_BASE or UART3_BASE or UART3_BASE
 *example:
    silan_uart_getc(UART2_BASE);
 */
extern int silan_uart_getc(unsigned int uart_addr);

/*
 *silan_uart_init
 */
extern void silan_uart_config(uint32_t uart_addr, uint32_t baudrate, uint32_t en_fifo, uint32_t en_intr); 

void silan_uart_sys_init(uint32_t addr,uint32_t baudrate);

int serial_putc(const char c);
int serial_getc(char *c);
int silan_uart_getc_noblock(unsigned int uart_addr);

#endif //__SILAN_UART_H__


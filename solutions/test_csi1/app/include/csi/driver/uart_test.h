#ifndef __UART_TEST__
#define __UART_TEST__
 
#include <stdint.h>
#include <drv/usart.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
	uint8_t		uart_idx;
    uint32_t    baudrate;
    uint8_t     data_bits;
    uint8_t     parity;
    uint8_t     stop_bits;
	uint8_t		flowctrl;
    uint32_t    uart_transfer_len;
    uint8_t     uart_mode;
}test_uart_args_t;




extern int test_uart_syncSend(void *args);
extern int test_uart_syncReceive(void *args);
extern int test_uart_async_send_char(void *args);
extern int test_uart_async_receive_char(void *args);
extern int test_uart_interface(void *args);
extern int test_uart_getChar(void *args);
extern int test_uart_putChar(void *args);
extern int test_uart_txcount(void *args);
extern int test_uart_rxcount(void *args);
#endif


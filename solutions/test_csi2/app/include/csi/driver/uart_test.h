/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __UART_TEST__
#define __UART_TEST__

#include <stdint.h>
#include <drv/uart.h>
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
    uint32_t    trans_size;
} test_uart_args_t;


extern int test_uart_config(csi_uart_t *uart_hd, test_uart_args_t *uart_args);
extern int test_uart_interface(char *args);
extern int test_uart_syncSend(char *args);
extern int test_uart_syncReceive(char *args);
extern int test_uart_syncSendChar(char *args);
extern int test_uart_syncReceiveChar(char *args);
extern int test_uart_asyncSend(char *args);
extern int test_uart_asyncReceive(char *args);
extern int test_uart_asyncSendChar(char *args);
extern int test_uart_asyncReceiveChar(char *args);
extern int test_uart_putChar(char *args);
extern int test_uart_getChar(char *args);
extern int test_uart_dmaSend(char *args);
extern int test_uart_dmaReceive(char *args);
extern int test_uart_asyncSendSpeed(char *args);
extern int test_uart_asyncReceiveSpeed(char *args);
extern int test_uart_syncSendSpeed(char *args);
extern int test_uart_syncReceiveSpeed(char *args);
extern int test_uart_sync_sendreceive(char *args);
extern int test_uart_main(char *args);

#endif

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <uart_test.h>

int test_uart_config(csi_uart_t *uart_hd, test_uart_args_t *uart_args)
{
    csi_error_t ret_status;

    ret_status = csi_uart_baud(uart_hd, uart_args->baudrate);
    TEST_CASE_ASSERT(ret_status == 0, "UART %d config baudrate error", uart_args->uart_idx);

    ret_status = csi_uart_format(uart_hd, uart_args->data_bits, uart_args->parity, uart_args->stop_bits);
    TEST_CASE_ASSERT(ret_status == 0, "UART %d config format error", uart_args->uart_idx);

    ret_status = csi_uart_flowctrl(uart_hd, uart_args->flowctrl);
    TEST_CASE_ASSERT(ret_status == 0, "UART %d config flowctrl error", uart_args->uart_idx);

    return 0;
}

test_func_map_t uart_test_funcs_map[] = {
    {"UART_INTERFACE", test_uart_interface},
    {"UART_SYNC_SEND", test_uart_syncSend},
    {"UART_SYNC_RECEIVE", test_uart_syncReceive},
    {"UART_SYNC_SEND_CHAR", test_uart_syncSendChar},
    {"UART_SYNC_RECEIVE_CHAR", test_uart_syncReceiveChar},
    {"UART_ASYNC_SEND", test_uart_asyncSend},
    {"UART_ASYNC_RECEIVE", test_uart_asyncReceive},
    {"UART_ASYNC_SEND_CHAR", test_uart_asyncSendChar},
    {"UART_ASYNC_RECEIVE_CHAR", test_uart_asyncReceiveChar},
    {"UART_PUT_CHAR", test_uart_putChar},
    {"UART_GET_CHAR", test_uart_getChar},
    {"UART_DMA_SEND", test_uart_dmaSend},
    {"UART_DMA_RECEIVE", test_uart_dmaReceive},
    {"UART_SYNC_SENDRECEIVE", test_uart_sync_sendreceive},
};


int test_uart_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(uart_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, uart_test_funcs_map[i].test_func_name)) {
            (*(uart_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("UART module don't support this command.");
    return -1;
}



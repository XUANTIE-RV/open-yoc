/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <uart_test.h>
#include <stdlib.h>

int test_uart_interface(char *args)
{
    csi_error_t ret_status;
    uint32_t ret_num;
    uint32_t ret_char_num;
    uint8_t ret_char;

    ret_status = csi_uart_init(NULL, 1);
    TEST_CASE_ASSERT(ret_status == CSI_ERROR, "csi_uart_init interface test error");

    csi_uart_uninit(NULL);

    csi_uart_t uart_hd;
    ret_status = csi_uart_init(&uart_hd, 1);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "uart init error");

    ret_status = csi_uart_baud(NULL, 115200);
    TEST_CASE_ASSERT(ret_status == CSI_ERROR, "csi_uart_baud interface test error");

    ret_status = csi_uart_format(NULL, 0, 0, 0);
    TEST_CASE_ASSERT(ret_status == CSI_ERROR, "csi_uart_format interface test error");

    ret_status = csi_uart_flowctrl(NULL, 0);
    TEST_CASE_ASSERT(ret_status == CSI_ERROR, "csi_uart_flowctrl interface test error");

    char *str = "abc";
    ret_num = csi_uart_send(NULL, str, 3, 0);
    TEST_CASE_ASSERT(ret_num == 0, "csi_uart_send interface test error");

    ret_status = csi_uart_send_async(NULL, str, 3);
    TEST_CASE_ASSERT(ret_status == CSI_ERROR, "csi_uart_send_async interface test error");

    char rec_data1[10];
    ret_char_num = csi_uart_receive(NULL, rec_data1, 10, 0);
    TEST_CASE_ASSERT(ret_char_num == 0, "csi_uart_receive interface test error");

    ret_status = csi_uart_receive_async(NULL, rec_data1, 10);
    TEST_CASE_ASSERT(ret_status == 0, "csi_uart_receive_async interface test error");

    ret_char = csi_uart_getc(NULL);
    TEST_CASE_ASSERT(ret_char == 0, "csi_uart_getc interface test error");

    csi_uart_putc(NULL, ret_char);

    ret_status = csi_uart_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_status == 0, "csi_uart_attach_callback interface test error");

    csi_uart_detach_callback(NULL);

    ret_status = csi_uart_link_dma(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_status == 0, "csi_uart_link_dma interface test error");

    csi_state_t state;

    ret_status = csi_uart_get_state(NULL, &state);
    TEST_CASE_ASSERT(ret_status == 0, "csi_uart_get_state interface test error");

    return 0;
}

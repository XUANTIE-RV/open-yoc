/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <uart_test.h>


static uint8_t data = 'a';

int test_uart_putChar(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_status;
    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.uart_idx = (uint8_t)_dev_idx;
    td.baudrate = (uint32_t)get_data[0];
    td.data_bits = (uint8_t)get_data[1];
    td.parity = (uint8_t)get_data[2];
    td.stop_bits = (uint8_t)get_data[3];
    td.flowctrl = (uint8_t)get_data[4];

    ret_status = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    TEST_CASE_READY();

    csi_uart_putc(&hd, data);

    tst_mdelay(100);
    csi_uart_uninit(&hd);
    return 0;
}


int test_uart_getChar(char *args)
{
    uint8_t receive_data;

    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.uart_idx = (uint8_t)_dev_idx;
    td.baudrate = (uint32_t)get_data[0];
    td.data_bits = (uint8_t)get_data[1];
    td.parity = (uint8_t)get_data[2];
    td.stop_bits = (uint8_t)get_data[3];
    td.flowctrl = (uint8_t)get_data[4];

    ret_status = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    TEST_CASE_READY();

    receive_data = csi_uart_getc(&hd);

    if (receive_data != data) {
        TEST_CASE_ASSERT(1 == 0, "uart %d:char sent is not equal to the received,received_data:%x, data:%x", td.uart_idx, receive_data, data);
    }

    csi_uart_uninit(&hd);
    return 0;
}

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <uart_test.h>

static void uart_callback(csi_uart_t *uart, csi_uart_event_t event, void *arg)
{
    switch (event) {
        case 0:
            *((uint8_t *)arg) = 0;
            break;

        case 1:
            *((uint8_t *)arg) = 0;
            break;

        default:
            break;
    }
}


int test_uart_asyncSend(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;

    csi_error_t ret_state;
    uint32_t get_data[6];

    ret = args_parsing(args, get_data, 6);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.uart_idx = (uint8_t)_dev_idx;
    td.baudrate = (uint32_t)get_data[0];
    td.data_bits = (uint8_t)get_data[1];
    td.parity = (uint8_t)get_data[2];
    td.stop_bits = (uint8_t)get_data[3];
    td.flowctrl = (uint8_t)get_data[4];
    td.trans_size = (uint32_t)get_data[5];

    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_uart_uninit(&hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    volatile uint8_t lock = 0;

    ret_state = csi_uart_attach_callback(&hd, uart_callback, (void *)&lock);

    if (ret_state != 0) {
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d attach callback error", td.uart_idx);
    }

    TEST_CASE_READY();
    transfer_data(send_data, td.trans_size);

    lock = 1;
    ret_state = csi_uart_send_async(&hd, send_data, td.trans_size);

    if (ret_state != 0) {
        csi_uart_detach_callback(&hd);
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d async send error", td.uart_idx);
    }

    while (lock) ;

    csi_uart_detach_callback(&hd);

    tst_mdelay(100);
    free(send_data);
    csi_uart_uninit(&hd);

    return 0;
}


int test_uart_asyncReceive(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_state;
    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 6);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.uart_idx = (uint8_t)_dev_idx;
    td.baudrate = (uint32_t)get_data[0];
    td.data_bits = (uint8_t)get_data[1];
    td.parity = (uint8_t)get_data[2];
    td.stop_bits = (uint8_t)get_data[3];
    td.flowctrl = (uint8_t)get_data[4];
    td.trans_size = (uint32_t)get_data[5];

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    volatile uint8_t lock = 0;

    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_uart_uninit(&hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    char *receive_data;
    receive_data = malloc(td.trans_size);

    if (receive_data == NULL) {
        csi_uart_uninit(&hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret_state = csi_uart_attach_callback(&hd, uart_callback, (void *)&lock);

    if (ret_state != 0) {
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "uart %d attach callback error", td.uart_idx);
    }


    lock = 1;
    TEST_CASE_READY();

    ret_state = csi_uart_receive_async(&hd, receive_data, td.trans_size);

    if (ret_state != 0) {
        csi_uart_detach_callback(&hd);
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT(ret_state == CSI_OK, "uart %d async receive error", td.uart_idx);
    }

    while (lock) ;

    if (memcmp(send_data, receive_data, td.trans_size)) {
        TEST_CASE_ASSERT(1 == 0, "uart:%d data async sent is not equal to the received, soure data:%x recevied data:%x", td.uart_idx, send_data, receive_data);
    }

    free(receive_data);
    free(send_data);
    csi_uart_detach_callback(&hd);

    csi_uart_uninit(&hd);

    return 0;
}


int test_uart_asyncSendChar(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_state;
    volatile uint8_t lock = 0;
    char send_data[1];
    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.uart_idx = (uint8_t)_dev_idx;
    td.baudrate = (uint32_t)get_data[0];
    td.data_bits = (uint8_t)get_data[1];
    td.parity = (uint8_t)get_data[2];
    td.stop_bits = (uint8_t)get_data[3];
    td.flowctrl = (uint8_t)get_data[4];

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    ret_state = csi_uart_attach_callback(&hd, uart_callback, (void *)&lock);

    if (ret_state != 0) {
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "uart %d attach callback error", td.uart_idx);
    }

    transfer_data(send_data, 1);
    TEST_CASE_READY();

    lock = 1;
    ret_state = csi_uart_send_async(&hd, send_data, 1);
    TEST_CASE_ASSERT(ret_state == CSI_OK, "uart %d async send error", td.uart_idx);


    while (lock) ;

    csi_uart_detach_callback(&hd);

    tst_mdelay(100);
    csi_uart_uninit(&hd);
    return 0;
}


int test_uart_asyncReceiveChar(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_state;

    char receive_data;
    char send_data[1];
    volatile uint8_t lock = 0;

    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.uart_idx = (uint8_t)_dev_idx;
    td.baudrate = (uint32_t)get_data[0];
    td.data_bits = (uint8_t)get_data[1];
    td.parity = (uint8_t)get_data[2];
    td.stop_bits = (uint8_t)get_data[3];
    td.flowctrl = (uint8_t)get_data[4];

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    ret_state = csi_uart_attach_callback(&hd, uart_callback, (void *)&lock);

    if (ret_state != 0) {
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "uart %d attach callback error", td.uart_idx);
    }

    lock = 1;
    TEST_CASE_READY();

    ret_state = csi_uart_receive_async(&hd, &receive_data, 1);

    if (ret_state != 0) {
        csi_uart_detach_callback(&hd);
        csi_uart_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "uart %d async receive error", td.uart_idx);
    }

    while (lock) ;

    csi_uart_detach_callback(&hd);

    transfer_data(send_data, 1);
    TEST_CASE_ASSERT(receive_data != *send_data, "uart %d:data async sent is not equal to the received", td.uart_idx);

    csi_uart_uninit(&hd);
    return 0;
}

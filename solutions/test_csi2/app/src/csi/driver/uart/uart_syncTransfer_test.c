/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <uart_test.h>

#define TRANSFER_DATA_SIZE 1024

int test_uart_syncSend(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    uint32_t ret_num;
    csi_error_t ret_state;
    csi_state_t uart_state;

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

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == CSI_OK, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);
    TEST_CASE_ASSERT(ret == 0, "uart config error");

    ret_state = csi_uart_get_state(&hd, &uart_state);
    TEST_CASE_ASSERT(ret_state == CSI_OK, "uart %d get state error", td.uart_idx);

    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_uart_uninit(&hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    TEST_CASE_READY();
    ret_num = csi_uart_send(&hd, send_data, td.trans_size, 1000);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "uart %d sync send error", td.uart_idx);

    ret_state = csi_uart_get_state(&hd, &uart_state);
    TEST_CASE_ASSERT(ret_state == CSI_OK, "uart %d get state error", td.uart_idx);

    tst_mdelay(200);
    free(send_data);
    csi_uart_uninit(&hd);
    return 0;
}



int test_uart_syncReceive(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;

    uint32_t ret_num;
    csi_error_t ret_state;
    csi_state_t uart_state;
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

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == 0, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);

    if (ret != 0) {
        return -1;
    }

    char *receive_data = NULL;
    receive_data = (char *)malloc(td.trans_size + 1);

    if (receive_data == NULL) {
        csi_uart_uninit(&hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    char *except_data = NULL;
    except_data = (char *)malloc(td.trans_size);

    if (except_data == NULL) {
        csi_uart_uninit(&hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret_state = csi_uart_get_state(&hd, &uart_state);
    TEST_CASE_ASSERT(ret_state == 0, "uart %d get state error", td.uart_idx);

    TEST_CASE_READY();
    ret_num = csi_uart_receive(&hd, receive_data, td.trans_size, 10000);
    *(receive_data + td.trans_size) = '\0';
    TEST_CASE_ASSERT(ret_num == td.trans_size, "the size of data received is not equal to the expected.");

    transfer_data(except_data, td.trans_size);

    ret = memcmp(except_data, receive_data, td.trans_size);
    TEST_CASE_ASSERT(ret == 0, "data uart %d sync sent is not equal to the received", td.uart_idx);

    ret_state = csi_uart_get_state(&hd, &uart_state);
    TEST_CASE_ASSERT(ret_state == 0, "uart %d get state error", td.uart_idx);

    free(receive_data);
    free(except_data);
    csi_uart_uninit(&hd);
    return 0;
}



int test_uart_syncSendChar(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t ret_num;
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

    ret_status = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);
    TEST_CASE_ASSERT(ret == 0, "uart config error");

    transfer_data(send_data, 1);

    TEST_CASE_READY();
    ret_num = csi_uart_send(&hd, send_data, 1, 1000);
    TEST_CASE_ASSERT(ret_num == 1, "uart %d sync send error", td.uart_idx);

    tst_mdelay(200);
    csi_uart_uninit(&hd);
    return 0;
}



int test_uart_syncReceiveChar(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    csi_error_t ret_status;
    uint32_t ret_num;
    char receive_data;
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

    ret_status = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);
    TEST_CASE_ASSERT(ret == 0, "uart config error");

    TEST_CASE_READY();
    ret_num = csi_uart_receive(&hd, &receive_data, 1, 10000);
    TEST_CASE_ASSERT_QUIT(ret_num == 1, "uart %d  sync receive error", td.uart_idx);

    transfer_data(send_data, 1);
    TEST_CASE_ASSERT(receive_data == send_data[0], "data uart %d sync sent is not equal to the received", td.uart_idx);

    csi_uart_uninit(&hd);
    return 0;
}


int test_uart_sync_sendreceive(char *args)
{
    csi_uart_t hd;
    test_uart_args_t td;
    int ret;
    uint32_t ret_num;
    csi_error_t ret_state;
    csi_state_t uart_state;
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

    transfer_data(send_data, td.trans_size);

    ret_state = csi_uart_init(&hd, td.uart_idx);
    TEST_CASE_ASSERT_QUIT(ret_state == 0, "uart %d init error", td.uart_idx);

    ret = test_uart_config(&hd, &td);
    TEST_CASE_ASSERT(ret == 0, "uart config error");

    ret_state = csi_uart_get_state(&hd, &uart_state);
    TEST_CASE_ASSERT(ret_state == 0, "uart %d get state error", td.uart_idx);

    TEST_CASE_READY();

    ret_num = csi_uart_send(&hd, send_data, td.trans_size, 1000);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "uart %d sync send error", td.uart_idx);

    ret_state = csi_uart_get_state(&hd, &uart_state);
    TEST_CASE_ASSERT(ret_state == 0, "uart %d get state error", td.uart_idx);

    char *receive_buffer = (char *)malloc(td.trans_size + 1);
    *(receive_buffer + td.trans_size) = '\0';

    ret_num = csi_uart_receive(&hd, receive_buffer, td.trans_size, 10000);
    free(receive_buffer);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "data received is not equal to the expected.");

    free(send_data);
    tst_mdelay(200);
    csi_uart_uninit(&hd);
    return 0;
}

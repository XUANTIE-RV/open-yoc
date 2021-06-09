/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spi_test.h"


int test_spi_sync_send(char *args)
{
    int ret;
    csi_state_t state;
    csi_error_t spi_ret;
    csi_spi_t spi_handler;
    uint32_t send_num = 0;
    uint32_t get_data[7];
    test_spi_args_t td;

    ret = args_parsing(args, get_data, 7);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.idx = get_data[0];
    td.spi_mode = get_data[1];
    td.frame_len = get_data[2];
    td.cp_format = get_data[3];
    td.baud = get_data[4];
    td.link_dma = get_data[5];
    td.trans_size = get_data[6];

    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);

    ret = spi_test_config(&spi_handler, &td, NULL, NULL, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    TEST_CASE_READY();
    send_num = csi_spi_send(&spi_handler, send_data, td.trans_size, 30000);
    TEST_CASE_ASSERT_QUIT(send_num > 0, "spi send data error, send_num should be greater than 0,but returned %d", send_num);

    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi get state error, should return CSI_OK, but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.writeable == 1, "spi get wrong state,should return spi_state.writeable==1,but retured %u", state.writeable);

    free(send_data);
    csi_spi_uninit(&spi_handler);
    return 0;
}

int test_spi_sync_receive(char *args)
{
    int ret;
    int received_num = 0 ;
    csi_error_t spi_ret;
    csi_state_t spi_state;
    csi_spi_t spi_handler;
    test_spi_args_t td;
    uint32_t get_data[7];

    ret = args_parsing(args, get_data, 7);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.idx = get_data[0];
    td.spi_mode = get_data[1];
    td.frame_len = get_data[2];
    td.cp_format = get_data[3];
    td.baud = get_data[4];
    td.link_dma = get_data[5];
    td.trans_size = get_data[6];

    char *receive_data = NULL;
    receive_data = (char *)malloc(td.trans_size);
    
    if (receive_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }
    char *except_data = NULL;
    except_data = (char *)malloc(td.trans_size);
    
    if (except_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(except_data, td.trans_size);

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);

    ret = spi_test_config(&spi_handler, &td, NULL, NULL, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi_test_config error.");

    TEST_CASE_READY();
    received_num = csi_spi_receive(&spi_handler, receive_data, td.trans_size, 10000);
    TEST_CASE_ASSERT(received_num == td.trans_size,"SPI receive error");

    ret = memcmp(receive_data,except_data,td.trans_size);
    TEST_CASE_ASSERT(ret == 0,"data SPI sync sent is not equal to the received");
 
    spi_ret = csi_spi_get_state(&spi_handler, &spi_state);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi get state error, should return CSI_OK, but returned: %d", spi_ret);
    TEST_CASE_ASSERT(spi_state.readable == 1, "spi get wrong state,should return spi_state.readable==1, but returned: %u", spi_state.readable);

    free(receive_data);
    free(except_data);
    csi_spi_uninit(&spi_handler);
    return 0;
}



int test_spi_sync_send_receive(char *args)
{
    int ret;
    csi_state_t state;
    csi_error_t spi_ret;
    csi_spi_t spi_handler;
    char *recv_buf;
    uint32_t get_data[7];
    test_spi_args_t td;

    ret = args_parsing(args, get_data, 7);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.idx = get_data[0];
    td.spi_mode = get_data[1];
    td.frame_len = get_data[2];
    td.cp_format = get_data[3];
    td.baud = get_data[4];
    td.link_dma = get_data[5];
    td.trans_size = get_data[6];

    char *spi_transfer_data = NULL;
    spi_transfer_data = (char *)malloc(td.trans_size);

    if (spi_transfer_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }    

    generate_rand_array(spi_transfer_data, 255, td.trans_size);

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);

    ret = spi_test_config(&spi_handler, &td, NULL, NULL, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    recv_buf = (char *)malloc(td.trans_size + 1);
    TEST_CASE_ASSERT_QUIT(recv_buf != NULL, "malloc error.");
    *(recv_buf + td.trans_size) = '\0';

    TEST_CASE_READY();
    uint32_t recv_num;
    recv_num = csi_spi_send_receive(&spi_handler, spi_transfer_data, recv_buf, td.trans_size, 10000);
    TEST_CASE_ASSERT_QUIT(recv_num == td.trans_size, "spi send and receive data error");
    TEST_CASE_ASSERT(memcmp(spi_transfer_data, recv_buf, td.trans_size) == 0, "data received is not equal to the expected");
    free(recv_buf);

    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK, but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.writeable == 1, "spi get wrong state,should return spi_state.writeable==1, but returned: %u", state.writeable);

    free(spi_transfer_data);
    csi_spi_uninit(&spi_handler);
    return 0;
}

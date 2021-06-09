/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spi_test.h"


static void test_spi_callback(csi_spi_t *spi_handler, csi_spi_event_t event, void *arg)
{
    switch (event) {
        case SPI_EVENT_SEND_COMPLETE:
            *((uint8_t *)arg) = 0;
            break;

        case SPI_EVENT_RECEIVE_COMPLETE:
            *((uint8_t *)arg) = 0;
            break;

        case SPI_EVENT_SEND_RECEIVE_COMPLETE:
            *((uint8_t *)arg) = 0;
            break;

        default:
            break;
    }
}

int test_spi_async_send(char *args)
{
    int ret;
    csi_state_t state;
    volatile uint8_t spi_flag = 0;
    csi_error_t spi_ret;
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

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);

    spi_ret = csi_spi_attach_callback(&spi_handler, test_spi_callback, (void *)&spi_flag);
    ret = spi_test_config(&spi_handler, &td, NULL, NULL, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    spi_flag = 1;
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi attach callback error");
    TEST_CASE_READY();
    spi_ret = csi_spi_send_async(&spi_handler, send_data, td.trans_size);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi send async error");

    while (spi_flag);

    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK,but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.writeable == 1, "spi get wrong state,should return spi state.writeable=1, but returned %u", state.writeable);

    if (td.link_dma == 1) {
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "unlink DMA error");
    }

    csi_spi_detach_callback(&spi_handler);
    csi_spi_uninit(&spi_handler);
    return 0;
}

int test_spi_async_receive(char *args)
{
    int ret;
    csi_error_t spi_ret;
    csi_state_t state;
    volatile uint8_t spi_flag = 0;
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
    spi_ret = csi_spi_attach_callback(&spi_handler, test_spi_callback, (void *)&spi_flag);
    ret = spi_test_config(&spi_handler, &td, NULL, NULL, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    spi_flag = 1;
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi attach callback error");

    TEST_CASE_READY();
    spi_ret = csi_spi_receive_async(&spi_handler, receive_data, td.trans_size);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi receive async error");

    while (spi_flag);

    ret = memcmp(receive_data,except_data,td.trans_size);
    TEST_CASE_ASSERT(1 == 0,"data SPI %d async sent is not equal to the received");
 
    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK,but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.readable == 1, "spi get wrong state,should return state.readable=1, but returned %u", state.readable);

    if (td.link_dma == 2) {
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "unlink DMA error");
    }

    csi_spi_detach_callback(&spi_handler);
    free(except_data);
    free(receive_data);
    csi_spi_uninit(&spi_handler);
    return 0;
}

int test_spi_async_send_receive(char *args)
{
    int ret;
    csi_error_t spi_ret;
    csi_state_t state;
    volatile uint8_t spi_flag = 0;
    char *recv_buffer;
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

    char *spi_transfer_data = NULL;
    spi_transfer_data = (char *)malloc(td.trans_size);

    if (spi_transfer_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }    

    generate_rand_array(spi_transfer_data, 255, td.trans_size);

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);
    spi_ret = csi_spi_attach_callback(&spi_handler, test_spi_callback, (void *)&spi_flag);
    ret = spi_test_config(&spi_handler, &td, NULL, NULL, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    spi_flag = 1;
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi attach callback error");

    recv_buffer = (char *)malloc(td.trans_size + 1);
    TEST_CASE_ASSERT_QUIT(recv_buffer != NULL, "malloc error");
    *(recv_buffer + td.trans_size) = '\0';

    TEST_CASE_READY();
    spi_ret = csi_spi_send_receive_async(&spi_handler, spi_transfer_data, recv_buffer, td.trans_size);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi receive async error");

    while (spi_flag);

    TEST_CASE_ASSERT_QUIT(memcmp(recv_buffer, spi_transfer_data, td.trans_size) == 0, "data async sent_and_received is not equal to the expected");
    free(recv_buffer);

    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK,but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.readable == 1, "spi get wrong state,should return spi_state.readable=1, but returned %u", state.readable);

    if (td.link_dma == 3) {
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "unlink DMA error");
    }

    csi_spi_detach_callback(&spi_handler);
    free(spi_transfer_data);
    csi_spi_uninit(&spi_handler);
    return 0;

}



int test_spi_dma_send(char *args)
{
    csi_dma_t spi_dma_handler;
    csi_dma_ch_t spi_dma_ch;
    int ret;
    csi_state_t state;
    volatile uint8_t spi_flag = 0;
    csi_error_t spi_ret;
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

    char *send_data =NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }    

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);

    spi_ret = csi_spi_attach_callback(&spi_handler, test_spi_callback, (void *)&spi_flag);
    ret = spi_test_config(&spi_handler, &td, &spi_dma_handler, &spi_dma_ch, NULL);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");


    spi_flag = 1;
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi attach callback error");
    TEST_CASE_READY();
    spi_ret = csi_spi_send_async(&spi_handler, send_data, td.trans_size);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi send async error");

    while (spi_flag);

    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK,but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.writeable == 1, "spi get wrong state,should return spi state.writeable=1, but returned %u", state.writeable);

    if (td.link_dma == 1) {
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "unlink DMA error");
    }

    csi_spi_detach_callback(&spi_handler);
    free(send_data);
    csi_spi_uninit(&spi_handler);
    return 0;
}


int test_spi_dma_receive(char *args)
{
    csi_dma_t spi_dma_handler;
    csi_dma_ch_t spi_dma_ch_1;
    int ret;
    csi_error_t spi_ret;
    csi_state_t state;
    volatile uint8_t spi_flag = 0;
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
    receive_data = (char *)malloc(td.trans_size + 1);

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

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);
    spi_ret = csi_spi_attach_callback(&spi_handler, test_spi_callback, (void *)&spi_flag);
    ret = spi_test_config(&spi_handler, &td, &spi_dma_handler, NULL, &spi_dma_ch_1);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    spi_flag = 1;
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi attach callback error");

    TEST_CASE_READY();
    spi_ret = csi_spi_receive_async(&spi_handler, receive_data, td.trans_size);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi receive async error");

    while (spi_flag);

    ret = memcmp(except_data, receive_data, td.trans_size);
    TEST_CASE_ASSERT(ret == 0, "data spi sent is not equal to the received");


    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK,but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.readable == 1, "spi get wrong state,should return state.readable=1, but returned %u", state.readable);

    if (td.link_dma == 2) {
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "unlink DMA error");
    }

    csi_spi_detach_callback(&spi_handler);

    csi_spi_uninit(&spi_handler);
    return 0;

}


int test_spi_dma_send_receive(char *args)
{
    csi_dma_t spi_dma_handler;
    csi_dma_ch_t spi_dma_ch, spi_dma_ch_1;
    int ret;
    csi_error_t spi_ret;
    csi_state_t state;
    volatile uint8_t spi_flag = 0;
    char *recv_buffer;
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

    char *spi_transfer_data = NULL;
    spi_transfer_data = (char *)malloc(td.trans_size);

    if (spi_transfer_data == NULL) {
        csi_spi_uninit(&spi_handler);
        TEST_CASE_WARN_QUIT("malloc error");
    }    

    generate_rand_array(spi_transfer_data, 255, td.trans_size);

    spi_ret = csi_spi_init(&spi_handler, td.idx);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi %d init error", td.idx);
    spi_ret = csi_spi_attach_callback(&spi_handler, test_spi_callback, (void *)&spi_flag);
    ret = spi_test_config(&spi_handler, &td, &spi_dma_handler, &spi_dma_ch, &spi_dma_ch_1);
    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi spi mode:%d", td.spi_mode);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_ASSERT_QUIT(ret == 0, "spi config error");

    spi_flag = 1;
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi attach callback error");

    recv_buffer = (char *)malloc(td.trans_size + 1);
    TEST_CASE_ASSERT_QUIT(recv_buffer != NULL, "malloc error");
    *(recv_buffer + td.trans_size) = '\0';

    TEST_CASE_READY();
    spi_ret = csi_spi_send_receive_async(&spi_handler, spi_transfer_data, recv_buffer, td.trans_size);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "spi receive async error");

    while (spi_flag);

    TEST_CASE_ASSERT_QUIT(memcmp(recv_buffer, spi_transfer_data, td.trans_size) == 0, "data async sent_and_received is not equal to the expected");
    free(recv_buffer);

    spi_ret = csi_spi_get_state(&spi_handler, &state);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "spi get state error,should return CSI_OK,but returned: %d", spi_ret);
    TEST_CASE_ASSERT(state.readable == 1, "spi get wrong state,should return spi_state.readable=1, but returned %u", state.readable);

    if (td.link_dma == 3) {
        ret = csi_spi_link_dma(&spi_handler, NULL, NULL);
        TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "unlink DMA error");
    }

    csi_spi_detach_callback(&spi_handler);
    free(spi_transfer_data);

    csi_spi_uninit(&spi_handler);
    return 0;

}
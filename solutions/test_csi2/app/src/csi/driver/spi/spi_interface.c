/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "spi_test.h"

int test_spi_interface(char *args)
{

    char data_buffer[8];
    char *data_out = "12345678";
    int recv_num = 0;
    csi_error_t spi_ret;
    csi_state_t spi_state;
    csi_spi_t spi_handler;

    spi_ret = csi_spi_init(NULL, 0);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_init interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_init(&spi_handler, 0);
    TEST_CASE_ASSERT_QUIT(spi_ret == CSI_OK, "csi_spi_init interface test error,should return CSI_OK, but returned %d", spi_ret);

    spi_ret = csi_spi_attach_callback(&spi_handler, NULL, NULL);
    TEST_CASE_ASSERT(spi_ret == CSI_OK, "csi_spi_attach_callback interface test error,should return CSI_OK, but returned %d", spi_ret);

    spi_ret = csi_spi_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_attach_callback interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    csi_spi_detach_callback(NULL);

    spi_ret = csi_spi_mode(NULL, SPI_MASTER);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_mode interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_mode(&spi_handler, 2);
    TEST_CASE_ASSERT(spi_ret == CSI_UNSUPPORTED, "csi_spi_mode interface test error,should return CSI_UNSUPPORTED, but returned %d", spi_ret);

    spi_ret = csi_spi_cp_format(NULL, SPI_FORMAT_CPOL0_CPHA0);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_cp_format interface test error,should return CSI_UNSUPPORTED, but returned %d", spi_ret);

    spi_ret = csi_spi_cp_format(&spi_handler, 4);
    TEST_CASE_ASSERT(spi_ret == CSI_UNSUPPORTED, "csi_spi_cp_format interface test error,should return CSI_UNSUPPORTED, but returned %d", spi_ret);

    spi_ret = csi_spi_frame_len(NULL, SPI_FRAME_LEN_16);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_frame_len interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_frame_len(&spi_handler, 17);
    TEST_CASE_ASSERT(spi_ret == CSI_UNSUPPORTED, "csi_spi_frame_len interface test error,should return CSI_UNSUPPORTED, but returned %d", spi_ret);

    spi_ret = csi_spi_baud(NULL, 0x1000000);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_baud interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_send(NULL, data_buffer, sizeof(data_buffer), 1);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_send interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_send_async(NULL, data_buffer, sizeof(data_buffer));
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_send_async interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    recv_num = csi_spi_receive(NULL, data_buffer, sizeof(data_buffer), 1);
    TEST_CASE_ASSERT(recv_num == CSI_ERROR, "csi_spi_receive interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_receive_async(NULL, data_buffer, sizeof(data_buffer));
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_receive_async interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_send_receive_async(NULL, data_out, data_buffer, sizeof(data_buffer));
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_send_receive_async interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_link_dma(NULL, NULL, NULL);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_send_receive_async interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_get_state(NULL, &spi_state);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_get_state interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    spi_ret = csi_spi_get_state(&spi_handler, NULL);
    TEST_CASE_ASSERT(spi_ret == CSI_ERROR, "csi_spi_get_state interface test error,should return CSI_ERROR, but returned %d", spi_ret);

    csi_spi_select_slave(NULL, 0);

    csi_spi_uninit(&spi_handler);
    return 0;

}
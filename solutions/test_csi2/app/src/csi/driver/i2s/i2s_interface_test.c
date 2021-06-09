/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "i2s_test.h"

int test_i2s_interface(char *args)
{
    csi_error_t st;
    int ret = 0 ;
    csi_i2s_t i2s;
    csi_i2s_format_t fmt;

    st = csi_i2s_init(NULL, 0);
    TEST_CASE_ASSERT_QUIT(st == CSI_ERROR, "I2S init should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_init(&i2s, 0);
    TEST_CASE_ASSERT_QUIT(st == CSI_OK, "I2S init error");

    csi_i2s_enable(NULL, true);

    st = csi_i2s_format(NULL, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "I2S format should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_format(&i2s, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "I2S format should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_format(&i2s, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "I2S format should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_format(NULL, &fmt);
    TEST_CASE_ASSERT(st == CSI_ERROR, "I2S format should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_rx_select_sound_channel(NULL, I2S_LEFT_RIGHT_CHANNEL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "rx select channel should CSI_ERROR, but returned %d", st);

    st = csi_i2s_tx_select_sound_channel(NULL, I2S_LEFT_RIGHT_CHANNEL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "tx select channel should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_rx_link_dma(&i2s, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "rx_link_dma should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_rx_link_dma(NULL, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "rx_link_dma should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_tx_link_dma(&i2s, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "rx_link_dma should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_tx_link_dma(NULL, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "rx_link_dma should return CSI_ERROR, but returned %d", st);

    csi_i2s_rx_set_buffer(NULL, NULL);
    csi_i2s_tx_set_buffer(NULL, NULL);
    TEST_CASE_TIPS("rx, tx set buffer tested");

    ret = csi_i2s_tx_set_period(&i2s, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_i2s_tx_set_period should return -1, but returned %d", ret);

    ret = csi_i2s_rx_buffer_avail(NULL);
    TEST_CASE_ASSERT(ret == 0, "csi_i2s_rx_buf_avail should return 0, but returned %d", ret);

    ret = csi_i2s_tx_buffer_avail(NULL);
    TEST_CASE_ASSERT(ret == 0, "csi_i2s_tx_buf_avail should return 0, but returned %d", ret);

    st = csi_i2s_tx_buffer_reset(&i2s);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_tx_buf_reset should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_rx_buffer_reset(&i2s);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_rx_buf_reset should return CSI_ERROR, but returned %d", st);

    ret =  csi_i2s_send(NULL, NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_i2s_send should return -1, but returned %d", ret);

    ret = csi_i2s_receive(NULL,  NULL, 0);
    TEST_CASE_ASSERT(ret == -1, "csi_i2s_receive should return -1, but returned %d", ret);

    ret =  csi_i2s_send_async(NULL, NULL, 0);
    TEST_CASE_ASSERT(ret == 0, "csi_i2s_send_async should return 0, but returned %d", ret);

    ret = csi_i2s_receive_async(NULL,  NULL, 0);
    TEST_CASE_ASSERT(ret == 0, "csi_i2s_receive_async should return 0, but returned %d", ret);

    st = csi_i2s_send_pause(NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_send_pause should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_send_resume(NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_send_resume should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_send_start(NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_send_start should return CSI_ERROR, but returned %d", st);

    st = csi_i2s_receive_start(NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_receive_start should return CSI_ERROR, but returned %d", st);

    csi_i2s_send_stop(NULL);
    csi_i2s_receive_stop(NULL);

    st = csi_i2s_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_attach_callback should return CSI_ERROR, but returned %d", st);

    csi_i2s_detach_callback(NULL);
    st = csi_i2s_get_state(NULL, NULL);
    TEST_CASE_ASSERT(st == CSI_ERROR, "csi_i2s_get_state should return CSI_ERROR, but returned %d", st);
    return 0;
}
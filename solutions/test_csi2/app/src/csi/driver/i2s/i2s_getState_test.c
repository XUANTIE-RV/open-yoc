/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "i2s_test.h"

#define I2S_STATUS_SEND_LEN     (1024)
static uint8_t data_buf[I2S_STATUS_SEND_LEN];
static uint8_t data_out[I2S_STATUS_SEND_LEN];
static volatile uint8_t i2s_temp;
static void i2s_callback_async(csi_i2s_t *i2s, csi_i2s_event_t event, void *arg)
{
    uint8_t tmp;
    tmp = *((uint8_t *)arg);

    switch (event) {
        case I2S_EVENT_SEND_COMPLETE:
            tmp--;
            *((uint8_t *)arg) = tmp;
            break;

        case I2S_EVENT_RECEIVE_COMPLETE:
            tmp--;
            *((uint8_t *)arg) = tmp;
            break;

        default:
            break;
    }
}


int test_i2s_sync_status(char *args)
{
    csi_error_t status, ret;
    csi_i2s_t i2s;
    csi_i2s_format_t fmt;
    ringbuffer_t i2s_buf;
    csi_dma_ch_t dma_ch;
    uint32_t get_data[3];
    uint8_t iis_idx;
    uint32_t avail_len1, avail_len2, avail_len;
    csi_state_t state;

    ret = args_parsing(args, get_data, 1);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");
    iis_idx = (uint32_t)get_data[0];

    i2s_buf.buffer = data_buf;
    i2s_buf.size = I2S_STATUS_SEND_LEN;
    status =  csi_i2s_init(&i2s, iis_idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s init error");

    status = csi_i2s_attach_callback(&i2s, i2s_callback_async, (void *)&i2s_temp);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s attach callback error");

    fmt.rate = 8000;
    fmt.width = 16;
    fmt.protocol = 0;
    fmt.polarity = 0;
    fmt.mode = I2S_MODE_MASTER;
    fmt.sclk_nfs = I2S_SCLK_16FS;
    fmt.mclk_nfs = I2S_MCLK_256FS;

    status = csi_i2s_format(&i2s, &fmt);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "config i2s format error.");

    status = csi_i2s_tx_link_dma(&i2s, &dma_ch);
    TEST_CASE_ASSERT(status == CSI_OK, "link tx DMA error");

    csi_i2s_tx_set_buffer(&i2s, &i2s_buf);
    csi_i2s_tx_buffer_reset(&i2s);
    status = csi_i2s_tx_set_period(&i2s, I2S_STATUS_SEND_LEN / 8);
    TEST_CASE_ASSERT(status == CSI_OK, "set i2s tx period error");
    i2s_temp = i2s_buf.size / i2s.tx_period;
    dataset((char *)data_out, I2S_STATUS_SEND_LEN, I2S_TRANSFER_PAT);

    ret = csi_i2s_get_state(&i2s, &state);
    TEST_CASE_ASSERT(ret == CSI_OK, "i2s get status error");
    TEST_CASE_ASSERT(state.writeable == 0, "i2s get status error");

    status = csi_i2s_send_start(&i2s);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s start to send error");

    ret = csi_i2s_get_state(&i2s, &state);
    TEST_CASE_ASSERT(ret == CSI_OK, "i2s get status error");
    TEST_CASE_ASSERT(state.writeable == 1, "i2s get status error,should return state.writeable=1,but returned %d", state.writeable);

    avail_len = csi_i2s_tx_buffer_avail(&i2s);
    TEST_CASE_ASSERT(avail_len == I2S_STATUS_SEND_LEN, "i2s read available buffer size error");
    TEST_CASE_TIPS("i2s read available tx buffer size: %d", avail_len);

    TEST_CASE_READY();
    csi_i2s_send_async(&i2s, data_out, I2S_STATUS_SEND_LEN);

    avail_len = csi_i2s_tx_buffer_avail(&i2s);
    TEST_CASE_ASSERT(avail_len == 0, "i2s read available buffer size error");

    ret = csi_i2s_send_pause(&i2s);
    TEST_CASE_ASSERT(ret == CSI_OK, "i2s pause sending error");

    avail_len1 = csi_i2s_tx_buffer_avail(&i2s);
    tst_mdelay(5);
    avail_len2 = csi_i2s_tx_buffer_avail(&i2s);
    TEST_CASE_ASSERT(avail_len1 == avail_len2, "i2s read available buffer size error,avail_len1 is:%d,avail_len2 is:%d", avail_len1, avail_len2);

    ret = csi_i2s_send_resume(&i2s);
    TEST_CASE_ASSERT(ret == CSI_OK, "i2s resume sending error");

    avail_len1 = csi_i2s_tx_buffer_avail(&i2s);
    tst_mdelay(5);
    avail_len2 = csi_i2s_tx_buffer_avail(&i2s);
    TEST_CASE_ASSERT(avail_len1 == avail_len2, "i2s read available buffer size error,avail_len1 is:%d,avail_len2 is:%d", avail_len1, avail_len2);

    while (i2s_temp != 0);

    csi_i2s_send_stop(&i2s);
    csi_i2s_enable(&i2s, false);
    csi_i2s_tx_link_dma(&i2s, NULL);
    csi_i2s_detach_callback(&i2s);
    csi_i2s_uninit(&i2s);
    return 0;

}
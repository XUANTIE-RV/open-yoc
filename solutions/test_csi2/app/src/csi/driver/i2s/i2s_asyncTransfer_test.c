/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "i2s_test.h"
volatile uint8_t i2s_lock;
static uint8_t data_in[I2S_TRANSFER_SIZE];
static uint8_t expect[I2S_TRANSFER_SIZE];
static uint8_t data_out[I2S_TRANSFER_SIZE];

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

int test_i2s_async_send(char *args)
{
    csi_error_t status;
    uint32_t sent_num = 0;
    csi_i2s_t i2s;
    csi_i2s_format_t fmt;
    uint8_t data_buf[I2S_BUF_SIZE];
    ringbuffer_t i2s_buf;
    csi_dma_ch_t dma_ch;
    uint32_t get_data[8];
    test_i2s_args_t td;
    int ret;

    ret = args_parsing(args, get_data, 8);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.mode = get_data[0];
    td.rate = get_data[1];
    td.width = get_data[2];
    td.protocol = get_data[3];
    td.polarity = get_data[4];
    td.period = get_data[5];
    td.sclk_freq = get_data[6];
    td.mclk_freq = get_data[7];

    switch (td.mode) {
        case 0:
            td.mode = I2S_MODE_SLAVE;
            break;

        case 1:
            td.mode = I2S_MODE_MASTER;
            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "choose i2s mode error error");
            break;
    }

    i2s_buf.buffer = data_buf;
    i2s_buf.size = I2S_BUF_SIZE;
    status =  csi_i2s_init(&i2s, td.idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s init error");

    status = csi_i2s_attach_callback(&i2s, i2s_callback_async, (void *)&i2s_lock);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s attach callback error");

    fmt.mode = td.mode;
    fmt.protocol = td.protocol;
    fmt.polarity = td.polarity;
    fmt.rate = td.rate;
    fmt.width = td.width;
    fmt.sclk_nfs = td.sclk_freq;
    fmt.mclk_nfs = td.mclk_freq;

    status = csi_i2s_format(&i2s, &fmt);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "config i2s format error.");

    status = csi_i2s_tx_link_dma(&i2s, &dma_ch);
    TEST_CASE_ASSERT(status == CSI_OK, "link tx DMA error");

    csi_i2s_tx_set_buffer(&i2s, &i2s_buf);
    csi_i2s_tx_buffer_reset(&i2s);
    status = csi_i2s_tx_set_period(&i2s, 512);
    TEST_CASE_ASSERT(status == CSI_OK, "set i2s tx period error");
    i2s_lock = i2s_buf.size / i2s.tx_period;

    dataset((char *)data_out, I2S_TRANSFER_SIZE, I2S_TRANSFER_PAT);

    status = csi_i2s_send_start(&i2s);

    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s start to send error");

    TEST_CASE_READY();
    sent_num = csi_i2s_send_async(&i2s, data_out, I2S_TRANSFER_SIZE);

    while (i2s_lock > 0);

    TEST_CASE_ASSERT(sent_num == I2S_TRANSFER_SIZE, "i2s async send data error");

    csi_i2s_send_stop(&i2s);
    csi_i2s_enable(&i2s, false);

    csi_i2s_tx_link_dma(&i2s, NULL);
    csi_i2s_detach_callback(&i2s);
    csi_i2s_uninit(&i2s);
    return 0;

}

int test_i2s_async_receive(char *args)
{
    csi_error_t status;
    uint32_t recv_num = 0;
    csi_i2s_t i2s;
    csi_i2s_format_t fmt;
    uint8_t data_buf[I2S_BUF_SIZE];
    ringbuffer_t i2s_buf;
    csi_dma_ch_t dma_ch;
    uint32_t get_data[8];
    test_i2s_args_t td;
    int ret;

    ret = args_parsing(args, get_data, 8);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.mode = get_data[0];
    td.rate = get_data[1];
    td.width = get_data[2];
    td.protocol = get_data[3];
    td.polarity = get_data[4];
    td.period = get_data[5];
    td.sclk_freq = get_data[6];
    td.mclk_freq = get_data[7];

    switch (td.mode) {
        case 0:
            td.mode = I2S_MODE_SLAVE;
            break;

        case 1:
            td.mode = I2S_MODE_MASTER;
            break;

        default:
            TEST_CASE_ASSERT(1 == 0, "choose i2s mode error");
            break;
    }

    i2s_buf.buffer = data_buf;
    i2s_buf.size = I2S_BUF_SIZE;
    status =  csi_i2s_init(&i2s, td.idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s init error");

    status = csi_i2s_attach_callback(&i2s, i2s_callback_async, (void *)&i2s_lock);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s attach callback error");

    fmt.mode = td.mode;
    fmt.protocol = td.protocol;
    fmt.polarity = td.polarity;
    fmt.rate = td.rate;
    fmt.width = td.width;
    fmt.sclk_nfs = td.sclk_freq;
    fmt.mclk_nfs = td.mclk_freq;

    status = csi_i2s_format(&i2s, &fmt);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "config i2s format error.");

    status = csi_i2s_rx_link_dma(&i2s, &dma_ch);
    TEST_CASE_ASSERT(status == CSI_OK, "link rx DMA error");

    TEST_CASE_ASSERT(status == CSI_OK, "set I2S rx period error");
    csi_i2s_rx_set_buffer(&i2s, &i2s_buf);
    csi_i2s_rx_buffer_reset(&i2s);
    status = csi_i2s_rx_set_period(&i2s, 512);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s set period error");
    i2s_lock = i2s_buf.size / i2s.rx_period;

    status = csi_i2s_receive_start(&i2s);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s receive start error.");

    memset(data_in, 0, I2S_TRANSFER_SIZE);
    dataset((char *)expect, I2S_TRANSFER_SIZE, I2S_TRANSFER_PAT);
    TEST_CASE_READY();

    while (1) {
        recv_num += csi_i2s_receive_async(&i2s, (data_in + recv_num), (I2S_TRANSFER_SIZE - recv_num));

        if (recv_num == I2S_TRANSFER_SIZE) {
            break;
        }
    }


    int i = 0;
    uint32_t expect_cnt = 0;

    for (i = 0; i < I2S_TRANSFER_SIZE; i++) {

        if (data_in[i] == 0x5a) {
            expect_cnt++;
        }
    }

    if (td.protocol < 2) {
        TEST_CASE_TIPS("expect data count: %d", expect_cnt);
        TEST_CASE_ASSERT(expect_cnt > (I2S_TRANSFER_SIZE - 800), "receive data error");

        TEST_CASE_TIPS("received data: %2x", expect[1]);
        TEST_CASE_ASSERT(recv_num == I2S_TRANSFER_SIZE, "expected data size: %d,but received data size: %d", I2S_TRANSFER_SIZE, recv_num);
    } else {
        TEST_CASE_TIPS("expect data count: %d", expect_cnt);
        TEST_CASE_ASSERT(expect_cnt > ((I2S_TRANSFER_SIZE - 800) / 2), "receive data error");

        TEST_CASE_TIPS("received data: %2x", expect[1]);
        TEST_CASE_ASSERT(recv_num == I2S_TRANSFER_SIZE, "expected data size: %d,but received data size: %d", I2S_TRANSFER_SIZE, recv_num);
    }

    csi_i2s_receive_stop(&i2s);

    csi_i2s_enable(&i2s, false);
    csi_i2s_rx_link_dma(&i2s, NULL);
    csi_i2s_detach_callback(&i2s);
    csi_i2s_uninit(&i2s);
    return 0;
}


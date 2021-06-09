/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "i2s_test.h"

static uint8_t data_in[I2S_TRANSFER_SIZE];
static uint8_t expect[I2S_TRANSFER_SIZE];
static uint8_t data_out[I2S_TRANSFER_SIZE];

int test_i2s_sync_send(char *args)
{
    csi_error_t status;
    uint32_t sent_num = 0;
    csi_i2s_t i2s;
    csi_i2s_format_t fmt;
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

    status =  csi_i2s_init(&i2s, td.idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s init error");

    fmt.mode = td.mode;
    fmt.protocol = td.protocol;
    fmt.polarity = td.polarity;
    fmt.rate = td.rate;
    fmt.width = td.width;
    fmt.sclk_nfs = td.sclk_freq;
    fmt.mclk_nfs = td.mclk_freq;

    status = csi_i2s_format(&i2s, &fmt);
    csi_i2s_enable(&i2s, 1);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "config i2s format error.");
    dataset((char *)data_out, I2S_TRANSFER_SIZE, I2S_TRANSFER_PAT);

    TEST_CASE_READY();
    sent_num = csi_i2s_send(&i2s, data_out, I2S_TRANSFER_SIZE);
    TEST_CASE_ASSERT(sent_num == I2S_TRANSFER_SIZE, "i2s send data error,should return %d,but retured %d", I2S_TRANSFER_SIZE, sent_num);
    csi_i2s_enable(&i2s, 0);
    csi_i2s_uninit(&i2s);
    return 0;
}

int test_i2s_sync_receive(char *args)
{
    csi_error_t status;
    uint32_t recv_num = 0;
    csi_i2s_t i2s;
    csi_i2s_format_t fmt;
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

    status =  csi_i2s_init(&i2s, td.idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "i2s init error");

    fmt.mode = td.mode;
    fmt.protocol = td.protocol;
    fmt.polarity = td.polarity;
    fmt.rate = td.rate;
    fmt.width = td.width;
    fmt.sclk_nfs = td.sclk_freq;
    fmt.mclk_nfs = td.mclk_freq;

    status = csi_i2s_format(&i2s, &fmt);
    csi_i2s_enable(&i2s, 1);

    memset(data_in, 0, I2S_TRANSFER_SIZE);
    dataset((char *)expect, I2S_TRANSFER_SIZE, I2S_TRANSFER_PAT);

    uint32_t expect_cnt = 0;
    TEST_CASE_READY();
    recv_num = csi_i2s_receive(&i2s, data_in, I2S_TRANSFER_SIZE);
    TEST_CASE_ASSERT(recv_num == I2S_TRANSFER_SIZE, "i2s receive data error,should return %d,but returned %d", I2S_TRANSFER_SIZE, recv_num);

    for (uint32_t i = 0; i < I2S_TRANSFER_SIZE; i++) {
        if (data_in[i] == 0x5a) {
            expect_cnt++;
        }
    }

    if (td.protocol < 2) {
        TEST_CASE_ASSERT(expect_cnt > (I2S_TRANSFER_SIZE - 800), "i2s receive data error");
    } else {
        TEST_CASE_ASSERT(expect_cnt > ((I2S_TRANSFER_SIZE - 800) / 2), "i2s receive data error");
    }


    csi_i2s_enable(&i2s, 0);
    csi_i2s_uninit(&i2s);
    return 0;

}
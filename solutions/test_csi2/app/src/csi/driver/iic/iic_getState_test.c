/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <iic_test.h>


int test_iic_getStateMasterSend(char *args)
{
    test_iic_args_t td;
    int ret;
    csi_error_t ret_sta;
    csi_iic_t iic_hd;

    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)_dev_idx;
    td.addr_mode = (uint8_t)get_data[0];
    td.speed = (uint8_t)get_data[1];
    td.trans_size = get_data[2];
    td.slave_addr = get_data[3];


    TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_MASTER);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's speed is %d", td.speed);
    TEST_CASE_TIPS("send size is %d", td.trans_size);
    TEST_CASE_TIPS("select slave address is %d", td.slave_addr);


    ret_sta = csi_iic_init(&iic_hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "iic %d init error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);

    ret_sta = csi_iic_mode(&iic_hd, IIC_MODE_MASTER);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config mode error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }


    ret_sta = csi_iic_addr_mode(&iic_hd, td.addr_mode);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config address mode error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_speed(&iic_hd, td.speed);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config speed error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_attach_callback(&iic_hd, NULL, NULL);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d attach callback error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    TEST_CASE_READY();
    ret_sta = csi_iic_master_send_async(&iic_hd, td.slave_addr, send_data, td.trans_size);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d master async send error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);

    csi_state_t state;
    uint32_t check_num = 0;

    do {
        ret_sta = csi_iic_get_state(&iic_hd, &state);
        TEST_CASE_ASSERT(ret_sta == 0, "iic %d master get state error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
        check_num++;
    } while (!state.writeable);

    if (check_num <= 1) {
        TEST_CASE_WARN("the size of data transferred is too small, test error");
    }

    free(send_data);

    csi_iic_detach_callback(&iic_hd);

    csi_iic_uninit(&iic_hd);

    return 0;
}



int test_iic_getStateSlaveReceive(char *args)
{
    test_iic_args_t td;
    int ret;
    csi_error_t ret_sta;
    csi_iic_t iic_hd;

    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)_dev_idx;
    td.addr_mode = (uint8_t)get_data[0];
    td.speed = (uint8_t)get_data[1];
    td.own_addr = get_data[2];
    td.trans_size = get_data[3];

    TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_SLAVE);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's speed is %d", td.speed);
    TEST_CASE_TIPS("config iic's own address is %d", td.own_addr);
    TEST_CASE_TIPS("receive size is %d", td.trans_size);


    ret_sta = csi_iic_init(&iic_hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "iic %d init error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);

    ret_sta = csi_iic_mode(&iic_hd, IIC_MODE_SLAVE);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config mode error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_addr_mode(&iic_hd, td.addr_mode);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config address mode error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_speed(&iic_hd, td.speed);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config speed error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_own_addr(&iic_hd, td.own_addr);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config own address error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_attach_callback(&iic_hd, NULL, NULL);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d attach callback error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
    }


    char *receive_data = NULL, *expect_data = NULL;
    receive_data = (char *)malloc(td.trans_size);
    expect_data = (char *)malloc(td.trans_size);

    if ((receive_data == NULL) || (expect_data == NULL)) {
        free(receive_data);
        free(expect_data);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(expect_data, td.trans_size);

    TEST_CASE_READY();
    ret_sta = csi_iic_slave_receive_async(&iic_hd, receive_data, td.trans_size);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d slave async receive error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);

    csi_state_t state;
    uint32_t check_num = 0;

    do {
        ret_sta = csi_iic_get_state(&iic_hd, &state);
        TEST_CASE_ASSERT(ret_sta == 0, "iic %d master get state error, expected return value is 0, but actual return value is %d.", td.dev_idx, ret_sta);
        check_num++;
    } while (!state.readable);

    if (check_num <= 1) {
        TEST_CASE_WARN("the size of data is too small, test error");
    }

    ret = memcmp(expect_data, receive_data, td.trans_size);
    TEST_CASE_ASSERT(ret == 0, "data sent is not equal to the received");

    free(receive_data);
    free(expect_data);

    csi_iic_detach_callback(&iic_hd);

    csi_iic_uninit(&iic_hd);

    return 0;
}

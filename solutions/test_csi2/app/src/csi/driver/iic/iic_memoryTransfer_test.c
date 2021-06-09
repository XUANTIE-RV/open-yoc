/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <iic_test.h>

int test_iic_memoryTransfer(char *args)
{
    test_iic_args_t td;
    int ret;
    csi_error_t ret_sta;
    csi_iic_t iic_hd;
    int32_t ret_num;

    uint32_t get_data[7];

    ret = args_parsing(args, get_data, 7);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)_dev_idx;
    td.addr_mode = (uint8_t)get_data[0];
    td.speed = (uint8_t)get_data[1];
    td.trans_size = get_data[2];
    td.slave_addr = get_data[3];
    td.timeout = get_data[4];
    td.mem_addr = get_data[5];
    td.mem_addr_size = (uint8_t)get_data[6];

    TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_MASTER);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's sped is %d", td.speed);
    TEST_CASE_TIPS("send size is %d", td.trans_size);
    TEST_CASE_TIPS("select slave address is %d", td.slave_addr);
    TEST_CASE_TIPS("set send timeout is %d", td.timeout);
    TEST_CASE_TIPS("select EEPROM's address is %d", td.mem_addr);
    TEST_CASE_TIPS("set transfer size mode is %d with EEPROM", td.mem_addr_size);

    ret_sta = csi_iic_init(&iic_hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "iic %d init error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_iic_mode(&iic_hd, IIC_MODE_MASTER);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config mode error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }


    ret_sta = csi_iic_addr_mode(&iic_hd, td.addr_mode);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config address mode error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }

    ret_sta = csi_iic_speed(&iic_hd, td.speed);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config speed error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }

    char *send_data = NULL, *receive_data = NULL;
    send_data = (char *)malloc(td.trans_size);
    receive_data = (char *)malloc(td.trans_size);

    if ((send_data == NULL) || (receive_data == NULL)) {
        free(send_data);
        free(receive_data);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    ret_num = csi_iic_mem_send(&iic_hd, td.slave_addr, td.mem_addr, td.mem_addr_size, send_data, td.trans_size, td.timeout);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "iic %d send data from EEPROM error,should return %d,but returned %d", td.dev_idx, td.trans_size, ret_num);

    tst_mdelay(1000);

    ret_num = csi_iic_mem_receive(&iic_hd, td.slave_addr, td.mem_addr, td.mem_addr_size, receive_data, td.trans_size, td.timeout);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "iic %d receive data from EEPROM error,should return %d,but returned %d", td.dev_idx, td.trans_size, ret_num);

    ret = memcmp((const char *)send_data, (const char *)receive_data, td.trans_size);
    TEST_CASE_ASSERT(ret == 0, "data read by EEPROM is not equal to the data writen");


    free(send_data);
    free(receive_data);

    csi_iic_uninit(&iic_hd);

    return 0;

}

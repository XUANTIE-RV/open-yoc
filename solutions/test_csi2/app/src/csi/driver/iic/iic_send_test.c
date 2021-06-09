/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <iic_test.h>

static void iic_send_callback(csi_iic_t *iic, csi_iic_event_t event, void *arg)
{
    if (event == IIC_EVENT_SEND_COMPLETE) {
        *(uint8_t *)arg = 0;
    }
}


int test_iic_masterAsyncSend(char *args)
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
    td.trans_size = (uint32_t)get_data[2];
    td.slave_addr = (uint32_t)get_data[3];

    TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_MASTER);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's speed is %d", td.speed);
    TEST_CASE_TIPS("send size is %d", td.trans_size);
    TEST_CASE_TIPS("select slave address is %d", td.slave_addr);



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

    volatile uint8_t lock;
    ret_sta = csi_iic_attach_callback(&iic_hd, iic_send_callback, (void *)&lock);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d attach callback error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }


    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_detach_callback(&iic_hd);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    lock = 1;
    TEST_CASE_READY();
    ret_sta = csi_iic_master_send_async(&iic_hd, td.slave_addr, send_data, td.trans_size);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d master async send error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    while (lock) ;

    csi_iic_detach_callback(&iic_hd);

    free(send_data);

    csi_iic_uninit(&iic_hd);

    return 0;
}


int test_iic_slaveAsyncSend(char *args)
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
    TEST_CASE_TIPS("send size is %d", td.trans_size);

    ret_sta = csi_iic_init(&iic_hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "iic %d init error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_iic_mode(&iic_hd, IIC_MODE_SLAVE);

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

    ret_sta = csi_iic_own_addr(&iic_hd, td.own_addr);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config own address error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }

    volatile uint8_t lock;
    ret_sta = csi_iic_attach_callback(&iic_hd, iic_send_callback, (void *)&lock);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d attach callback error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }


    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_detach_callback(&iic_hd);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    lock = 1;
    TEST_CASE_READY();
    ret_sta = csi_iic_slave_send_async(&iic_hd, send_data, td.trans_size);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d slave async send error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    while (lock) ;

    csi_iic_detach_callback(&iic_hd);

    free(send_data);

    csi_iic_uninit(&iic_hd);

    return 0;
}



int test_iic_masterSyncSend(char *args)
{
    test_iic_args_t td;
    int ret;
    csi_error_t ret_sta;
    csi_iic_t iic_hd;
    int32_t ret_num;

    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)_dev_idx;
    td.addr_mode = (uint8_t)get_data[0];
    td.speed = (uint8_t)get_data[1];
    td.trans_size = get_data[2];
    td.slave_addr = get_data[3];
    td.timeout = get_data[4];

    TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_MASTER);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's speed is %d", td.speed);
    TEST_CASE_TIPS("send size is %d", td.trans_size);
    TEST_CASE_TIPS("select slave address is %d", td.slave_addr);
    TEST_CASE_TIPS("set send timeout is %d", td.timeout);


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


    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);
    TEST_CASE_READY();
    ret_num = csi_iic_master_send(&iic_hd, td.slave_addr, send_data, td.trans_size, td.timeout);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "iic %d async send error,should return %d,but returned %d.", td.dev_idx, td.trans_size, ret_num);



    free(send_data);

    csi_iic_uninit(&iic_hd);

    return 0;
}



int test_iic_slaveSyncSend(char *args)
{
    test_iic_args_t td;
    int ret;
    csi_error_t ret_sta;
    csi_iic_t iic_hd;
    int32_t ret_num;

    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)_dev_idx;
    td.addr_mode = (uint8_t)get_data[0];
    td.speed = (uint8_t)get_data[1];
    td.own_addr = get_data[2];
    td.trans_size = get_data[3];
    td.timeout = get_data[4];

    TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_SLAVE);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's speed is %d", td.speed);
    TEST_CASE_TIPS("config iic's own address is %d", td.own_addr);
    TEST_CASE_TIPS("send size is %d", td.trans_size);
    TEST_CASE_TIPS("set send timeout is %d", td.timeout);


    ret_sta = csi_iic_init(&iic_hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "iic %d init error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_iic_mode(&iic_hd, IIC_MODE_SLAVE);

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

    ret_sta = csi_iic_own_addr(&iic_hd, td.own_addr);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config own address error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }



    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);
    TEST_CASE_READY();
    ret_num = csi_iic_slave_send(&iic_hd, send_data, td.trans_size, td.timeout);
    TEST_CASE_ASSERT(ret_num == td.trans_size, "iic %d async send error,should return %d,but returned %d.", td.dev_idx, td.trans_size, ret_num);

    free(send_data);

    csi_iic_uninit(&iic_hd);
    return 0;
}



int test_iic_masterDmaSend(char *args)
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


    volatile uint8_t lock;
    ret_sta = csi_iic_attach_callback(&iic_hd, iic_send_callback, (void *)&lock);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d attach callback error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }

    csi_dma_ch_t tx_dma;
    ret_sta = csi_iic_link_dma(&iic_hd, &tx_dma, NULL);

    if (ret_sta != 0) {
        csi_iic_detach_callback(&iic_hd);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d link DMA channel error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }



    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_detach_callback(&iic_hd);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    lock = 1;
    TEST_CASE_READY();
    ret_sta = csi_iic_master_send_async(&iic_hd, td.slave_addr, send_data, td.trans_size);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d master async send error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    while (lock) ;

    ret_sta = csi_iic_link_dma(&iic_hd, NULL, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d unlink DMA channel error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    csi_iic_detach_callback(&iic_hd);

    free(send_data);

    csi_iic_uninit(&iic_hd);

    return 0;
}


int test_iic_slaveDmaSend(char *args)
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
    TEST_CASE_TIPS("send size is %d", td.trans_size);

    ret_sta = csi_iic_init(&iic_hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "iic %d init error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_iic_mode(&iic_hd, IIC_MODE_SLAVE);

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

    ret_sta = csi_iic_own_addr(&iic_hd, td.own_addr);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d config own address error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }

    volatile uint8_t lock;
    ret_sta = csi_iic_attach_callback(&iic_hd, iic_send_callback, (void *)&lock);

    if (ret_sta != 0) {
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d attach callback error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }

    csi_dma_ch_t tx_dma;
    ret_sta = csi_iic_link_dma(&iic_hd, &tx_dma, NULL);

    if (ret_sta != 0) {
        csi_iic_detach_callback(&iic_hd);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "iic %d link DMA channel error,should return 0,but returned %d.", td.dev_idx, ret_sta);
    }



    char *send_data = NULL;
    send_data = (char *)malloc(td.trans_size);

    if (send_data == NULL) {
        csi_iic_detach_callback(&iic_hd);
        csi_iic_uninit(&iic_hd);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    transfer_data(send_data, td.trans_size);

    lock = 1;
    TEST_CASE_READY();
    ret_sta = csi_iic_slave_send_async(&iic_hd, send_data, td.trans_size);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d master async send error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    while (lock) ;

    ret_sta = csi_iic_link_dma(&iic_hd, NULL, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "iic %d unlink DMA channel error,should return 0,but returned %d.", td.dev_idx, ret_sta);

    csi_iic_detach_callback(&iic_hd);

    free(send_data);

    csi_iic_uninit(&iic_hd);

    return 0;
}



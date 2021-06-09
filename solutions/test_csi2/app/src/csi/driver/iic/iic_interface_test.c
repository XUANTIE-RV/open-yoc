/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <iic_test.h>

int test_iic_interface(char *args)
{
    csi_error_t ret_sta;

    ret_sta = csi_iic_init(NULL, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_init interface test error,should return CSI_OK,but returned %d.", ret_sta);

    csi_iic_uninit(NULL);

    ret_sta = csi_iic_mode(NULL, IIC_MODE_MASTER);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_mode interface test error,should return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_iic_addr_mode(NULL, IIC_ADDRESS_7BIT);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_addr_mode interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_speed(NULL, IIC_BUS_SPEED_STANDARD);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_speed interface test error,should return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_iic_own_addr(NULL, 0x0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_own_addr interface test error,should return CSI_OK,but returned %d.", ret_sta);

    const char *data_sta = "123";
    ret_sta = csi_iic_master_send(NULL, 0x0, data_sta, 3, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_master_send interface test error,should return CSI_OK,but returned %d.", ret_sta);

    char data_rece[5];
    ret_sta = csi_iic_master_receive(NULL, 0x0, data_rece, 5, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_master_receive interface test error,should return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_iic_master_send_async(NULL, 0x0, data_sta, 3);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_master_send_async interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_master_receive_async(NULL, 0x0, data_rece, 5);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_master_receive_async interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_mem_send(NULL, 0x0, 0x0, IIC_MEM_ADDR_SIZE_8BIT, data_sta, 3, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_mem_send interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_mem_receive(NULL, 0x0, 0x0, IIC_MEM_ADDR_SIZE_8BIT, data_rece, 5, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_mem_receive interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_slave_send(NULL, data_sta, 3, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_slave_send interface test error,should return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_iic_slave_receive(NULL, data_rece, 5, 0);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_slave_receive interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_slave_send_async(NULL, data_sta, 3);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_slave_send_async interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_slave_receive_async(NULL, data_rece, 5);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_slave_receive_async interface test error,should return CSI_OK,but returned %d.", ret_sta);

    ret_sta = csi_iic_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_attach_callback interface test error,should return CSI_OK,but returned %d.", ret_sta);

    csi_iic_detach_callback(NULL);

    ret_sta = csi_iic_xfer_pending(NULL, false);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_xfer_pending interface test error,should return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_iic_link_dma(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_link_dma interface test error,should return CSI_OK,but returned %d.", ret_sta);

    csi_state_t state;
    ret_sta = csi_iic_get_state(NULL, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "csi_iic_get_state interface test error,should return CSI_OK,but returned %d.", ret_sta);

    return 0;
}

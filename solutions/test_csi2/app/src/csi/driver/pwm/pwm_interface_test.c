/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pwm_test.h>

int test_pwm_interface(char *args)
{
    csi_error_t ret_sta;

    ret_sta = csi_pwm_init(NULL, 0);
    TEST_CASE_ASSERT(ret_sta != CSI_OK, "csi_pwm_init interface test error,shouldn't return CSI_OK,but returned %d.", ret_sta);

    csi_pwm_uninit(NULL);

    ret_sta = csi_pwm_out_config(NULL, 0, 10, 5, 0);
    TEST_CASE_ASSERT(ret_sta != CSI_OK, "csi_pwm_out_config interface test error,shouldn't return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_pwm_out_start(NULL, 0);
    TEST_CASE_ASSERT(ret_sta != CSI_OK, "csi_pwm_out_start interface test error,shouldn't return CSI_OK,but returned %d.", ret_sta);


    csi_pwm_out_stop(NULL, 0);


    ret_sta = csi_pwm_capture_config(NULL, 0, 0, 1);
    TEST_CASE_ASSERT(ret_sta != CSI_OK, "csi_pwm_capture_config interface test error,shouldn't return CSI_OK,but returned %d.", ret_sta);


    ret_sta = csi_pwm_capture_start(NULL, 1);
    TEST_CASE_ASSERT(ret_sta != CSI_OK, "csi_pwm_capture_start interface test error,shouldn't return CSI_OK,but returned %d.", ret_sta);


    csi_pwm_capture_stop(NULL, 1);

    ret_sta = csi_pwm_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_sta != CSI_OK, "csi_pwm_attach_callback interface test error,shouldn't return CSI_OK,but returned %d.", ret_sta);


    csi_pwm_detach_callback(NULL);

    return 0;
}

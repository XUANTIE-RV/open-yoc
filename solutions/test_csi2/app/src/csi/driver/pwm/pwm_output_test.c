/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pwm_test.h>

int test_pwm_output(char *args)
{
    csi_error_t ret_sta;
    int ret;
    test_pwm_args_t td;
    csi_pwm_t hd;
    uint32_t get_data[5];

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)_dev_idx;
    td.channel = (uint8_t)get_data[0];
    td.period = (uint32_t)get_data[1] * 1000;
    td.pulse_width = (uint32_t)get_data[2] * 1000;
    td.output_polarity = (uint8_t)get_data[3];
    td.delay_ms = (uint32_t)get_data[4];

    TEST_CASE_TIPS("test PWM idx is %d", td.dev_idx);
    TEST_CASE_TIPS("test pwm channel is %d", td.channel);
    TEST_CASE_TIPS("config period is %d us", td.period);
    TEST_CASE_TIPS("config pulse width is %d us", td.pulse_width);
    TEST_CASE_TIPS("config output polarity is %d", td.output_polarity);
    TEST_CASE_TIPS("config output time is %d ms", td.delay_ms);


    ret_sta = csi_pwm_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "pwm %d init error,should return CSI_OK, but returned %d.", td.dev_idx, ret_sta);


    ret_sta = csi_pwm_out_config(&hd, td.channel, td.period, td.pulse_width, td.output_polarity);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "pwm %d out config error,should return CSI_OK, but returned %d.", td.dev_idx, ret_sta);

    TEST_CASE_READY();

    ret_sta = csi_pwm_out_start(&hd, td.channel);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "pwm channel %d output error,should return CSI_OK, but returned %d.", td.dev_idx, ret_sta);

    tst_mdelay(td.delay_ms);

    csi_pwm_out_stop(&hd, td.channel);


    csi_pwm_uninit(&hd);
    return 0;
}

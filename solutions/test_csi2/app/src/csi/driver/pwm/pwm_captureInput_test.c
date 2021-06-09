/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pwm_test.h>

int test_pwm_captureInput(char *args)
{
    csi_gpio_t hd;
    csi_error_t ret_sta;

    int ret;
    test_pwm_args_t td;
    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)_dev_idx;
    td.pin = (uint8_t)get_data[0];
    td.capture_polarity = (uint8_t)get_data[1];
    td.capture_count = (uint32_t)get_data[2];
    td.delay_ms = (uint32_t)get_data[3];

    TEST_CASE_TIPS("config capture input gpio idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config  capture input pin is %d", td.pin);
    TEST_CASE_TIPS("test capture polarity is %d", td.capture_polarity);
    TEST_CASE_TIPS("test capture count is %d", td.capture_count);
    TEST_CASE_TIPS("config delay of %d ms, the single is output", td.delay_ms);


    ret_sta = csi_gpio_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_gpio_dir(&hd, 1 << td.pin, GPIO_DIRECTION_OUTPUT);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d config dir error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_gpio_mode(&hd, 1 << td.pin, GPIO_MODE_PULLNONE);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d config mode error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);



    if (td.capture_polarity == PWM_CAPTURE_POLARITY_POSEDGE) {
        csi_gpio_write(&hd, 1 << td.pin, 0);
        TEST_CASE_READY();
        tst_mdelay(td.delay_ms);

        for (int i = 0; i < td.capture_count; i++) {
            tst_mdelay(1);
            csi_gpio_write(&hd, 1 << td.pin, 0);
            tst_mdelay(9);
            csi_gpio_write(&hd, 1 << td.pin, 1);
        }
    }

    if (td.capture_polarity == PWM_CAPTURE_POLARITY_NEGEDGE) {
        csi_gpio_write(&hd, 1 << td.pin, 1);
        TEST_CASE_READY();
        tst_mdelay(td.delay_ms);

        for (int i = 0; i < td.capture_count; i++) {
            tst_mdelay(1);
            csi_gpio_write(&hd, 1 << td.pin, 1);
            tst_mdelay(9);
            csi_gpio_write(&hd, 1 << td.pin, 0);
        }
    }


    if (td.capture_polarity == PWM_CAPTURE_POLARITY_BOTHEDGE) {
        csi_gpio_write(&hd, 1 << td.pin, 0);
        TEST_CASE_READY();
        tst_mdelay(td.delay_ms);

        for (int i = 0; i < td.capture_count; i++) {
            tst_mdelay(10);
            csi_gpio_toggle(&hd, 1 << td.pin);
        }
    }

    csi_gpio_uninit(&hd);
    return 0;
}

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <gpio_test.h>


int test_gpio_toggle(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_OUTPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];
    td.delay_ms = (uint32_t)get_data[3];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pin is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);
    TEST_CASE_TIPS("output delay %d ms", td.delay_ms);


    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    csi_gpio_pin_state_t value;

    if (td.pin_value == GPIO_PIN_LOW) {
        value = GPIO_PIN_HIGH;
    } else {
        value = GPIO_PIN_LOW;
    }


    csi_gpio_write(&hd, td.pin_mask, value);

    tst_mdelay(50);

    csi_gpio_toggle(&hd, td.pin_mask);

    TEST_CASE_READY();

    tst_mdelay(td.delay_ms);

    csi_gpio_uninit(&hd);
    return 0;
}



int test_gpio_debonceToggle(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_OUTPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];
    td.delay_ms = (uint32_t)get_data[3];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pin is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);
    TEST_CASE_TIPS("output delay %d ms", td.delay_ms);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0, but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_debonce(&hd, td.pin_mask, true);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio open debonce error,should return 0, but returned %d.", ret_status);

    csi_gpio_pin_state_t value;

    value = td.pin_value;


    csi_gpio_write(&hd, td.pin_mask, value);
    tst_mdelay(50);

    csi_gpio_toggle(&hd, td.pin_mask);

    TEST_CASE_READY();

    tst_mdelay(td.delay_ms);


    ret_status = csi_gpio_debonce(&hd, td.pin_mask, false);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio close debonce error,should return 0, but returned %d.", ret_status);

    csi_gpio_uninit(&hd);
    return 0;
}

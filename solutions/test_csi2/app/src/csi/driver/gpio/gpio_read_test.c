/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <gpio_test.h>

int test_gpio_pinsRead(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t read_value;

    uint32_t expec_value, actual_value;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected read value is %d", td.pin_value);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    expec_value = 0xFFFFFFFF & td.pin_mask;


    TEST_CASE_READY();
    read_value = csi_gpio_read(&hd, td.pin_mask);
    actual_value = read_value & td.pin_mask;

    if (td.pin_value) {
        TEST_CASE_ASSERT(actual_value == expec_value, "gpio read error,should return %x,but returned %x", expec_value, actual_value);
    } else {
        TEST_CASE_ASSERT(actual_value == 0, "gpio read error,should return 0,but returned %x",  actual_value);
    }

    csi_gpio_uninit(&hd);

    return 0;
}



int test_gpio_deboncePinsRead(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;
    uint32_t read_value;

    uint32_t expec_value, actual_value;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected read value is %d", td.pin_value);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    expec_value = 0xFFFFFFFF & td.pin_mask;

    ret_status = csi_gpio_debonce(&hd, td.pin_mask, true);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio open debonce error,should return 0,but returned %d.", ret_status);

    TEST_CASE_READY();

    read_value = csi_gpio_read(&hd, td.pin_mask);
    actual_value = read_value & td.pin_mask;

    ret_status = csi_gpio_debonce(&hd, td.pin_mask, false);
    TEST_CASE_ASSERT_QUIT(ret_status == -4, "gpio close debonce error,should return 0,but returned %d.", ret_status);

    if (td.pin_value) {
        TEST_CASE_ASSERT(actual_value == expec_value, "gpio read error,should return %x,but returned %x", expec_value, actual_value);
    } else {
        TEST_CASE_ASSERT(actual_value == 0, "gpio read error,should return 0,but returned %x",  actual_value);
    }

    csi_gpio_uninit(&hd);
    return 0;
}




int test_gpio_pinRead(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t read_value;

    uint32_t expec_value, actual_value;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin = (uint8_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pin is %d", td.pin);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected read value is %d", td.pin_value);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, 1 << td.pin, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, 1 << td.pin, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    expec_value = td.pin_value;

    TEST_CASE_READY();

    read_value = csi_gpio_read(&hd, 1 << td.pin);
    actual_value = (read_value & (1 << td.pin)) >> td.pin;

    TEST_CASE_ASSERT(actual_value == expec_value, "gpio read error,should return %x,but returned %x", expec_value, actual_value);

    csi_gpio_uninit(&hd);

    return 0;
}



int test_gpio_deboncePinRead(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;
    uint32_t read_value;

    uint32_t expec_value, actual_value;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin = (uint8_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pin is %d", td.pin);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected read value is %d", td.pin_value);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, 1 << td.pin, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, 1 << td.pin, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    expec_value = td.pin_value;

    ret_status = csi_gpio_debonce(&hd, 1 << td.pin, true);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio open debonce error,should return 0,but returned %d.", ret_status);

    TEST_CASE_READY();

    read_value = csi_gpio_read(&hd, 1 << td.pin);
    actual_value = (read_value & (1 << td.pin)) >> td.pin;

    ret_status = csi_gpio_debonce(&hd, 1 << td.pin, false);
    TEST_CASE_ASSERT_QUIT(ret_status == -4, "gpio close debonce error,should return 0,but returned %d.", ret_status);

    TEST_CASE_ASSERT(actual_value == expec_value, "gpio read error,should return %x,but returned %x", expec_value, actual_value);

    csi_gpio_uninit(&hd);
    return 0;
}



int test_gpio_pinsToggleRead(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;

    uint32_t read_value;

    uint32_t expec_value, actual_value;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected read value is %d", td.pin_value);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    expec_value = 0xFFFFFFFF & td.pin_mask;


    TEST_CASE_READY();
    read_value = csi_gpio_read(&hd, td.pin_mask);
    actual_value = read_value & td.pin_mask;

    if (td.pin_value) {
        TEST_CASE_ASSERT(actual_value == 0, "gpio read error,should return 0,but returned %x",  actual_value);
    } else {
        TEST_CASE_ASSERT(actual_value == expec_value, "gpio read error,should return %x,but returned %x", expec_value, actual_value);
    }

    csi_gpio_uninit(&hd);

    return 0;
}



int test_gpio_deboncePinsToggleRead(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;
    uint32_t read_value;

    uint32_t expec_value, actual_value;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.pin_value = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected read value is %d", td.pin_value);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error,should return 0,but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error,should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error,should return 0,but returned %d.", ret_status);


    expec_value = 0xFFFFFFFF & td.pin_mask;

    ret_status = csi_gpio_debonce(&hd, td.pin_mask, true);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio open debonce error,should return 0,but returned %d.", ret_status);

    TEST_CASE_READY();

    read_value = csi_gpio_read(&hd, td.pin_mask);
    actual_value = read_value & td.pin_mask;

    ret_status = csi_gpio_debonce(&hd, td.pin_mask, false);
    TEST_CASE_ASSERT_QUIT(ret_status == -4, "gpio close debonce error,should return 0,but returned %d.", ret_status);

    if (td.pin_value) {
        TEST_CASE_ASSERT(actual_value == 0, "gpio read error,should return 0,but returned %x",  actual_value);
    } else {
        TEST_CASE_ASSERT(actual_value == expec_value, "gpio read error,should return %x,but returned %x", expec_value, actual_value);
    }

    csi_gpio_uninit(&hd);
    return 0;
}

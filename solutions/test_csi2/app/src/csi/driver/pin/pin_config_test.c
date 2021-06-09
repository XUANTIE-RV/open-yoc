/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pin_test.h>

int test_pin_config(char *args)
{
    uint32_t ret = 0;
    uint32_t get_data[5] = {0};
    test_pin_args_t pin_arg = {0};
    pin_func_t pin_func = 0;
    uint32_t gpio_idx = 0;
    uint32_t gpio_channel = 0;
    pin_name_t pin_name = 0;

    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    memset((char *)&pin_arg, 0, sizeof(pin_arg));
    pin_arg.idx = get_data[0];
    pin_arg.func = get_data[1];
    pin_arg.mode = get_data[2];
    pin_arg.speed = get_data[3];
    pin_arg.drive = get_data[4];

    TEST_CASE_TIPS("begin test pin idx %u, func %u, mode %u, speed %u, drive %u ", \
                   pin_arg.idx, pin_arg.func, pin_arg.mode, pin_arg.speed, pin_arg.drive);

    if (pin_arg.idx < PA0 || pin_arg.idx > PA31) {
        TEST_CASE_ASSERT_QUIT(0, "pin_arg.idx %d error", pin_arg.idx);
    }

    if (pin_arg.mode < GPIO_MODE_PULLNONE || pin_arg.mode > GPIO_MODE_PUSH_PULL) {
        TEST_CASE_ASSERT_QUIT(0, "pin_arg.mode %d error", pin_arg.mode);
    }

    if (pin_arg.speed < PIN_SPEED_LV0 || pin_arg.speed > PIN_SPEED_LV3) {
        TEST_CASE_ASSERT_QUIT(0, "pin_arg.speed %d error", pin_arg.speed);
    }

    if (pin_arg.drive < PIN_DRIVE_LV0 || pin_arg.drive > PIN_DRIVE_LV3) {
        TEST_CASE_ASSERT_QUIT(0, "pin_arg.drive %d error", pin_arg.drive);
    }

    ret = csi_pin_set_mux(pin_arg.idx, pin_arg.func);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_pin_set_mux failed with idx: %d", pin_arg.idx);

    pin_func = csi_pin_get_mux(pin_arg.idx);
    TEST_CASE_ASSERT_QUIT(pin_func == pin_arg.func, "pin_func no match pin_arg.func");

    ret = csi_pin_mode(pin_arg.idx, pin_arg.mode);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_pin_mode failed with idx: %d", pin_arg.idx);

    ret = csi_pin_speed(pin_arg.idx, pin_arg.speed);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_pin_speed failed with idx: %d", pin_arg.idx);

    ret = csi_pin_drive(pin_arg.idx, pin_arg.drive);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "csi_pin_drive failed with idx: %d", pin_arg.idx);

    if (pin_arg.func == PIN_FUNC_GPIO) {
        gpio_idx = csi_pin_get_gpio_devidx(pin_arg.idx);
        csi_pin_get_uart_devidx(pin_arg.idx);
        csi_pin_get_iic_devidx(pin_arg.idx);
        csi_pin_get_spi_devidx(pin_arg.idx);
        csi_pin_get_i2s_devidx(pin_arg.idx);
        TEST_CASE_TIPS("gpio_idx %u", gpio_idx);

        gpio_channel = csi_pin_get_gpio_channel(pin_arg.idx);
        csi_pin_get_pwm_channel(pin_arg.idx);
        csi_pin_get_adc_channel(pin_arg.idx);
        TEST_CASE_TIPS("gpio_channel %u", gpio_channel);

        pin_name = csi_pin_get_pinname_by_gpio((uint8_t)gpio_idx, (uint8_t)gpio_channel);
        TEST_CASE_ASSERT_QUIT(pin_name == pin_arg.idx,
                              "csi_pin_get_pinname_by_gpio pin_name %d, idx: %u", pin_name, pin_arg.idx);
    }

    TEST_CASE_TIPS("pin test finish");

    return 0;
}

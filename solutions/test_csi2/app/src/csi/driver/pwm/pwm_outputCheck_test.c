/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pwm_test.h>

static volatile uint32_t period_time[10] = {0}, high_level_time[10] = {0};
static volatile uint8_t counter = 0;
static volatile uint8_t lock = 0;

static void gpio_callback(csi_gpio_t *gpio, uint32_t pins, void *arg)
{
    lock = 0;

    if ((pins & (1 << (*(uint8_t *)arg))) && (counter < 10)) {
        period_time[counter] = tst_timer_get_interval();
        counter++;
    }
}


int test_pwm_outputCheck(char *args)
{
    csi_gpio_t hd;
    csi_error_t ret_sta;
    uint32_t read_value;
    uint32_t period = 0;
    uint32_t pulse_width = 0;


    int ret;
    test_pwm_args_t td;
    uint32_t get_data[4];

    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)_dev_idx;
    td.pin = (uint8_t)get_data[0];
    td.period = (uint32_t)get_data[1] * 1000;
    td.pulse_width = (uint32_t)get_data[2] * 1000;
    td.output_polarity = (uint8_t)get_data[3];

    TEST_CASE_TIPS("config PWM output check gpio idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config  PWM output check pin is %d", td.pin);
    TEST_CASE_TIPS("config period is %d us", td.period);
    TEST_CASE_TIPS("config pulse width is %d us", td.pulse_width);
    TEST_CASE_TIPS("config output polarity is %d", td.output_polarity);



    ret_sta = csi_gpio_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_gpio_dir(&hd, 1 << td.pin, GPIO_DIRECTION_INPUT);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d config dir error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);


    ret_sta = csi_gpio_mode(&hd, 1 << td.pin, GPIO_MODE_PULLUP);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d config mode error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_gpio_irq_mode(&hd, 1 << td.pin, GPIO_IRQ_MODE_RISING_EDGE);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d config irq mode error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_gpio_attach_callback(&hd, gpio_callback, &td.pin);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d attach callback error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);


    counter = 0;
    lock = 1;
    TEST_CASE_READY();

    tst_timer_restart();
    ret_sta = csi_gpio_irq_enable(&hd, 1 << td.pin, true);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "gpio %d pin irq enable error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    for (; counter < 10;) {
        if (lock == 0) {
            read_value = csi_gpio_read(&hd, 1 << td.pin);

            if ((read_value & 1 << td.pin) == 0) {
                high_level_time[counter] = tst_timer_get_interval();
                lock = 1;
            }
        }
    }

    ret_sta = csi_gpio_irq_enable(&hd, 1 << td.pin, false);
    TEST_CASE_ASSERT(ret_sta == CSI_OK, "gpio %d pin irq disable error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    csi_gpio_detach_callback(&hd);

    csi_gpio_uninit(&hd);

    for (int i = 0; i < 9; i++) {
        period += period_time[i + 1] - period_time[i];
        pulse_width += high_level_time[i + 1] - period_time[i];
    }

    period = period / 9;

    if (((period - 1) > td.period / 1000) || ((period + 1) < td.period / 1000)) {
        TEST_CASE_ASSERT(1 == 0, "the testing value of period is %d, but value set is %d", period, td.period / 1000);
    }

    pulse_width = pulse_width / 9;

    if (td.output_polarity == PWM_POLARITY_HIGH) {
        if (((pulse_width - 1) > td.pulse_width / 1000) || ((pulse_width + 1) < td.pulse_width / 1000)) {
            TEST_CASE_ASSERT(1 == 0, "the testing value of pulse_width is %d,but value set is %d", pulse_width, td.pulse_width / 1000);
        }
    } else {
        pulse_width = period - pulse_width;

        if (((pulse_width - 1) > td.pulse_width / 1000) || ((pulse_width + 1) < td.pulse_width / 1000)) {
            TEST_CASE_ASSERT(1 == 0, "the testing value of pulse_width is %d,but value set is %d", pulse_width, td.pulse_width / 1000);
        }
    }

    return 0;
}

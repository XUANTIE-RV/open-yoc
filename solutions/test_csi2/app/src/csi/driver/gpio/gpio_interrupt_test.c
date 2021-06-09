/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <gpio_test.h>

static volatile uint8_t lock = 0;
static volatile uint32_t irq_pin;

static void gpio_callback(csi_gpio_t *hd, uint32_t pins, void *arg)
{
    lock++;
    irq_pin = pins;

    if (*(csi_gpio_irq_mode_t *)arg != GPIO_IRQ_MODE_BOTH_EDGE) {
        csi_gpio_irq_enable(hd, pins, false);
    }
}

int test_gpio_interruptCapture(char *args)
{
    csi_gpio_t hd;
    test_gpio_args_t td;
    int ret;
    csi_error_t ret_status;
    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.gpio_idx = (uint8_t)_dev_idx;
    td.pin_mask = (uint32_t)get_data[0];
    td.dir = GPIO_DIRECTION_INPUT;
    td.gpio_mode = (uint8_t)get_data[1];
    td.gpio_irq_mode = (uint8_t)get_data[2];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pin is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("config irq mode is %d", td.gpio_irq_mode);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error, should return 0, but returned  %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error: should return 0,but returned  %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error: should return 0,but returned  %d.", ret_status);

    ret_status = csi_gpio_irq_mode(&hd, td.pin_mask, td.gpio_irq_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio irq mode config error: should return 0,but returned  %d.", ret_status);

    ret_status = csi_gpio_attach_callback(&hd, gpio_callback, &td.gpio_irq_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio attach callback error, should return 0, but returned  %d.", ret_status);

    ret_status = csi_gpio_irq_enable(&hd, td.pin_mask, true);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio irq enable error, should return 0, but returned  %d.", ret_status);

    lock = 0;

    TEST_CASE_READY();

    if (td.gpio_irq_mode == GPIO_IRQ_MODE_BOTH_EDGE) {
        while (lock != 2) {
            ;
        }
    } else {
        while (lock != 1) {
            ;
        }
    }

    ret_status = csi_gpio_irq_enable(&hd, td.pin_mask, false);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio irq disable error, should return 0, but returned  %d.", ret_status);

    TEST_CASE_ASSERT(irq_pin == td.pin_mask, "gpio trigger rising interrupt error, expected trigger pis is 0x%x,actual trigger pins is 0x%x", td.pin_mask, irq_pin);

    csi_gpio_detach_callback(&hd);

    csi_gpio_uninit(&hd);
    return 0;
}


int test_gpio_interruptTrigger(char *args)
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
    td.gpio_irq_mode = (uint8_t)get_data[2];
    td.delay_ms = (uint32_t)get_data[3];

    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pin is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir);
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("config triggle signle is %d", td.gpio_irq_mode);
    TEST_CASE_TIPS("config trigger delay is %d ms", td.delay_ms);

    ret_status = csi_gpio_init(&hd, td.gpio_idx);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio %d init error, should return 0, but returned %d.", td.gpio_idx, ret_status);

    ret_status = csi_gpio_dir(&hd, td.pin_mask, td.dir);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio direction config error: should return 0,but returned %d.", ret_status);

    ret_status = csi_gpio_mode(&hd, td.pin_mask, td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret_status == 0, "gpio mode config error: should return 0,but returned %d.", ret_status);


    switch (td.gpio_irq_mode) {
        case GPIO_IRQ_MODE_RISING_EDGE:
            csi_gpio_write(&hd, td.pin_mask, false);
            TEST_CASE_READY();
            tst_mdelay(td.delay_ms);
            csi_gpio_write(&hd, td.pin_mask, true);
            break;

        case GPIO_IRQ_MODE_FALLING_EDGE:
            csi_gpio_write(&hd, td.pin_mask, true);
            TEST_CASE_READY();
            tst_mdelay(td.delay_ms);
            csi_gpio_write(&hd, td.pin_mask, false);
            break;

        case GPIO_IRQ_MODE_BOTH_EDGE:
            csi_gpio_write(&hd, td.pin_mask, false);
            TEST_CASE_READY();
            tst_mdelay(td.delay_ms);
            csi_gpio_write(&hd, td.pin_mask, true);
            tst_mdelay(100);
            csi_gpio_write(&hd, td.pin_mask, false);
            break;

        case GPIO_IRQ_MODE_LOW_LEVEL:
            csi_gpio_write(&hd, td.pin_mask, true);
            TEST_CASE_READY();
            tst_mdelay(td.delay_ms);
            csi_gpio_write(&hd, td.pin_mask, false);
            break;

        case GPIO_IRQ_MODE_HIGH_LEVEL:
            csi_gpio_write(&hd, td.pin_mask, false);
            TEST_CASE_READY();
            tst_mdelay(td.delay_ms);
            csi_gpio_write(&hd, td.pin_mask, true);
            break;

        default:
            break;


    }

    tst_mdelay(100);

    csi_gpio_uninit(&hd);
    return 0;
}


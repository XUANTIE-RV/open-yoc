/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "aos/hal/gpio.h"
#include "aos/hal/pwm.h"
#include "aos/hal/adc.h"
#include "aos/hal/uart.h"
#include "aos/hal/wdg.h"
#include "app_main.h"

/*gpio demo*/
extern void hal_gpio_app_test(int id);
void hal_gpio_app_int(int id, int trigger_method);

/*pwm demo*/
extern void hal_pwm_app_static_out(void);
extern void hal_pwm_app_dynamic_out(void);

/*adc demo*/
extern void hal_adc_app_out(void);

/*wdg demo*/
extern void hal_watchdog_app_enable(void);

#include "drv/gpio.h"
#include "drv/gpio_pin.h"
#include "drv/pin.h"

void aoshal_test(void)
{
    printf("hal app test start\r\n");

    aos_msleep(3000);

    /* GPIO output high and low level test */
    // hal_gpio_app_test(GPIO_PIN_17);
    // hal_gpio_app_test(GPIO_PIN_18);
    // hal_gpio_app_test(GPIO_PIN_19);
    // hal_gpio_app_test(GPIO_PIN_41);

    /* GPIO edge-triggered test */
    // hal_gpio_app_int(GPIO_PIN_17, IRQ_TRIGGER_RISING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_17, IRQ_TRIGGER_FALLING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_18, IRQ_TRIGGER_RISING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_18, IRQ_TRIGGER_FALLING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_19, IRQ_TRIGGER_RISING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_19, IRQ_TRIGGER_FALLING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_41, IRQ_TRIGGER_RISING_EDGE);
    // hal_gpio_app_int(GPIO_PIN_41, IRQ_TRIGGER_FALLING_EDGE);

    /* pwm */
    // hal_pwm_app_static_out();

    /* adc */
    hal_adc_app_out();

    /*
    printf("After 10 seconds the system will be restarted by"
            "triggering the watchdog\r\n");*/
    // hal_watchdog_app_enable();

    printf("hal app test end!\r\n");
}

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __GPIO_TEST__
#define __GPIO_TEST__

#include <stdint.h>
#include <drv/gpio.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
    uint8_t     gpio_idx;
    uint32_t    pin_mask;
    uint8_t     dir;
    uint8_t     gpio_mode;
    uint8_t     gpio_irq_mode;
	uint32_t	delay_ms;
	uint8_t		pin;
	uint8_t		pin_value;
}test_gpio_args_t;


extern int test_gpio_pinsWrite(char *args);
extern int test_gpio_pinWrite(char *args);
extern int test_gpio_pinDebonceWrite(char *args);
extern int test_gpio_pinsDebonceWrite(char *args);
extern int test_gpio_pinRead(char *args);
extern int test_gpio_deboncePinRead(char *args);
extern int test_gpio_pinsRead(char *args);
extern int test_gpio_deboncePinsRead(char *args);
extern int test_gpio_interruptCapture(char *args);
extern int test_gpio_toggle(char *args);
extern int test_gpio_debonceToggle(char *args);
extern int test_gpio_pinsToggleRead(char *args);
extern int test_gpio_deboncePinsToggleRead(char *args);
extern int test_gpio_interruptTrigger(char *args);
extern int test_gpio_main(char *args);
#endif

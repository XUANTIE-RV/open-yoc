/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __PWM_TEST__
#define __PWM_TEST__

#include <stdint.h>
#include <drv/pwm.h>
#include <drv/gpio.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <pinmux_test.h>


typedef struct {
    uint32_t    dev_idx;
    uint8_t     channel;
    uint32_t    period;
    uint32_t    pulse_width;
    uint8_t     output_polarity;
	uint8_t		capture_polarity;
	uint32_t	capture_count;
	uint8_t		pin;
	uint32_t	delay_ms;
}test_pwm_args_t;




extern int test_pwm_interface(char *args);
extern int test_pwm_output(char *args);
extern int test_pwm_outputCheck(char *args);
extern int test_pwm_capture(char *args);
extern int test_pwm_captureInput(char *args);
extern int test_pwm_main(char *args);


#endif

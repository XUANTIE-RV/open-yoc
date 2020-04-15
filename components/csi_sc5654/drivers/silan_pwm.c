/*
 * silan_pwm.c
 */

//#define __DEBUG__

#include "silan_pwm.h"
#include "silan_printf.h"
#include "silan_syscfg.h"
#include "silan_iomux.h"

int silan_pwm_config(uint8_t open_num, uint32_t x_duty, uint32_t x_period)
{
	if(open_num > 7)
		return -1;
	if(!x_duty)
		return -1;
	if(!x_period)
		return -1;

	silan_pwm_duty_set(open_num,x_duty);
	silan_pwm_period_set(open_num,x_period);

	return 0;
}

int silan_io_pwm_config(uint32_t io, uint32_t x_duty, uint32_t x_period)
{
	uint8_t open_num;

	open_num = IO_TO_PWM(io);
	silan_pwm_config(open_num, x_duty, x_period);
	silan_pwm_channel_open(open_num);

	silan_io_func_config(io, IO_FUNC_PWM);

	return 0;
}


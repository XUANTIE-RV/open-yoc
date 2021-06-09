/*
 * silan_pwm.h
 */

#ifndef __SILAN_PWM_H__
#define __SILAN_PWM_H__

#include "silan_types.h"
#include "silan_pwm_regs.h"

#define IO_TO_PWM(x)		(x % 8)

static inline int silan_pwm_channel_open(uint8_t channel_num)
{
	if(channel_num > 7)
		return -1;
	PWMCON |=  (1<<(channel_num + 4 ));
	return 0;
}
static inline int silan_pwm_channel_close(uint8_t channel_num)
{
	if(channel_num > 7)
		return -1;
	PWMCON &= ~(1<<(channel_num + 4));
	return 0;
}

static inline int silan_pwm_duty_set(uint8_t channel_num , uint32_t para)
{
	if(channel_num > 7)
		return -1;
	__REG32(SILAN_PWM_REGS_BASE+0x04+channel_num*4) = (para-1);
	return 0;
}

static inline int silan_pwm_period_set(uint8_t channel_num_2 , uint32_t para)
{
	if(channel_num_2 > 7)
		return -1;
	__REG32(SILAN_PWM_REGS_BASE+0x24+channel_num_2/2*4 ) = (para-1);
	return 0;
}

static inline int silan_pwm_prediv_set(uint32_t para)
{
	if((para < 1) || (para > 256))
		return -1;
	PWMPSC = para-1;
	return 0;
}

int silan_pwm_config(uint8_t open_num, uint32_t x_duty, uint32_t x_period);
int silan_io_pwm_config(uint32_t io, uint32_t x_duty, uint32_t x_period);
 
#endif  //__SILAN_PWM_H__


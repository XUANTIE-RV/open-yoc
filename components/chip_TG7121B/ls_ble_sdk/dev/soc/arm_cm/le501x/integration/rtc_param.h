#ifndef _RTC_PARAM_H_
#define _RTC_PARAM_H_

#include <stddef.h>
#include "rtc.h"
#include "reg_rtc.h"
#include "field_manipulate.h"

void rtc_sw_reset(RTC_HandleTypeDef *inst);
void rtc_clock_enable(RTC_HandleTypeDef *inst,uint8_t status);

void rtc_int_op(void (*isr)(RTC_HandleTypeDef *),RTC_HandleTypeDef *inst,uint8_t states);
void RTC_status_set(RTC_HandleTypeDef *inst,uint8_t status);
#endif



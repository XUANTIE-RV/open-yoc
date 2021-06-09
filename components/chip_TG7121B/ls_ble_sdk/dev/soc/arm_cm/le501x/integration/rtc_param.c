#include "rtc_param.h"
#include "reg_rcc.h"
#include "field_manipulate.h"
#include "rtc.h"
#include "reg_rtc.h"
#include "le501x.h"
#include "HAL_def.h"
#include "sys_stat.h"
#include "reg_rcc.h"
#include "reg_rcc_type.h"
static RTC_HandleTypeDef *rtc_inst_env;
void (*rtc_isr)(RTC_HandleTypeDef *);

void rtc_sw_reset(RTC_HandleTypeDef *inst)
{
    if (inst->Instance == RTC)
    {
        REG_FIELD_WR(RCC->AHBRST, RCC_RTC, 1);
        REG_FIELD_WR(RCC->AHBRST, RCC_RTC, 0);
    }
}

void rtc_clock_enable(RTC_HandleTypeDef *inst,uint8_t status)
{
    if (inst->Instance == RTC)
    {
        REG_FIELD_WR(RCC->AHBEN, RCC_RTC, status);
    }
}

void rtc_int_op(void (*isr)(RTC_HandleTypeDef *),RTC_HandleTypeDef *inst,uint8_t states)
{
    if (states)
    {
        rtc_isr=isr; 
        if (inst->Instance == RTC)
        {
            NVIC_ClearPendingIRQ(RTC_IRQn);
            rtc_inst_env = inst;
            NVIC_EnableIRQ(RTC_IRQn);
        }
    }
    else
    {
        if (inst->Instance == RTC)
        {
            NVIC_DisableIRQ(RTC_IRQn);
        }
        rtc_isr = NULL ;
    }
}

void RTC_status_set(RTC_HandleTypeDef *inst,uint8_t status)
{
    rtc_status_set(status);
}
                
void RTC_Handler(void)
{
    rtc_isr(rtc_inst_env);
}

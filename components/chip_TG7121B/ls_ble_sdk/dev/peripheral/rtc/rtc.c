#include <stdio.h>
#include "rtc.h"
#include "rtc_param.h" 
#include "log.h"
#include "field_manipulate.h"
#include "HAL_def.h"
#include "reg_rtc_type.h"
#include "io_config.h"
#include "ls_dbg.h"

static RTC_HandleTypeDef RTC_Config;

static void RTC_SetConfig(uint8_t cksel);

static void HAL_RTC_Init(void)
{
    rtc_sw_reset(&RTC_Config);
    rtc_clock_enable(&RTC_Config,1);
    RTC_SetConfig(RTC_Config.rtc_cksel);
    RTC_status_set(&RTC_Config, 1);
}

static void HAL_RTC_DeInit(void)
{
    rtc_clock_enable(&RTC_Config,0);
    RTC_status_set(&RTC_Config, 0);
}

static void RTC_SetConfig(uint8_t cksel)
{
    LS_ASSERT(cksel < CKSEL_MAX);
    REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_RTCEN,0);
    // REG_FIELD_WR(RTC_Config.Instance->WKUP,RTC_WKUP_WKSEL,0);
    REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_CKSEL,cksel);
}

void RTC_Init(void)
{
    RTC_Config.Instance = RTC;
    RTC_Config.rtc_cksel = CKSEL_LSI;
    HAL_RTC_Init();
}

void RTC_Deinit(void)
{
    HAL_RTC_DeInit();
}

void RTC_CalendarSet(_calendar_cal *calendar_cal,_calendar_time *calendar_time)
{
    REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_RTCEN,0);
    
    REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_YEAR_T,(calendar_cal->year/10)%100);
    REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_YEAR_U,(calendar_cal->year%100)%10);
    REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_MON_T,calendar_cal->mon/10);
    REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_MON_U,calendar_cal->mon%10);
    REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_DATE_T,calendar_cal->date/10);
    REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_DATE_U,calendar_cal->date%10);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_WEEK,calendar_time->week);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_HOUR_T,calendar_time->hour/10);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_HOUR_U,calendar_time->hour%10);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_MIN_T,calendar_time->min/10);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_MIN_U,calendar_time->min%10);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_SEC_T,calendar_time->sec/10);
    REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_SEC_U,calendar_time->sec%10);

    REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_RTCEN,1);
    // REG_FIELD_WR(RTC_Config.Instance->IER,RTC_IER_F1HZ,1);
}

HAL_StatusTypeDef RTC_CalendarGet(_calendar_cal *calendar_cal,_calendar_time *calendar_time)
{
    HAL_StatusTypeDef result = HAL_OK;
    if(!REG_FIELD_RD(RTC_Config.Instance->STA,RTC_STA_EMPTY))
    {
        calendar_cal->year  = REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_YEAR_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_YEAR_U);
        calendar_cal->mon   = REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_MON_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_MON_U); 
        calendar_cal->date  = REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_DATE_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_DATE_U);     
        calendar_time->week = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_WEEK);
        calendar_time->hour = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_HOUR_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_HOUR_U); 
        calendar_time->min  = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_MIN_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_MIN_U);     
        calendar_time->sec  = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_SEC_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_SEC_U);
    }
    else
    {
        result = HAL_STATE_ERROR;
    }
    
    return result;
}


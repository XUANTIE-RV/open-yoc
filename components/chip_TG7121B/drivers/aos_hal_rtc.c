#include "aos/hal/rtc.h"
#include <stddef.h>
#include <stdio.h>
#include "field_manipulate.h"
#include "reg_rcc_type.h"
#include "reg_syscfg.h"
#include "rtc_param.h"
#include "rtc.h"
#include "reg_rtc.h"
#include "reg_rtc_type.h"

static RTC_HandleTypeDef RTC_Config;

static uint8_t BCD2DEC(uint8_t bcd)  
{  
    return (bcd-(bcd>>4)*6);
}
 
static uint8_t DEC2BCD(uint8_t dec)  
{  
    return (dec+(dec/10)*6);
}

static void RTC_SetConfig(uint8_t cksel)
{
    REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_RTCEN,0);
    REG_FIELD_WR(RTC_Config.Instance->WKUP,RTC_WKUP_WKSEL,0x1);
    REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_CKSEL,cksel);
}

static void HAL_RTC_Init(void)
{
    rtc_sw_reset(&RTC_Config);
    rtc_clock_enable(&RTC_Config,1);
    RTC_SetConfig(RTC_Config.rtc_cksel);
    RTC_status_set(&RTC_Config, 1);
}

/**
 * This function will initialize the on board CPU real time clock
 *
 *
 * @param[in]  rtc  rtc device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    if (NULL == rtc) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    RTC_Config.Instance = RTC;
    RTC_Config.rtc_cksel = CKSEL_LSI;
    HAL_RTC_Init();

    return 0;
}

/**
 * This function will return the value of time read from the on board CPU real time clock.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    if (NULL == rtc || NULL == time) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if(!REG_FIELD_RD(RTC_Config.Instance->STA,RTC_STA_EMPTY))
    {
        if(rtc->config.format == HAL_RTC_FORMAT_DEC)
        {
            time->year  = REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_YEAR_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_YEAR_U);
            time->month   = REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_MON_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_MON_U); 
            time->date  = REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_DATE_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_DATE_U);     
            time->weekday = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_WEEK);
            time->hr = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_HOUR_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_HOUR_U); 
            time->min  = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_MIN_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_MIN_U);     
            time->sec  = REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_SEC_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_SEC_U) + 1;
        }
        else if(rtc->config.format == HAL_RTC_FORMAT_BCD)
        {
            time->year  = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_YEAR_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_YEAR_U));
            time->month   = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_MON_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_MON_U)); 
            time->date  = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_DATE_T)*10 + REG_FIELD_RD(RTC_Config.Instance->CAL,RTC_CAL_DATE_U));     
            time->weekday = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_WEEK));
            time->hr = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_HOUR_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_HOUR_U)); 
            time->min  = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_MIN_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_MIN_U));     
            time->sec  = DEC2BCD(REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_SEC_T)*10 + REG_FIELD_RD(RTC_Config.Instance->TIME,RTC_TIME_SEC_U)) + 1;
        }
        else
        {
            printf("input param invalid in %s \r\n", __func__);
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

/**
 * This function will set MCU RTC time to a new value.
 *
 * @param[in]   rtc   rtc device
 * @param[in]   time  pointer to a time structure
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    if (NULL == rtc || NULL == time) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if(time->year < 0 || time->month < 0 || time->date < 0 || time->weekday < 0 || time->hr < 0 || time->min < 0 || time->sec < 0)
    {
        printf("invalid rtc set time param in %s \r\n", __func__);
        return -1;
    }

    if(rtc->config.format == HAL_RTC_FORMAT_BCD)
    {
        uint32_t year_u32, mon_u32, data_u32, week_u32, hour_u32, min_u32, sec_u32;
        year_u32 = BCD2DEC(time->year);
        mon_u32 = BCD2DEC(time->month);
        data_u32 = BCD2DEC(time->date);
        week_u32 = BCD2DEC(time->weekday);
        hour_u32 = BCD2DEC(time->hr);
        min_u32 = BCD2DEC(time->min);
        sec_u32 = BCD2DEC(time->sec);

        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_YEAR_T,(year_u32/10)%100);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_YEAR_U,(year_u32%100)%10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_MON_T,mon_u32/10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_MON_U,mon_u32%10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_DATE_T,data_u32/10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_DATE_U,data_u32%10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_WEEK,week_u32);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_HOUR_T,hour_u32/10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_HOUR_U,hour_u32%10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_MIN_T,min_u32/10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_MIN_U,min_u32%10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_SEC_T,sec_u32/10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_SEC_U,sec_u32%10);

        REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_RTCEN,1);
    }
    else if(rtc->config.format == HAL_RTC_FORMAT_DEC)
    {
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_YEAR_T,(time->year/10)%100);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_YEAR_U,(time->year%100)%10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_MON_T,time->month/10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_MON_U,time->month%10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_DATE_T,time->date/10);
        REG_FIELD_WR(RTC_Config.Instance->CAL,RTC_CAL_DATE_U,time->date%10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_WEEK,time->weekday);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_HOUR_T,time->hr/10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_HOUR_U,time->hr%10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_MIN_T,time->min/10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_MIN_U,time->min%10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_SEC_T,time->sec/10);
        REG_FIELD_WR(RTC_Config.Instance->TIME,RTC_TIME_SEC_U,time->sec%10);

        REG_FIELD_WR(RTC_Config.Instance->CTRL,RTC_CTRL_RTCEN,1);
    }
    else
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    return 0;
}

/**
 * De-initialises an RTC interface, Turns off an RTC hardware interface
 *
 * @param[in]  RTC  the interface which should be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
    if (NULL == rtc) 
    {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    rtc_clock_enable(&RTC_Config,0);
    RTC_status_set(&RTC_Config, 0);

    return 0;
}
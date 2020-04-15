/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_rtc_csi.c
 * @brief    CSI Source File for RTC Driver
 * @version  V1.0
 * @date     20. Nov 2018
 ******************************************************************************/

#include <csi_config.h>
#include <stdbool.h>
#include <string.h>
#include <drv/rtc.h>
#include <soc.h>
#include <silan_rtc_regs.h>
#include <silan_rtc.h>
#include <silan_syscfg.h>
#include <silan_pic.h>

#define ERR_RTC(errno) (CSI_DRV_ERRNO_RTC_BASE | errno)
#define RTC_BUSY_TIMEOUT        0x10000000
#define RTC_NULL_PARAM_CHK(para)    HANDLE_PARAM_CHK(para, ERR_RTC(DRV_ERROR_PARAMETER))

typedef struct {
    uint32_t base;
    uint32_t irq;
    uint64_t time_count;
    rtc_event_cb_t cb_event;
    struct tm rtc_base;
} silan_rtc_priv_t;

extern void mdelay(uint32_t ms);
extern int32_t target_get_rtc_count(void);
extern int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq);
extern void silan_rtcmem_ldo_onoff(uint8_t onoff);

static silan_rtc_priv_t rtc_instance[CONFIG_RTC_NUM];
static uint8_t leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint8_t noleap_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const rtc_capabilities_t rtc_capabilities = {
    .interrupt_mode = 1,    /* supports Interrupt mode */
    .wrap_mode = 0          /* supports wrap mode */
};

static void silan_rtc_reg_reset(void)
{
    silan_rtc_reg_set(RTC_TIMER, 0);
    silan_rtc_reg_set(RTC_TMCON, 0);
    silan_rtc_reg_set(RTC_CLKOUT_CON, 0);
    silan_rtc_reg_set(RTC_WEEK_ALARM, 0x08);/* week alarm disable 0x08 */
    silan_rtc_reg_set(RTC_DAY_ALARM, 0x40); /* day alarm disable 0x40 */
    silan_rtc_reg_set(RTC_HOUR_ALARM, 0x40);/* hour alarm disable 0x40 */
    silan_rtc_reg_set(RTC_MIN_ALARM, 0x08); /* min alarm disable 0x80 */
    // silan_rtc_reg_set(RTC_YEARL, 0);
    // silan_rtc_reg_set(RTC_MON, 0);
    // silan_rtc_reg_set(RTC_WEEK, 0);
    // silan_rtc_reg_set(RTC_DAY, 0);
    // silan_rtc_reg_set(RTC_HOUR, 0);
    // silan_rtc_reg_set(RTC_MIN, 0);
    // silan_rtc_reg_set(RTC_SEC, 0);
    // silan_rtc_reg_set(RTC_YEARH, 0);

    silan_rtc_reg_set(RTC_CS0, 0);
    silan_rtc_reg_set(RTC_CS1, 0);
    silan_rtc_reg_set(RTC_LOADH, 0);
    silan_rtc_reg_set(RTC_LOADL, 0);
}

static unsigned char hex_to_bcd(unsigned char data)
{
    unsigned char temp;

    temp = (((data / 10) << 4) + (data % 10));
    return temp;
}

static unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;

    temp = ((data >> 4) * 10 + (data & 0x0f));
    return temp;
}

static inline int clock_isleapyear(int year)
{
    return (year % 400) ? ((year % 100) ? ((year % 4) ? 0 : 1) : 0) : 1;
}

static int check_tm_ok(struct tm *rtctime)
{
    int32_t leap = 1;

    if (rtctime->tm_year < 70 || rtctime->tm_year >= 200) {
        goto error_time;
    }

    leap = clock_isleapyear(rtctime->tm_year + 1900);

    if (rtctime->tm_sec < 0 || rtctime->tm_sec >= 60) {
        goto error_time;
    }

    if (rtctime->tm_min < 0 || rtctime->tm_min >= 60) {
        goto error_time;
    }

    if (rtctime->tm_hour < 0 || rtctime->tm_hour >= 24) {
        goto error_time;
    }

    if (rtctime->tm_mon < 0 || rtctime->tm_mon >= 12) {
        goto error_time;
    }

    if (leap) {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > leap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    } else {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > noleap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    }

    return 0;
error_time:
    return ERR_RTC(RTC_ERROR_TIME);

}

#define SEC_PER_MIN  ((time_t)60)
#define SEC_PER_HOUR ((time_t)60 * SEC_PER_MIN)
#define SEC_PER_DAY  ((time_t)24 * SEC_PER_HOUR)
static const uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static int clock_daysbeforemonth(int month, bool leapyear)
{
    int retval = g_noleap_daysbeforemonth[month];

    if (month >= 2 && leapyear) {
        retval++;
    }

    return retval;
}


static void clock_utc2calendar(time_t days, int *year, int *month, int *day)
{
    /* There is one leap year every four years, so we can get close with the
     * following:
     */
    int value   = days  / (4 * 365 + 1); /* Number of 4-years periods since the epoch */
    days   -= value * (4 * 365 + 1); /* Remaining days */
    value <<= 2;                     /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    bool leapyear;
    int  tmp;

    for (; ;) {
        /* Is this year a leap year (we'll need this later too) */
        leapyear = clock_isleapyear(value + 1970);

        /* Get the number of days in the year */
        tmp = (leapyear ? 366 : 365);

        /* Do we have that many days? */
        if (days >= tmp) {
            /* Yes.. bump up the year */
            value++;
            days -= tmp;
        } else {
            /* Nope... then go handle months */
            break;
        }
    }

    /* At this point, value has the year and days has number days into this year */
    *year = 1970 + value;

    /* Handle the month (zero based) */
    int  min = 0;
    int  max = 11;

    do {
        /* Get the midpoint */
        value = (min + max) >> 1;

        /* Get the number of days that occurred before the beginning of the month
         * following the midpoint.
         */
        tmp = clock_daysbeforemonth(value + 1, leapyear);

        /* Does the number of days before this month that equal or exceed the
         * number of days we have remaining?
         */
        if (tmp > days) {
            /* Yes.. then the month we want is somewhere from 'min' and to the
             * midpoint, 'value'.  Could it be the midpoint?
             */
            tmp = clock_daysbeforemonth(value, leapyear);
            if (tmp > days) {
                /* No... The one we want is somewhere between min and value-1 */
                max = value - 1;
            } else {
                /* Yes.. 'value' contains the month that we want */
                break;
            }
        } else {
            /* No... The one we want is somwhere between value+1 and max */
            min = value + 1;
        }

        /* If we break out of the loop because min == max, then we want value
         * to be equal to min == max.
         */
        value = min;
    } while (min < max);

    /* The selected month number is in value. Subtract the number of days in the
     * selected month
     */
    days -= clock_daysbeforemonth(value, leapyear);

    /* At this point, value has the month into this year (zero based) and days has
     * number of days into this month (zero based)
     */
    *month = value + 1; /* 1-based */
    *day   = days + 1;  /* 1-based */
}

static uint32_t get_week_by_date(const struct tm *rtctime)
{
    /* week:                 tm_wday:
     * 0 is Monday           0 is Sunday
     * 1 is Tuesday          1 is Monday
     * 2 is Wednesday        2 is Tuesday
     * 3 is Thursday         3 is Wednesday
     * 4 is Friday           4 is Thursday
     * 5 is Saturday         5 is Friday
     * 6 is Sunday           6 is Saturday
     */
    uint32_t year, mon, day, week;
    year = rtctime->tm_year + 1900;
    mon = rtctime->tm_mon + 1;
    day = rtctime->tm_mday;
    week = (day + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - \
            year / 100 + year / 400) % 7;

    if (week == 6) {
        week = 0;
    } else {
        week++;
    }

    return week;
}

struct tm *gmtime_r(const time_t *timer, struct tm *result)
{
    time_t epoch;
    time_t jdn;
    int    year;
    int    month;
    int    day;
    int    hour;
    int    min;
    int    sec;

    /* Get the seconds since the EPOCH */
    epoch = *timer;

    /* Convert to days, hours, minutes, and seconds since the EPOCH */
    jdn    = epoch / SEC_PER_DAY;
    epoch -= SEC_PER_DAY * jdn;

    hour   = epoch / SEC_PER_HOUR;
    epoch -= SEC_PER_HOUR * hour;

    min    = epoch / SEC_PER_MIN;
    epoch -= SEC_PER_MIN * min;

    sec    = epoch;

    /* Convert the days since the EPOCH to calendar day */
    clock_utc2calendar(jdn, &year, &month, &day);

    /* Then return the struct tm contents */
    result->tm_year  = (int)year - 1900; /* Relative to 1900 */
    result->tm_mon   = (int)month - 1;   /* zero-based */
    result->tm_mday  = (int)day;         /* one-based */
    result->tm_hour  = (int)hour;
    result->tm_min   = (int)min;
    result->tm_sec   = (int)sec;
    result->tm_wday  = get_week_by_date(result);
    return result;
}


static int32_t rtc_reg_calendar_set(const struct tm *time)
{
    RTC_NULL_PARAM_CHK(time);
    if (check_tm_ok((struct tm *)time) < 0) {
        return ERR_RTC(RTC_ERROR_TIME);;
    }
    silan_rtc_reg_set(RTC_SEC, hex_to_bcd(time->tm_sec));
    silan_rtc_reg_set(RTC_MIN, hex_to_bcd(time->tm_min));
    silan_rtc_reg_set(RTC_HOUR, hex_to_bcd(time->tm_hour));
    silan_rtc_reg_set(RTC_DAY, hex_to_bcd(time->tm_mday));
    silan_rtc_reg_set(RTC_WEEK, hex_to_bcd(time->tm_wday));
    silan_rtc_reg_set(RTC_MON, hex_to_bcd(time->tm_mon + 1));
    silan_rtc_reg_set(RTC_YEARL, hex_to_bcd(time->tm_year % 100));
    silan_rtc_reg_set(RTC_YEARH, hex_to_bcd((time->tm_year + 1900) / 100));
    return 0;
}

static void rtc_reg_calendar_get(struct tm *time)
{
    RTC_NULL_PARAM_CHK(time);

    time->tm_sec  = bcd_to_hex(silan_rtc_reg_get(RTC_SEC));
    time->tm_min  = bcd_to_hex(silan_rtc_reg_get(RTC_MIN));
    time->tm_hour = bcd_to_hex(silan_rtc_reg_get(RTC_HOUR));
    time->tm_mday = bcd_to_hex(silan_rtc_reg_get(RTC_DAY));
    time->tm_wday = bcd_to_hex(silan_rtc_reg_get(RTC_WEEK));
    time->tm_mon  = bcd_to_hex(silan_rtc_reg_get(RTC_MON) & 0x1f) - 1;
    time->tm_year = (bcd_to_hex(silan_rtc_reg_get(RTC_YEARL)) + 
                     bcd_to_hex(silan_rtc_reg_get(RTC_YEARH)) * 100) - 1900;
}

static int check_initial_calendar(void)
{
    struct tm time;
    rtc_reg_calendar_get(&time);
    if (check_tm_ok(&time) < 0) {
        return ERR_RTC(RTC_ERROR_TIME);;
    }
    return 0;
}

void silan_rtc_irqhandler(int32_t idx)
{
    silan_rtc_priv_t *rtc_priv = &rtc_instance[idx];

    if (silan_rtc_reg_get(RTC_CS0) & RTC_CS0_TF) {
        silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TF)));
    }

    if (silan_rtc_reg_get(RTC_CS0) & RTC_CS0_AF) {
        silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AF)));

        if (rtc_priv->cb_event) {
            rtc_priv->cb_event(idx, RTC_EVENT_TIMER_INTRERRUPT);
        }
    }
}

/**
  \brief       Initialize RTC Interface. 1. Initializes the resources needed for the RTC interface 2.registers event callback function
  \param[in]   idx  rtc index
  \param[in]   cb_event  Pointer to \ref rtc_event_cb_t
  \return      pointer to rtc instance
*/
rtc_handle_t csi_rtc_initialize(int32_t idx, rtc_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_RTC_NUM) {
        return NULL;
    }

    int32_t real_idx;
    uint32_t base = 0u;
    uint32_t irq;

    real_idx = target_get_rtc(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    silan_rtc_priv_t *rtc_priv;

    rtc_priv = &rtc_instance[idx];
    rtc_priv->base = base;
    rtc_priv->irq  = irq;
    rtc_priv->time_count = 0;

    rtc_priv->cb_event = cb_event;

    silan_vlsp_cclk_config(CLK_ON);
    silan_rtc_reg_reset();
    silan_rtcmem_ldo_onoff(0);

    if (check_initial_calendar() != 0) {
        /* initial error so set to fixed value 2001-1-1 12:0:0 */
        struct tm time;
        time.tm_year = 101;/* 2001 - 1900 */
        time.tm_mon  = 0;  /* 1 - 1 */
        time.tm_mday = 1;
        time.tm_wday = 1;
        time.tm_hour = 12;
        time.tm_min  = 0;
        time.tm_sec  = 0;
        rtc_reg_calendar_set(&time);
    }

    silan_rtc_reg_get(RTC_CLKOUT_CON);
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_STOP)));

    silan_rtc_reg_set(RTC_TMCON, silan_rtc_reg_get(RTC_TMCON) & (~RTC_TMCON_HYS));
    silan_rtc_reg_set(RTC_TMCON, silan_rtc_reg_get(RTC_TMCON) & (~RTC_TMCON_DLY));
    silan_rtc_reg_set(RTC_TMCON, (silan_rtc_reg_get(RTC_TMCON) & (~RTC_TMCON_TD)) | 0x1);
    /* silan_rtc_reg_set(RTC_TIMER, RTC_TIMER_LOAD_DEFUALT_VALUE); */

    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TF)));
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AF)));

    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TIE)));
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AIE)));

    /* calibrate rtc pulse conuter of second as 32763(0x7ffb) with work clock 32.768kHz. */
    silan_rtc_reg_set(RTC_LOADH, 0x7f);
    silan_rtc_reg_set(RTC_LOADL, 0xfb);

    silan_pic_request(PIC_IRQID_RTC, 0, (hdl_t)silan_rtc_irqhandler);

    return (rtc_handle_t)rtc_priv;
}

/**
  \brief       De-initialize RTC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
*/
int32_t csi_rtc_uninitialize(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    silan_rtc_priv_t *rtc_priv = handle;

    rtc_priv->cb_event = NULL;

    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) | RTC_CS0_STOP));

    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TF)));
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AF)));

    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TIE)));
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AIE)));

    return 0;
}

int32_t csi_rtc_power_control(rtc_handle_t handle, csi_power_stat_e state)
{
    RTC_NULL_PARAM_CHK(handle);
    return ERR_RTC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx  rtc index
  \return      \ref rtc_capabilities_t
*/
rtc_capabilities_t csi_rtc_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_RTC_NUM) {
        rtc_capabilities_t ret;
        memset(&ret, 0, sizeof(rtc_capabilities_t));
        return ret;
    }

    return rtc_capabilities;
}

/**
  \brief       Set RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime \ref struct tm
  \return      \ref execution_status
*/

int32_t csi_rtc_set_time(rtc_handle_t handle, const struct tm *rtctime)
{
    RTC_NULL_PARAM_CHK(handle);
    RTC_NULL_PARAM_CHK(rtctime);

    silan_rtc_priv_t *rtc_priv = handle;
    rtc_priv->rtc_base.tm_sec  = rtctime->tm_sec;
    rtc_priv->rtc_base.tm_min  = rtctime->tm_min;
    rtc_priv->rtc_base.tm_hour = rtctime->tm_hour;
    rtc_priv->rtc_base.tm_mday = rtctime->tm_mday;
    rtc_priv->rtc_base.tm_wday = rtctime->tm_wday;
    rtc_priv->rtc_base.tm_mon  = rtctime->tm_mon;
    rtc_priv->rtc_base.tm_year = rtctime->tm_year;

    return rtc_reg_calendar_set(rtctime);
}

/**
  \brief       Get RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime \ref struct tm
  \return      \ref execution_status
*/
int32_t csi_rtc_get_time(rtc_handle_t handle, struct tm *rtctime)
{
    RTC_NULL_PARAM_CHK(handle);
    rtc_reg_calendar_get(rtctime);

    return 0;
}

/**
  \brief       Start RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
*/
int32_t csi_rtc_start(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    /* silan_rtc_reg_set(RTC_CS0, silan_rtc_reg_get(RTC_CS0) | RTC_CS0_TIE); */
    silan_rtc_reg_set(RTC_CS0, silan_rtc_reg_get(RTC_CS0) | RTC_CS0_AIE);
    silan_rtc_reg_set(RTC_TMCON, silan_rtc_reg_get(RTC_TMCON) | RTC_TMCON_TE);
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AF)));
    return 0;
}

/**
  \brief       Stop RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
*/
int32_t csi_rtc_stop(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    /* silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TIE))); */
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AIE)));
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_TF)));
    silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AF)));

    silan_rtc_reg_set(RTC_TMCON, silan_rtc_reg_get(RTC_TMCON) & (~RTC_TMCON_TE));

    return 0;
}

/**
  \brief       Get RTC status.
  \param[in]   handle rtc handle to operate.
  \return      RTC status \ref rtc_status_t
*/
rtc_status_t csi_rtc_get_status(rtc_handle_t handle)
{
    rtc_status_t rtc_status = {0};

    if (handle == NULL) {
        return rtc_status;
    }

    if ((silan_rtc_reg_get(RTC_CS0) & RTC_CS0_TF) || (silan_rtc_reg_get(RTC_CS0) & RTC_CS0_AF)) {
        rtc_status.active = 1;
    }

    return rtc_status;
}

/**
  \brief       config RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime time to wake up
  \return      error code
*/
int32_t csi_rtc_set_alarm(rtc_handle_t handle, const struct tm *rtctime)
{
    RTC_NULL_PARAM_CHK(handle);
    RTC_NULL_PARAM_CHK(rtctime);

    if (rtctime->tm_wday > 6)
       return ERR_RTC(RTC_ERROR_TIME);
    if (rtctime->tm_hour < 0 || rtctime->tm_hour >= 24)
        return ERR_RTC(RTC_ERROR_TIME);
    if (rtctime->tm_min < 0 || rtctime->tm_min >= 60)
        return ERR_RTC(RTC_ERROR_TIME);

    silan_rtc_reg_set(RTC_MIN_ALARM, (silan_rtc_reg_get(RTC_MIN_ALARM) & (~0x7f)) | (hex_to_bcd(rtctime->tm_min) & (~RTC_WEEK_ALARM_MAE)));
    silan_rtc_reg_set(RTC_HOUR_ALARM, (silan_rtc_reg_get(RTC_HOUR_ALARM) & (~0x7f)) | (hex_to_bcd(rtctime->tm_hour) & (~RTC_HOUR_ALARM_HAE)));
    // silan_rtc_reg_set(RTC_DAY_ALARM, (silan_rtc_reg_get(RTC_DAY_ALARM) & 0x3f) | hex_to_bcd(rtctime->tm_mday));
    if (rtctime->tm_wday < 0) {
        /* disable week alarm */
        silan_rtc_reg_set(RTC_WEEK_ALARM, (silan_rtc_reg_get(RTC_WEEK_ALARM) & (~0xF)) | hex_to_bcd(0) | RTC_WEEK_ALARM_WAE);
    } else {
        silan_rtc_reg_set(RTC_WEEK_ALARM, (silan_rtc_reg_get(RTC_WEEK_ALARM) & (~0xF)) | (hex_to_bcd(rtctime->tm_wday) & (~RTC_WEEK_ALARM_WAE)));
    }
    return 0;
}

/**
  \brief       disable or enable RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   flag  1 - enable rtc alarm 0 - disable rtc alarm
  \return      error code
*/
int32_t csi_rtc_enable_alarm(rtc_handle_t handle, uint8_t en)
{
    RTC_NULL_PARAM_CHK(handle);

    if (en == 1) {
        silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) | RTC_CS0_AIE));
    } else if (en == 0) {
        silan_rtc_reg_set(RTC_CS0, (silan_rtc_reg_get(RTC_CS0) & (~RTC_CS0_AIE)));
    } else {
        return ERR_RTC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/* mktime */

static time_t clock_calendar2utc(int year, int month, int day)
{
    time_t days;

    /* Years since epoch in units of days (ignoring leap years). */

    days = (year - 1970) * 365;

    /* Add in the extra days for the leap years prior to the current year. */

    days += (year - 1969) >> 2;

    /* Add in the days up to the beginning of this month. */

    days += (time_t)clock_daysbeforemonth(month, clock_isleapyear(year));

    /* Add in the days since the beginning of this month (days are 1-based). */

    days += day - 1;

    /* Then convert the seconds and add in hours, minutes, and seconds */

    return days;
}


static int ck_check_tm_ok(struct tm *rtctime)
{
    int32_t leap = 1;

    if (rtctime->tm_year < 70 || rtctime->tm_year >= 200) {
        goto error_time;
    }

    leap = clock_isleapyear(rtctime->tm_year + 1900);

    if (rtctime->tm_sec < 0 || rtctime->tm_sec >= 60) {
        goto error_time;
    }

    if (rtctime->tm_min < 0 || rtctime->tm_min >= 60) {
        goto error_time;
    }

    if (rtctime->tm_hour < 0 || rtctime->tm_hour >= 24) {
        goto error_time;
    }

    if (rtctime->tm_mon < 0 || rtctime->tm_mon >= 12) {
        goto error_time;
    }

    if (leap) {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > leap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    } else {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > noleap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    }

    return 0;
error_time:
    return ERR_RTC(RTC_ERROR_TIME);

}

time_t mktime(struct tm *tp)
{
    time_t ret;
    time_t jdn;

    /* Get the EPOCH-relative julian date from the calendar year,
     * month, and date
     */

    ret = ck_check_tm_ok(tp);

    if (ret < 0) {
        return -1;
    }

    jdn = clock_calendar2utc(tp->tm_year + 1900, tp->tm_mon, tp->tm_mday);

    /* Return the seconds into the julian day. */
    ret = ((jdn * 24 + tp->tm_hour) * 60 + tp->tm_min) * 60 + tp->tm_sec;

    return ret;
}


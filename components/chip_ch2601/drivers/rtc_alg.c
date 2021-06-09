/*
 *    Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file     rtc_alg.c
 * @brief    source file for rtc driver
 * @version  V1.0
 * @date     22. June 2020
 * ******************************************************/

#include "rtc_alg.h"

static int32_t   year_to_secs = 0;               ///< cache total sec of year
static const int8_t  leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int8_t  noleap_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int  noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};


int32_t clock_update_basetime(const struct tm *rtctime)
{
    int32_t total_sec;
    struct tm set_date;

    set_date.tm_sec   = rtctime->tm_sec;
    set_date.tm_min   = rtctime->tm_min;
    set_date.tm_hour  = rtctime->tm_hour;
    set_date.tm_mday  = rtctime->tm_mday;
    set_date.tm_mon   = rtctime->tm_mon;
    set_date.tm_isdst = WJ_RTC_PARA_UNSUPPORT;
    set_date.tm_wday  = WJ_RTC_PARA_UNSUPPORT;
    set_date.tm_yday  = WJ_RTC_PARA_UNSUPPORT;
    year_to_secs = 0;

    if (WJ_RTC_IS_LEAPYEAR(rtctime->tm_year + RTC_TIME_BASE_YEAR)) {
        set_date.tm_year  = RTC_TIME_TEMP_LEAP_YEAR;
    } else {
        set_date.tm_year  = RTC_TIME_TEMP_NONLEAP_YEAR;
    }

    total_sec = mktime(&set_date);                         ///< base date(not include year)

    set_date.tm_year  = rtctime->tm_year;
    year_to_secs = mktime(&set_date) - total_sec;          ///< save year in csi driver memory

    return total_sec;
}

int32_t clock_check_tm_ok(const struct tm *rtctime)
{
    int32_t ret = 0;

    do {

        /**
         * First check whether the regular date is legal
        */
        if ((rtctime->tm_sec  > RTC_TIME_MAX_VAL_SEC)  ||  \
            (rtctime->tm_min  > RTC_TIME_MAX_VAL_MIN)  ||  \
            (rtctime->tm_hour > RTC_TIME_MAX_VAL_HOUR) ||  \
            (rtctime->tm_mon  > RTC_TIME_MAX_VAL_MON)  ||  \
            ((rtctime->tm_year > RTC_TIME_MAX_VAL_YEAR)  || (rtctime->tm_year < RTC_TIME_MIN_VAL_YEAR))) {
            ret = -1;
            break;
        }

        /**
         * Second check whether the day less than the minimum
        */
        if (rtctime->tm_mday < RTC_TIME_MIN_VAL_DAY) {
            ret = -1;
            break;
        }

        /**
         * Third check whether the day more than the maximum(based on whether the year is a leap year or not)
        */
        if (WJ_RTC_IS_LEAPYEAR(rtctime->tm_year + RTC_TIME_BASE_YEAR)) {
            if (rtctime->tm_mday > leap_year[rtctime->tm_mon]) {
                ret = -1;
            }
        } else {
            if (rtctime->tm_mday > noleap_year[rtctime->tm_mon]) {
                ret = -1;
            }
        }

    } while (0);

    return ret;
}
static int get_week_by_date(const struct tm *rtctime)
{
    int year, mon, day, week;

    year = rtctime->tm_year + RTC_TIME_BASE_YEAR;
    mon  = rtctime->tm_mon + 1;
    day  = rtctime->tm_mday;

    if ((mon == 1) || (mon == 2)) {
        mon += 12;
        year --;
    }

    week = WJ_RTC_CAL_WEEK(year, mon, day) % 7 + 1;

    return week;
}
time_t mktime(struct tm *tp)
{
    time_t ret;
    time_t jdn;

    /* Get the EPOCH-relative julian date from the calendar year,
     * month, and date
     */

    jdn = clock_calendar2utc(tp->tm_year + RTC_TIME_BASE_YEAR, tp->tm_mon, tp->tm_mday);

    /* Return the seconds into the julian day. */

    ret = (((((jdn * 24) + tp->tm_hour) * 60) + tp->tm_min) * 60) + tp->tm_sec;

    ret -= (time_t)year_to_secs;

    return ret;
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
    epoch += (time_t)year_to_secs;          ///< add cache year

    /* Convert to days, hours, minutes, and seconds since the EPOCH */

    jdn    = epoch / WJ_RTC_SEC_PER_DAY;
    epoch -= WJ_RTC_SEC_PER_DAY * jdn;

    hour   = epoch / WJ_RTC_SEC_PER_HOUR;
    epoch -= WJ_RTC_SEC_PER_HOUR * hour;

    min    = epoch / WJ_RTC_SEC_PER_MIN;
    epoch -= WJ_RTC_SEC_PER_MIN * min;

    sec    = epoch;

    /* Convert the days since the EPOCH to calendar day */

    clock_utc2calendar(jdn, &year, &month, &day);


    /* Then return the struct tm contents */

    result->tm_year  = (int)year - RTC_TIME_BASE_YEAR;
    result->tm_mon   = (int)month;
    result->tm_mday  = (int)day;

    /* Calculate week by MM DD YY */
    result->tm_wday  = get_week_by_date((const struct tm *)result);
    result->tm_hour  = (int)hour;
    result->tm_min   = (int)min;
    result->tm_sec   = (int)sec;

    return result;
}


int clock_daysbeforemonth(int month, int leapyear)
{
    int retval = noleap_daysbeforemonth[month];

    if ((month >= 2) && leapyear) {
        retval++;
    }

    return retval;
}

time_t clock_calendar2utc(int year, int month, int day)
{
    time_t days;

    /* Years since epoch in units of days (ignoring leap years). */

    days = (year - (RTC_TIME_BASE_YEAR + RTC_TIME_MIN_VAL_YEAR)) * 365;

    /* Add in the extra days for the leap years prior to the current year. */

    days += (year - ((RTC_TIME_BASE_YEAR + RTC_TIME_MIN_VAL_YEAR) - 1)) >> 2;

    /* Add in the days up to the beginning of this month. */

    days += (time_t)clock_daysbeforemonth(month, WJ_RTC_IS_LEAPYEAR(year));

    /* Add in the days since the beginning of this month (days are 1-based). */

    days += day - RTC_TIME_MIN_VAL_DAY;

    /* Then convert the seconds and add in hours, minutes, and seconds */

    return days;
}

void clock_utc2calendar(time_t days, int *year, int *month, int *day)
{

    /* There is one leap year every four years, so we can get close with the
     * following:
     */

    int value   = days  / ((4 * 365) + 1); /* Number of 4-years periods since the epoch */

    days   -= value * ((4 * 365) + 1); /* Remaining days */
    value <<= 2;                     /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    int leapyear;
    int  tmp;

    for (;;) {
        /* Is this year a leap year (we'll need this later too) */

        leapyear = WJ_RTC_IS_LEAPYEAR(value + (RTC_TIME_BASE_YEAR + RTC_TIME_MIN_VAL_YEAR));

        /* Get the number of days in the year */

        tmp = (leapyear ? (int)366 : (int)365);

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

    *year = (RTC_TIME_BASE_YEAR + RTC_TIME_MIN_VAL_YEAR) + value;

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

    *month = value;                         /* 1-based */
    *day   = days + RTC_TIME_MIN_VAL_DAY;   /* 1-based */
}

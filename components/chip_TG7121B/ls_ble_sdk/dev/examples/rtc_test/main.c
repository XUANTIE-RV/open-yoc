#include "rtc.h"
#include "lsgpio.h"
#include "le501x.h"
#include "rtc_param.h"
#include "platform.h"
#include "io_config.h"
#include <string.h>
#include "log.h"

_calendar_time calendar_time;
_calendar_cal calendar_cal;

static void rtc_test()
{
    calendar_cal.year = 24;
    calendar_cal.mon = 2;
    calendar_cal.date = 28;
    calendar_time.hour = 23;
    calendar_time.min = 59;
    calendar_time.sec = 55;
    calendar_time.week = 7;
    RTC_CalendarSet(&calendar_cal,&calendar_time);
} 

int main()
{
    sys_init_app();
    // io_init();
    // io_cfg_output(PB08);
    // rco_calib_mode_set(0);
    // rco_calibration_start();
    RTC_Init();
    rtc_test();
    while(1)
    {
        RTC_CalendarGet(&calendar_cal,&calendar_time);
        LOG_I("%d : %d : %d  %d/%d/%d week = %d",calendar_time.hour,calendar_time.min,calendar_time.sec,calendar_cal.year,calendar_cal.mon,calendar_cal.date,calendar_time.week);
        DELAY_US(1000000);
    }

}



/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>

#include <soc.h>
#include <aos/hal/rtc.h>
#include <yoc/atserver.h>

#define TAG "TEST_HAL_RTC"

/* define dev */
rtc_dev_t rtc1,rtc2;
rtc_time_t   time_buf1,time_buf2,time_buf3,time_buf4,time_buf5;

static void aos_hal_rtc_DEC_max(){
    int32_t ret1 = -1;

    rtc1.port = 0;
    /* set to DEC format */
    rtc1.config.format = HAL_RTC_FORMAT_DEC;
    ret1 = hal_rtc_init(&rtc1);

    int32_t ret2 = -1;
    time_buf1.sec     = 59;
    time_buf1.min     = 59;
    time_buf1.hr      = 23;
    time_buf1.date    = 31;
    time_buf1.month   = 12;
    //年份上限，目前驱动支持到2037年
    time_buf1.year    = 37;
    ret2 = hal_rtc_set_time(&rtc1, &time_buf1);
    LOGI(TAG, "set time sec     = %d\n", time_buf1.sec);
    LOGI(TAG, "set time min     = %d\n", time_buf1.min);
    LOGI(TAG, "set time hr      = %d\n", time_buf1.hr);
    LOGI(TAG, "set time date    = %d\n", time_buf1.date);
    LOGI(TAG, "set time month   = %d\n", time_buf1.month);
    LOGI(TAG, "set time year    = %d\n", time_buf1.year + 2000);

    memset(&time_buf1, 0, sizeof(rtc_time_t));
    aos_msleep(10000);

    int32_t ret3 = -1;
    int32_t ret3_result = -1;
    ret3 = hal_rtc_get_time(&rtc1, &time_buf1);
    LOGI(TAG, "get time sec     = %d\n", time_buf1.sec);
    LOGI(TAG, "get time min     = %d\n", time_buf1.min);
    LOGI(TAG, "get time hr      = %d\n", time_buf1.hr);
    LOGI(TAG, "get time weekday = %d\n", time_buf1.weekday);
    LOGI(TAG, "get time date    = %d\n", time_buf1.date);
    LOGI(TAG, "get time month   = %d\n", time_buf1.month);
    LOGI(TAG, "get time year    = %d\n", time_buf1.year + 2000);
    if (time_buf1.sec == 9 && time_buf1.min == 0 && time_buf1.hr == 0 && time_buf1.date == 1 && time_buf1.month == 1 && time_buf1.year == 38) {
            ret3_result = 0;
    }

    int32_t ret4 = -1;
    ret4 = hal_rtc_finalize(&rtc1);
    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret3_result == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_rtc_DEC_normal(){
    int32_t ret1 = -1;

    rtc1.port = 0;
    /* set to DEC format */
    rtc1.config.format = HAL_RTC_FORMAT_DEC;
    hal_rtc_init(&rtc1);

    time_buf2.sec     = 20;
    time_buf2.min     = 20;
    time_buf2.hr      = 20;
    time_buf2.date    = 20;
    time_buf2.month   = 10;
    time_buf2.year    = 20;
    ret1 = hal_rtc_set_time(&rtc1, &time_buf2);
    LOGI(TAG, "set time sec     = %d\n", time_buf2.sec);
    LOGI(TAG, "set time min     = %d\n", time_buf2.min);
    LOGI(TAG, "set time hr      = %d\n", time_buf2.hr);
    LOGI(TAG, "set time date    = %d\n", time_buf2.date);
    LOGI(TAG, "set time month   = %d\n", time_buf2.month);
    LOGI(TAG, "set time year    = %d\n", time_buf2.year + 2000);

    memset(&time_buf2, 0, sizeof(rtc_time_t));
    aos_msleep(20000);

    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    ret2 = hal_rtc_get_time(&rtc1, &time_buf2);
    LOGI(TAG, "get time sec     = %d\n", time_buf2.sec);
    LOGI(TAG, "get time min     = %d\n", time_buf2.min);
    LOGI(TAG, "get time hr      = %d\n", time_buf2.hr);
    LOGI(TAG, "get time weekday = %d\n", time_buf2.weekday);
    LOGI(TAG, "get time date    = %d\n", time_buf2.date);
    LOGI(TAG, "get time month   = %d\n", time_buf2.month);
    LOGI(TAG, "get time year    = %d\n", time_buf2.year + 2000);
    if (time_buf2.sec == 40 && time_buf2.min == 20 && time_buf2.hr == 20 && time_buf2.date == 20 && time_buf2.month == 10 && time_buf2.year == 20) {
            ret2_result = 0;
    }

    hal_rtc_finalize(&rtc1);
    if (ret1 == 0 && ret2 == 0 && ret2_result == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_rtc_DEC_min(){
    int32_t ret1 = -1;

    rtc1.port = 0;
    /* set to DEC format */
    rtc1.config.format = HAL_RTC_FORMAT_DEC;
    hal_rtc_init(&rtc1);

    time_buf5.sec     = 0;
    time_buf5.min     = 0;
    time_buf5.hr      = 0;
    time_buf5.date    = 1;
    time_buf5.month   = 1;
    time_buf5.year    = 0;
    ret1 = hal_rtc_set_time(&rtc1, &time_buf5);
    LOGI(TAG, "set time sec     = %d\n", time_buf5.sec);
    LOGI(TAG, "set time min     = %d\n", time_buf5.min);
    LOGI(TAG, "set time hr      = %d\n", time_buf5.hr);
    LOGI(TAG, "set time date    = %d\n", time_buf5.date);
    LOGI(TAG, "set time month   = %d\n", time_buf5.month);
    LOGI(TAG, "set time year    = %d\n", time_buf5.year + 2000);

    memset(&time_buf5, 0, sizeof(rtc_time_t));
    aos_msleep(30000);

    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    ret2 = hal_rtc_get_time(&rtc1, &time_buf5);
    LOGI(TAG, "get time sec     = %d\n", time_buf5.sec);
    LOGI(TAG, "get time min     = %d\n", time_buf5.min);
    LOGI(TAG, "get time hr      = %d\n", time_buf5.hr);
    LOGI(TAG, "get time weekday = %d\n", time_buf5.weekday);
    LOGI(TAG, "get time date    = %d\n", time_buf5.date);
    LOGI(TAG, "get time month   = %d\n", time_buf5.month);
    LOGI(TAG, "get time year    = %d\n", time_buf5.year + 2000);
    if (time_buf5.sec == 30 && time_buf5.min == 0 && time_buf5.hr == 0 && time_buf5.date == 1 && time_buf5.month == 1 && time_buf5.year == 0) {
            ret2_result = 0;
    }

    hal_rtc_finalize(&rtc1);
    if (ret1 == 0 && ret2 == 0 && ret2_result == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_rtc_BCD_max(){
    int32_t ret1 = -1;

    rtc2.port = 0;
    /* set to BCD format */
    rtc2.config.format = HAL_RTC_FORMAT_BCD;
    ret1 = hal_rtc_init(&rtc2);

    int32_t ret2 = -1;
    /* set time */
    time_buf3.sec     = 0x59;
    time_buf3.min     = 0x59;
    time_buf3.hr      = 0x23;
    time_buf3.date    = 0x31;
    time_buf3.month   = 0x12;
    time_buf3.year    = 0x25;
    ret2 = hal_rtc_set_time(&rtc2, &time_buf3);
    LOGI(TAG, "set time sec     = %x\n", time_buf3.sec);
    LOGI(TAG, "set time min     = %x\n", time_buf3.min);
    LOGI(TAG, "set time hr      = %x\n", time_buf3.hr);
    LOGI(TAG, "set time date    = %x\n", time_buf3.date);
    LOGI(TAG, "set time month   = %x\n", time_buf3.month);
    LOGI(TAG, "set time year    = %x\n", time_buf3.year + 0x2000);


    memset(&time_buf3, 0, sizeof(rtc_time_t));
    aos_msleep(10000);

    int32_t ret3 = -1;
    int32_t ret3_result = -1;
    ret3 = hal_rtc_get_time(&rtc2, &time_buf3);
    LOGI(TAG, "get time sec     = %x\n", time_buf3.sec);
    LOGI(TAG, "get time min     = %x\n", time_buf3.min);
    LOGI(TAG, "get time hr      = %x\n", time_buf3.hr);
    LOGI(TAG, "get time weekday = %x\n", time_buf3.weekday);
    LOGI(TAG, "get time date    = %x\n", time_buf3.date);
    LOGI(TAG, "get time month   = %x\n", time_buf3.month);
    LOGI(TAG, "get time year    = %x\n", time_buf3.year + 0x2000);
    if (time_buf3.sec == 0x09 && time_buf3.min == 0x00 && time_buf3.hr == 0x00 && time_buf3.date == 0x01 && time_buf3.month == 0x01 && time_buf3.year == 0x26) {
            ret3_result = 0;
    }

    int32_t ret4 = -1;
    ret4 = hal_rtc_finalize(&rtc2);

    if (ret1 == 0 && ret2 == 0 && ret3 == 0 && ret3_result == 0 && ret4 == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_rtc_BCD_normal(){
    int32_t ret1 = -1;

    rtc2.port = 0;
    /* set to BCD format */
    rtc2.config.format = HAL_RTC_FORMAT_BCD;
    hal_rtc_init(&rtc2);

    /* set time */
    time_buf4.sec     = 0x20;
    time_buf4.min     = 0x20;
    time_buf4.hr      = 0x20;
    time_buf4.date    = 0x20;
    time_buf4.month   = 0x02;
    time_buf4.year    = 0x20;
    ret1 = hal_rtc_set_time(&rtc2, &time_buf4);
    LOGI(TAG, "set time sec     = %x\n", time_buf4.sec);
    LOGI(TAG, "set time min     = %x\n", time_buf4.min);
    LOGI(TAG, "set time hr      = %x\n", time_buf4.hr);
    LOGI(TAG, "set time date    = %x\n", time_buf4.date);
    LOGI(TAG, "set time month   = %x\n", time_buf4.month);
    LOGI(TAG, "set time year    = %x\n", time_buf4.year + 0x2000);

    memset(&time_buf4, 0, sizeof(rtc_time_t));
    aos_msleep(10000);

    int32_t ret2 = -1;
    int32_t ret2_result = -1;
    ret2 = hal_rtc_get_time(&rtc2, &time_buf4);
    LOGI(TAG, "get time sec     = %x\n", time_buf4.sec);
    LOGI(TAG, "get time min     = %x\n", time_buf4.min);
    LOGI(TAG, "get time hr      = %x\n", time_buf4.hr);
    LOGI(TAG, "get time weekday = %x\n", time_buf4.weekday);
    LOGI(TAG, "get time date    = %x\n", time_buf4.date);
    LOGI(TAG, "get time month   = %x\n", time_buf4.month);
    LOGI(TAG, "get time year    = %x\n", time_buf4.year + 0x2000);
    if (time_buf4.sec == 0x40 && time_buf4.min == 0x20 && time_buf4.hr == 0x20 && time_buf4.date == 0x20 && time_buf4.month == 0x02 && time_buf4.year == 0x20) {
            ret2_result = 0;
    }

    hal_rtc_finalize(&rtc2);

    if (ret1 == 0 && ret2 == 0 && ret2_result == 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

static void aos_hal_rtc_errorcase(){
    int32_t ret1 = -1;
    ret1 = hal_rtc_init(NULL);

    int32_t ret2 = -1;
    /* set time */
    time_buf1.sec     = -1;
    time_buf1.min     = -1;
    time_buf1.hr      = -1;
    time_buf1.date    = -1;
    time_buf1.month   = -1;
    time_buf1.year    = -1;
    ret2 = hal_rtc_set_time(&rtc1, &time_buf1); 

    int32_t ret3 = -1;
    ret3 = hal_rtc_set_time(&rtc1, NULL);

    int32_t ret4 = -1;
    ret4 = hal_rtc_set_time(&rtc2, NULL);

    int32_t ret5 = -1;
    ret5 = hal_rtc_get_time(NULL, NULL);

    int32_t ret6 = -1;
    ret6 = hal_rtc_finalize(NULL);

    if (ret1 != 0 && ret2 != 0 && ret3 != 0 && ret4 != 0 && ret5 != 0 && ret6 != 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

void test_hal_rtc(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "'DEC_max'\0") == 0) {
        aos_hal_rtc_DEC_max();
    }else if (strcmp((const char *)data, "'DEC_normal'\0") == 0) {
        aos_hal_rtc_DEC_normal();
    }else if (strcmp((const char *)data, "'DEC_min'\0") == 0) {
        aos_hal_rtc_DEC_min();
    }else if (strcmp((const char *)data, "'BCD_max'\0") == 0) {
        aos_hal_rtc_BCD_max();
    }else if (strcmp((const char *)data, "'BCD_normal'\0") == 0) {
        aos_hal_rtc_BCD_normal();
    }else if (strcmp((const char *)data, "'error'\0") == 0) {
        aos_hal_rtc_errorcase();
    }
}
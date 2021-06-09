/*
* Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions an
* limitations under the License.
*/

#include "rtc_test.h"

int rtc_time_assert(struct tm *ret_time, struct tm *expect){

    TEST_CASE_TIPS("Got RTC  time: %d-%d-%d %d:%d:%d", ret_time->tm_year+1900, ret_time->tm_mon+1, ret_time->tm_mday, ret_time->tm_hour, ret_time->tm_min, ret_time->tm_sec);
    TEST_CASE_TIPS("Expected time: %d-%d-%d %d:%d",       expect->tm_year+1900,    expect->tm_mon+1, expect->tm_mday, expect->tm_hour, expect->tm_min);
    TEST_CASE_ASSERT(ret_time->tm_year == expect->tm_year, "wrong year!");
    TEST_CASE_ASSERT(ret_time->tm_mon == expect->tm_mon, "wrong mon!");
    TEST_CASE_ASSERT(ret_time->tm_mday == expect->tm_mday, "wrong mday!");
    TEST_CASE_ASSERT(ret_time->tm_hour == expect->tm_hour, "wrong hour!");
    TEST_CASE_ASSERT(ret_time->tm_min == expect->tm_min, "wrong min!");
    //TEST_CASE_ASSERT(ret_time->tm_sec == expect->tm_sec; "wrong sec!");
    return 0;
}

//calculate expect time after delay 2000ms
//test for end of every month...
int calc_expect_rtc(struct tm *test_time, struct tm *expect, int sec_delta){
    if (test_time->tm_mon == 11){
        expect->tm_year = test_time->tm_year + 1;
        expect->tm_mon = 0;
    }else{
        expect->tm_year = test_time->tm_year;
        expect->tm_mon = test_time->tm_mon + 1;
    }
    expect->tm_mday = 1;
    expect->tm_hour = 0;
    expect->tm_min = 0;
    expect->tm_sec = sec_delta-1;

    return 0;
}
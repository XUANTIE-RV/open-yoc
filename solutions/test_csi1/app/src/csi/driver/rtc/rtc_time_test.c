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


static void rtc_callback(int32_t idx, rtc_event_e event)
{
    TEST_CASE_TIPS("enter rtc_callback");
}

int test_rtc_time(char *args)
{
    test_rtc_args td;   
    TEST_CASE_READY();
    rtc_handle_t test_rtc_handler;
    rtc_handle_t * rtc_handler= &test_rtc_handler; 
    struct tm expect_time;
    int32_t rtc_ret;
    struct tm cur_time;
    time_t time_temp;


	/*args get*/
	td.idx = (uint32_t)*((uint32_t *)args);
	td.tm.tm_year = (uint32_t)*((uint32_t *)args+1);
	td.tm.tm_mon = (uint32_t)*((uint32_t *)args+2);
	td.tm.tm_mday = (uint32_t)*((uint32_t *)args+3);
	td.tm.tm_hour = (uint32_t)*((uint32_t *)args+4);
	td.tm.tm_min = (uint32_t)*((uint32_t *)args+5);
	td.tm.tm_sec = (uint32_t)*((uint32_t *)args+6);
	td.offset_time = (uint32_t)*((uint32_t *)args+7);  
    TEST_CASE_TIPS("set RTC  time: %d-%d-%d %d:%d:%d  offset time:%d", td.tm.tm_year+1900, td.tm.tm_mon+1, td.tm.tm_mday, td.tm.tm_hour, td.tm.tm_min, td.tm.tm_sec, td.offset_time);


    /*rtc init*/
    rtc_handler = csi_rtc_initialize(td.idx, rtc_callback);
    TEST_CASE_ASSERT_QUIT(rtc_handler != NULL,"rtc init failed, idx:%d", td.idx);

    rtc_ret = csi_rtc_start(rtc_handler);    
    TEST_CASE_ASSERT_QUIT(rtc_ret==0,"rtc start failed(wait), idx:%d", td.idx);
    
    time_temp = mktime(&td.tm);
    time_temp += td.offset_time;
    time_temp -= 28800;

    expect_time = *localtime(&time_temp);   
    TEST_CASE_TIPS("expect_time: %d-%d-%d %d:%d:%d", expect_time.tm_year+1900, expect_time.tm_mon+1, expect_time.tm_mday, expect_time.tm_hour, expect_time.tm_min, expect_time.tm_sec);

    rtc_ret = csi_rtc_set_time(rtc_handler, &td.tm);   
    TEST_CASE_ASSERT_QUIT(rtc_ret==0,"rtc set time failed(wait), idx:%d", td.idx);

    if (td.tm.tm_year <70 || td.tm.tm_year >199 || \
        td.tm.tm_mon <0 || td.tm.tm_mon >11     || \
        td.tm.tm_mday <1 || td.tm.tm_mday >31   || \
        td.tm.tm_hour <0 || td.tm.tm_hour >23   || \
        td.tm.tm_min <0 || td.tm.tm_min >59   || \
        td.tm.tm_sec <0 || td.tm.tm_sec >59
       ){
        TEST_CASE_ASSERT_QUIT(rtc_ret != 0,"passed an invalid RTC time, returned:%d", rtc_ret);
    }else{
        TEST_CASE_ASSERT(rtc_ret==0,"rtc set time failed(wait), idx:%d", td.idx);
        tst_mdelay(td.offset_time*1000);
        rtc_ret = csi_rtc_get_time(rtc_handler, &cur_time);
        rtc_time_assert(&cur_time, &expect_time);
    }
    TEST_CASE_TIPS("***");
    return 0;
}

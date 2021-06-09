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
static volatile uint8_t rtc_flag;

static void rtc_callback(int32_t idx, rtc_event_e event)
{
    rtc_flag = 1;
}


int test_rtc_alarm_func(rtc_handle_t *rtc_handler, test_rtc_args *test_args, int cancel_flag,time_t offset_time){
    int32_t rtc_ret;
    struct tm alarm_time;
    time_t time_temp;

    time_temp = mktime(&test_args->tm);
    time_temp += offset_time;
    time_temp -= 28800;

    alarm_time = *localtime(&time_temp);
    TEST_CASE_TIPS("set RTC  time: %d-%d-%d %d:%d:%d", test_args->tm.tm_year+1900, test_args->tm.tm_mon+1, test_args->tm.tm_mday, test_args->tm.tm_hour, test_args->tm.tm_min, test_args->tm.tm_sec);
    TEST_CASE_TIPS("alarm    time: %d-%d-%d %d:%d:%d", alarm_time.tm_year+1900, alarm_time.tm_mon+1, alarm_time.tm_mday, alarm_time.tm_hour, alarm_time.tm_min, alarm_time.tm_sec);
    
    rtc_handler = csi_rtc_initialize(test_args->idx, rtc_callback);
    TEST_CASE_ASSERT_QUIT(rtc_handler != NULL,"rtc init failed, idx:%d", test_args->idx);
    
    rtc_ret = csi_rtc_start(rtc_handler);    
    TEST_CASE_ASSERT_QUIT(rtc_ret==0,"rtc start failed(wait), idx:%d", test_args->idx);

    rtc_ret = csi_rtc_set_time(rtc_handler, &test_args->tm);   
    TEST_CASE_ASSERT_QUIT(rtc_ret==0,"rtc set time failed(wait), idx:%d", test_args->idx);

    rtc_flag = 0;
    rtc_ret = csi_rtc_set_alarm(rtc_handler, &alarm_time);
    TEST_CASE_ASSERT_QUIT(rtc_ret==0,"rtc set alarm failed, idx:%d", test_args->idx);

    rtc_ret = csi_rtc_enable_alarm(rtc_handler, 1);
    TEST_CASE_ASSERT_QUIT(rtc_ret==0,"rtc enable alarm failed, idx:%d", test_args->idx);

    tst_mdelay(offset_time*1000/3);

    if (cancel_flag==1){
        rtc_ret =  csi_rtc_enable_alarm(rtc_handler,0);
        TEST_CASE_TIPS("RTC Alarm canceled...");
        TEST_CASE_ASSERT(rtc_ret == 0, "csi_rtc_cancel_alarm failed. returned:%d ", rtc_ret);
    }
    tst_mdelay(offset_time*1000/2);
    TEST_CASE_ASSERT(rtc_flag == 0, "Alarm occured before expect time ");
    
    tst_mdelay(offset_time*1000+1000);
    if(cancel_flag == 0){
        TEST_CASE_ASSERT(rtc_flag == 1, "Alarm not occured as expected ");
    }else{
        TEST_CASE_ASSERT(rtc_flag == 0, "Alarm still occured after canceled alarm! ");
    }
    
    csi_rtc_uninitialize(rtc_handler);
    return 0;
}

int test_rtc_alarm_set(char *args)
{

    test_rtc_args td;   
    TEST_CASE_READY();
    rtc_handle_t test_rtc_handler;
    rtc_handle_t * rtc_handler= &test_rtc_handler; 


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

    //test for no alarm cancelling
    test_rtc_alarm_func(rtc_handler, &td, 0, td.offset_time); 

    //test for alarm cancelling
    test_rtc_alarm_func(rtc_handler, &td, 1, td.offset_time); 
    return 0;
}
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


void rtc_event_cb()
{
    TEST_CASE_TIPS("enter rtc cb");
}

int test_rtc_interface(char *args)
{
    int32_t ret;

    //get_capabilities
    rtc_capabilities_t cap = csi_rtc_get_capabilities(2);
    TEST_CASE_ASSERT(cap.interrupt_mode == 0,"interrupt_mode !=0  act_val=%d",cap.interrupt_mode);
    TEST_CASE_ASSERT(cap.wrap_mode == 0,"wrap_mode !=0 act_val=%d",cap.wrap_mode);

    rtc_handle_t rtc = csi_rtc_initialize(0, rtc_event_cb);
    TEST_CASE_ASSERT(rtc != NULL,"rtc != NULL act_val=%d",rtc);

    rtc = csi_rtc_initialize(0, NULL);
    TEST_CASE_ASSERT(rtc != NULL,"rtc == NULL act_val=%d",rtc);

    // power control
    ret = csi_rtc_power_control(NULL, DRV_POWER_FULL);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    
    ret = csi_rtc_power_control(rtc, 5);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

    //uninitialize
    ret = csi_rtc_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //start
    ret = csi_rtc_start(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    csi_rtc_start(rtc);

    struct tm set_time;
    set_time.tm_year   = 117;
    set_time.tm_mon    = 11;
    set_time.tm_mday   = 31;
    set_time.tm_hour   = 23;
    set_time.tm_min    = 59;
    set_time.tm_sec    = 59;
    //set_time
    ret = csi_rtc_set_time(NULL, &set_time);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_rtc_set_time(rtc, NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_rtc_set_time(rtc, &set_time);

    //get_time
    ret = csi_rtc_get_time(NULL, &set_time);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_rtc_get_time(rtc, NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //set_alarm
    ret = csi_rtc_set_alarm(NULL, &set_time);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_rtc_set_alarm(rtc, NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //enable_alarm
    ret = csi_rtc_enable_alarm(NULL, 0);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //get_status
    rtc_status_t stat = csi_rtc_get_status(NULL);
    TEST_CASE_ASSERT(stat.active == 0,"active != 0 act_val=%d",stat.active);

    //stop
    ret = csi_rtc_stop(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_RTC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    csi_rtc_stop(rtc);

    csi_rtc_uninitialize(rtc);

    return 0;
}
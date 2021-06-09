/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "timer_test.h"

static volatile uint32_t timeout_flag = 0;

static void test_timer_event_cb(int32_t idx, timer_event_e event)
{
    if (event == TIMER_EVENT_TIMEOUT) 
    {
        if (!timeout_flag)
        {
            timeout_flag = 1;
        }
        //TEST_CASE_TIPS("enter timer call back %d",timeout_flag);

    }
}


int test_timer_status(char *args)
{

    int32_t ret;
	uint32_t get_data[10];
    test_timer_args_t td;
    timer_handle_t timer;
    uint32_t value1,value2;
    timer_status_t status;

	/*args get*/
    ret = args_parsing(args, get_data, 1);
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");
    td.timer_idx = get_data[0];
    TEST_CASE_TIPS("test timer idx is %d", td.timer_idx);

    /*timer init*/
    timer = csi_timer_initialize(td.timer_idx, test_timer_event_cb);
    TEST_CASE_ASSERT(timer != NULL,"timer init failed");

    /*timer config mode*/
    ret = csi_timer_config(timer, TIMER_MODE_RELOAD);
    TEST_CASE_ASSERT(ret == 0,"timer config TIMER_MODE_RELOAD failed");

    /*timer set timeout*/
    ret = csi_timer_set_timeout(timer, 300000);
    TEST_CASE_ASSERT(ret == 0,"timer set timerout failed");
   
    /*timer start*/
    ret = csi_timer_start(timer);
    TEST_CASE_ASSERT(ret == 0,"timer start failed");

    /*read value and cmpare*/
    ret = csi_timer_get_current_value(timer, &value1);
    TEST_CASE_ASSERT(ret == 0,"get current value failed");
    tst_mdelay(100);
    ret = csi_timer_get_current_value(timer, &value2);
    TEST_CASE_ASSERT(ret == 0,"get current value failed"); 
    TEST_CASE_ASSERT(value1 != value2,"get current value failed");   
    status = csi_timer_get_status(timer);
    TEST_CASE_ASSERT(status.active == 1,"get current value failed"); 

    /*timer stop*/
    ret = csi_timer_stop(timer);
    TEST_CASE_ASSERT(ret == 0,"timer stop failed");

    /*read value and cmpare*/
    ret = csi_timer_get_current_value(timer, &value1);
    TEST_CASE_ASSERT(ret == 0,"get current value failed");
    tst_mdelay(100);
    ret = csi_timer_get_current_value(timer, &value2);
    TEST_CASE_ASSERT(ret == 0,"get current value failed"); 
    TEST_CASE_ASSERT(value1 == value2,"get current value failed");  
    status = csi_timer_get_status(timer);
    TEST_CASE_ASSERT(status.active == 0,"get current value failed"); 

    
    /*timer start*/
    ret = csi_timer_start(timer);
    TEST_CASE_ASSERT(ret == 0,"timer start failed");

    /*timer suspend*/
    ret = csi_timer_suspend(timer);
    if(ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_UNSUPPORTED))
    {
        ret = csi_timer_suspend(timer);
        TEST_CASE_ASSERT(ret == 0,"timer suspend failed"); 
        
        /*read value and cmpare*/
        ret = csi_timer_get_current_value(timer, &value1);
        TEST_CASE_ASSERT(ret == 0,"get current value failed");
        tst_mdelay(100);
        ret = csi_timer_get_current_value(timer, &value2);
        TEST_CASE_ASSERT(ret == 0,"get current value failed"); 
        TEST_CASE_ASSERT(value1 == value2,"get current value failed");  
        status = csi_timer_get_status(timer);
        TEST_CASE_ASSERT(status.active == 0,"get current value failed"); 

        /*timer resume*/
        ret = csi_timer_resume(timer);
        TEST_CASE_ASSERT(ret == 0,"timer suspend failed"); 

        /*read value and cmpare*/
        ret = csi_timer_get_current_value(timer, &value1);
        TEST_CASE_ASSERT(ret == 0,"get current value failed");
        tst_mdelay(100);
        ret = csi_timer_get_current_value(timer, &value2);
        TEST_CASE_ASSERT(ret == 0,"get current value failed"); 
        TEST_CASE_ASSERT(value1 != value2,"get current value failed");   
        status = csi_timer_get_status(timer);
        TEST_CASE_ASSERT(status.active == 1,"get current value failed"); 
    }

    /*uninit timer*/
    ret = csi_timer_stop(timer);   
    TEST_CASE_ASSERT(ret == 0,"timer stop failed");
    ret = csi_timer_uninitialize(timer);
    TEST_CASE_ASSERT(ret == 0,"timer uninit failed");
    return 0;  
}
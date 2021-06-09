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

int test_timer_mode(test_timer_args_t *ptd,timer_mode_e mode)
{
    int32_t ret; 
    timer_handle_t timer;
    uint32_t diff_time;   
    uint32_t cnt; 

    /*timer init*/
    timer = csi_timer_initialize(ptd->timer_idx, test_timer_event_cb);
    TEST_CASE_ASSERT(timer != NULL,"timer init failed");

    /*timer config mode*/
    ret = csi_timer_config(timer, mode);
    TEST_CASE_ASSERT(ret == 0,"timer config TIMER_MODE_RELOAD failed");

    /*timer set timeout*/
    ret = csi_timer_set_timeout(timer, ptd->timeout);
    TEST_CASE_ASSERT(ret == 0,"timer set timerout failed");
   
    /*timer start*/
    ret = csi_timer_start(timer);
    TEST_CASE_ASSERT(ret == 0,"timer start failed");

    /*test timer precesion*/
    cnt = 0;
    timeout_flag = 0;
    while (!timeout_flag){tst_mdelay(1); cnt++;}
    TEST_CASE_TIPS("cnt=%d", cnt);

    if(cnt*1000 >= ptd->timeout)
    {
        diff_time = cnt*1000 -  ptd->timeout;
    }
    else
    {
        diff_time = ptd->timeout -  cnt*1000;
    }

    TEST_CASE_TIPS("diff_time %d us", diff_time); 
    TEST_CASE_ASSERT(diff_time <= ptd->timeout*0.5, "timer precision error greater than 5%%");    
   
    /*uninit timer*/
    ret = csi_timer_stop(timer);   
    TEST_CASE_ASSERT(ret == 0,"timer stop failed");
    ret = csi_timer_uninitialize(timer);
    TEST_CASE_ASSERT(ret == 0,"timer uninit failed");
    return 0;
}


int test_timer_precision(char *args)
{

    int32_t ret;
	uint32_t get_data[10];
    test_timer_args_t td;
  
	/*args get*/
    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");
    td.timer_idx = get_data[0];
    td.timeout = get_data[1];
    TEST_CASE_TIPS("test timer idx is %d, timeout %d us", td.timer_idx, td.timeout);

    TEST_CASE_TIPS("test TIMER_MODE_RELOAD mode- - - - - - - - - - - - - -");
    test_timer_mode(&td,TIMER_MODE_RELOAD);
    TEST_CASE_TIPS("test TIMER_MODE_FREE_RUNNING mode- - - - - - - - - - -");
    test_timer_mode(&td,TIMER_MODE_FREE_RUNNING);
  
    return 0;
}
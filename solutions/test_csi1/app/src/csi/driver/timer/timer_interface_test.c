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


static void test_timer_event_cb(int32_t idx, timer_event_e event)
{
    if (event == TIMER_EVENT_TIMEOUT) 
    {
        TEST_CASE_TIPS("Timer%d !\n", idx);
    }
}

int test_timer_interface(char *args)
{
    //initialize
    timer_handle_t timer = csi_timer_initialize(5, test_timer_event_cb);
    TEST_CASE_ASSERT(timer == NULL,"timer != NULL act_val=%d",timer);

    timer = csi_timer_initialize(0, NULL);
    TEST_CASE_ASSERT(timer != NULL,"timer == NULL act_val=%d",timer);

    int32_t ret = 0;

    ret = csi_timer_power_control(NULL, DRV_POWER_FULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);


    ret = csi_timer_power_control(timer, 6);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

    //uninitialize
    ret = csi_timer_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //config
    ret = csi_timer_config(NULL, TIMER_MODE_RELOAD);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);


    ret = csi_timer_config(timer, 2);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_timer_config(timer, TIMER_MODE_RELOAD);

    //set_timeout
    ret = csi_timer_set_timeout(NULL, 1000000);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //start
    ret = csi_timer_start(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    uint32_t value;
    //get_current_value
    ret = csi_timer_get_current_value(NULL, &value);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_timer_get_current_value(timer, NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //get_status
    timer_status_t stat = csi_timer_get_status(NULL);
    TEST_CASE_ASSERT(stat.active == 0,"active != 0 act_val=%d",stat.active);

    //get_reload_value
    ret = csi_timer_get_load_value(NULL, &value);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_timer_get_load_value(timer, NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //stop
    ret = csi_timer_stop(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //suspend
    ret = csi_timer_suspend(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //resume
    ret = csi_timer_resume(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_TIMER_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    csi_timer_uninitialize(timer);

    return 0;
}
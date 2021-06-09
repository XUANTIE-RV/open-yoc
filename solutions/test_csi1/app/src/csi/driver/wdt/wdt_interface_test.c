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

#include "wdt_test.h"

void wdt_event_cb_func(int32_t idx, wdt_event_e event)
{
    if (event == WDT_EVENT_TIMEOUT) ;
}

int test_wdt_interface(char *args)
{
    //initialize
    wdt_handle_t wdt = csi_wdt_initialize(10, wdt_event_cb_func);
    TEST_CASE_ASSERT(wdt == NULL,"wdt != NULL act_val=%d",wdt);

    wdt = csi_wdt_initialize(0, NULL);
    TEST_CASE_ASSERT(wdt != NULL,"wdt == NULL act_val=%d",wdt);

    int32_t ret;
    //uninitialize
    ret = csi_wdt_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //set_timeout
    ret = csi_wdt_set_timeout(NULL, 100000);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //start
    ret = csi_wdt_start(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //stop
    ret = csi_wdt_stop(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //restart
    ret = csi_wdt_restart(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    uint32_t value;
    //read_current_value
    ret = csi_wdt_read_current_value(NULL, &value);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_wdt_read_current_value(wdt, NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //power control
    ret = csi_wdt_power_control(NULL, DRV_POWER_OFF);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_wdt_power_control(wdt, 10);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    csi_wdt_uninitialize(wdt);
    return 0;
}
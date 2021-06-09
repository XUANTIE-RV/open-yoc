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


static void wdt_event_cb_func(int32_t idx, wdt_event_e event)
{
//    if (event == WDT_EVENT_TIMEOUT) ;

    TEST_CASE_TIPS("enter wdt back call");
}


int test_wdt_reset(char *args)
{

    int32_t ret;
	uint32_t get_data[10];
    test_wdt_args_t td;
    wdt_handle_t wdt;
    uint8_t i;

	/*args get*/
    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");
    td.dev_idx = (uint8_t)get_data[0];
    td.timeout_ms = (uint32_t)get_data[1];
    TEST_CASE_ASSERT_QUIT(td.timeout_ms >= 30, "wdt is short");

    TEST_CASE_TIPS("test WDT idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config timeout is %d ms", td.timeout_ms);

    /*wdt init*/
    wdt = csi_wdt_initialize(td.dev_idx, wdt_event_cb_func);
    TEST_CASE_ASSERT(wdt != NULL,"wdt init failed");

    /*wdt set timeout*/
    ret = csi_wdt_set_timeout(wdt, td.timeout_ms);
    TEST_CASE_ASSERT(ret == 0,"wdt set timeout failed");

    /*wdt start*/
    ret = csi_wdt_start(wdt);
    TEST_CASE_ASSERT(ret == 0,"wdt start failed");

    
    TEST_CASE_TIPS("next wdt donot reset,clean wdt time- - - - - - - -", td.timeout_ms);
    for(i=0;i<2;i++)
    {
        tst_mdelay(td.timeout_ms - 30);  
        ret = csi_wdt_restart(wdt); 
        TEST_CASE_ASSERT(ret == 0,"wdt restart failed");
    }
    TEST_CASE_TIPS("test ok- - - - - -  - - - - - - - - - - - - - - - -\r\n");

    TEST_CASE_TIPS("next wdt donot reset,stop wdt - - - - - - - -- - - ", td.timeout_ms);
    ret = csi_wdt_stop(wdt);
    if(ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_UNSUPPORTED))
    {
        tst_mdelay(td.timeout_ms + 30);
    }
    TEST_CASE_TIPS("test ok- - - - - -  - - - - - - - - - - - - - - - -\r\n");
   

    TEST_CASE_TIPS("next wdt will be reset,start wdt - - - - - - - - - - ", td.timeout_ms);
    ret = csi_wdt_start(wdt);
    if(ret != (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_UNSUPPORTED))
    {
        tst_mdelay(td.timeout_ms*2 + 30);
    }
    TEST_CASE_TIPS("test err- - - - - -  - - - - - - - - - - - - - - - - \r\n");

    /*uinit wdt*/
    ret = csi_wdt_stop(wdt);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_WDT_BASE | DRV_ERROR_UNSUPPORTED),"wdt stop failed");
    ret = csi_wdt_uninitialize(wdt);
    TEST_CASE_ASSERT(ret == 0,"wdt uinit failed");

    return 0;
}
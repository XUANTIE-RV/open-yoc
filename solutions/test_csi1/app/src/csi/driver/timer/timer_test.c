/*
 * Copyright (C) 2020 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#include <timer_test.h>


int test_timer_main(char *args)
{
    char *case_name[] = {
        "TIMER_INTERFACE",
        "TIMER_PRECISION",
        "TIMER_STATUS",
    };


    int (*case_func[])(char *args) = {
        test_timer_interface,
        test_timer_precision,
        test_timer_status,
    };


    uint8_t i = 0;

    for (i=0; i<sizeof(case_name)/sizeof(char *); i++) {
        if (!strcmp((void *)_mc_name, case_name[i])) {
            case_func[i](args);
            return 0;
        }
    }

    TEST_CASE_TIPS("modedlur don't supported this command");
    return -1;   
    
}

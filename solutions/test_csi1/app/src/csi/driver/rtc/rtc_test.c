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

#include <rtc_test.h>


test_func_info_t rtc_test_funcs_map[] = {
	{"RTC_INTERFACE",(test_func)test_rtc_interface, 0},
	{"RTC_ALARM",(test_func)test_rtc_alarm_set, 8},
    {"RTC_TIME",(test_func)test_rtc_time,8},
};


int test_rtc_main(char *args)
{
	uint8_t i;
	void *args_value = NULL;
	int ret;



	for(i=0;i<sizeof(rtc_test_funcs_map)/sizeof(test_func_info_t);i++){
		if(!strcmp((void *)_mc_name, rtc_test_funcs_map[i].name)){
            args_value = malloc((sizeof(uint32_t)*rtc_test_funcs_map[i].args_num) + 4);//
            if (args_value == NULL){
                TEST_CASE_WARN_QUIT("malloc space failed, unparsed parameter");
            }
            ret = args_parsing(args, (uint32_t *)args_value, rtc_test_funcs_map[i].args_num);
            if (ret != 0){
                free(args_value);
                TEST_CASE_WARN_QUIT("parameter resolution error");
            }
            (*(rtc_test_funcs_map[i].function))(args_value);
            free(args_value);
            return 0;
		}
	}

	TEST_CASE_TIPS("module don't support this command.");
	return -1;
}

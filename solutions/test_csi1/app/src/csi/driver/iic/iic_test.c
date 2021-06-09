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

#include "iic_test.h"


test_func_info_t iic_test_funcs_map[] = {
	{"IIC_INTERFACE",(test_func)test_iic_interface, 0},
	{"IIC_MEMORY_TRANSFER",(test_func)test_iic_memoryTransfer, 7},
    {"IIC_MASTER_SEND",(test_func)test_iic_masterSend,6},
    {"IIC_SLAVE_RECEIVE",(test_func)test_iic_slaveReceive,6},
    {"IIC_SLAVE_SEND",(test_func)test_iic_slaveSend,6},
    {"IIC_MASTER_RECEIVE",(test_func)test_iic_masterReceive,6},
};


int test_iic_main(char *args)
{
	uint8_t i;
	void *args_value = NULL;
	int ret;

	for(i=0;i<sizeof(iic_test_funcs_map)/sizeof(test_func_info_t);i++){
		if(!strcmp((void *)_mc_name, iic_test_funcs_map[i].name)){
            args_value = malloc((sizeof(uint32_t)*iic_test_funcs_map[i].args_num) + 4);//
            if (args_value == NULL){
                TEST_CASE_WARN_QUIT("malloc space failed, unparsed parameter");
            }
            ret = args_parsing(args, (uint32_t *)args_value, iic_test_funcs_map[i].args_num);
            if (ret != 0){
                free(args_value);
                TEST_CASE_WARN_QUIT("parameter resolution error");
            }
            (*(iic_test_funcs_map[i].function))(args_value);
            free(args_value);
            return 0;
		}
	}

	TEST_CASE_TIPS("module don't support this command.");
	return -1;
}
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

#include "iic_test.h"


int test_iic_interface(char *args)
{
    //initialize
    int32_t ret = 0;
    iic_handle_t iic = NULL;
    uint8_t data[10];

	iic = csi_iic_initialize(10, NULL);
	TEST_CASE_ASSERT(iic == 0, "csi_iic_initialize interface test fail");

	iic = csi_iic_initialize(0, NULL);
	TEST_CASE_ASSERT(iic != 0, "csi_iic_initialize interface test fail");    
	csi_iic_uninitialize(iic);
    
	ret = csi_iic_config_mode(NULL, IIC_MODE_MASTER);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_config_mode interface test fail");

	ret = csi_iic_config_addr_mode(NULL, IIC_ADDRESS_7BIT);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_config_addr_mode interface test fail");
	
	ret = csi_iic_config_slave_addr(NULL, 0x20);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_config_slave_addr interface test fail");

	ret = csi_iic_config_speed(NULL, IIC_BUS_SPEED_STANDARD);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_config_speed interface test fail");

    ret = csi_iic_slave_send(NULL,"abcd", 4);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_slave_send interface test fail");

    ret = csi_iic_slave_receive(NULL, data, 10);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_slave_receive interface test fail");

    ret = csi_iic_master_send(NULL, 0x20, "abcd", 4, true);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_master_send interface test fail");
    
    ret = csi_iic_master_receive(NULL, 0x20, data, 4, true);
	TEST_CASE_ASSERT(ret != 0, "csi_iic_master_send interface test fail");
    return 0;
}
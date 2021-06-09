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

static volatile uint8_t transfer_done_flag;
static void iic_callback(int32_t idx, iic_event_e event)
{
    switch (event) {
        case IIC_EVENT_TRANSFER_DONE:
            transfer_done_flag = 0;
            break;

        default :
            break;
    }
}

int test_iic_slaveSend(char *args)
{

    test_iic_args_t td;
    iic_handle_t iic = NULL;
	int32_t ret = 0;

	td.dev_idx = (uint8_t)*((uint32_t *)args);
	td.addr_mode = *((uint32_t *)args+1);
	td.speed = (uint32_t)*((uint32_t *)args+2);
	td.own_addr = (uint16_t)*((uint32_t *)args+3);
	td.trans_size = (uint32_t)*((uint32_t *)args+4);
	td.iic_mode = (uint32_t)*((uint32_t *)args+5);

    TEST_CASE_TIPS("config iic's mode is %d", IIC_MODE_SLAVE);

	TEST_CASE_TIPS("test iic's idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config iic's address mode is %d", td.addr_mode);
    TEST_CASE_TIPS("config iic's speed is %d", td.speed);
    TEST_CASE_TIPS("config iic's own address is %d", td.own_addr);
    TEST_CASE_TIPS("receive size is %d", td.trans_size);
	TEST_CASE_TIPS("iic mode is %d", td.iic_mode);
	
	iic = csi_iic_initialize(td.dev_idx, iic_callback);
    TEST_CASE_ASSERT(iic != NULL, "csi_iic_initialize interface test fail");

	ret = csi_iic_power_control(iic, DRV_POWER_FULL);	
	if(ret != (CSI_DRV_ERRNO_IIC_BASE | DRV_ERROR_UNSUPPORTED))	
	{
		TEST_CASE_ASSERT(ret == 0, "csi_iic_power_control interface test fail");
	}

	ret = csi_iic_config_mode(iic, td.iic_mode);
	TEST_CASE_ASSERT(ret == 0, "csi_iic_config_mode interface test fail");

	ret = csi_iic_config_speed(iic, td.speed);
	TEST_CASE_ASSERT(ret == 0, "csi_iic_config_speed interface test fail");

	ret = csi_iic_config_slave_addr(iic, td.own_addr);
	TEST_CASE_ASSERT(ret == 0, "csi_iic_config_slave_addr interface test fail");

	char *send_data = NULL, *receive_data = NULL;
    send_data = (char *)malloc(td.trans_size);
	receive_data = (char *)malloc(td.trans_size);
    if ((send_data == NULL) || (receive_data == NULL)) {
		free(send_data);
		free(receive_data);
        csi_iic_uninitialize(iic);
        TEST_CASE_WARN_QUIT("transfer size too long, malloc error");
    }
	transfer_data(send_data, td.trans_size);

	TEST_CASE_READY();
    transfer_done_flag = 1;
	ret = csi_iic_slave_send(iic, send_data, td.trans_size);
	TEST_CASE_ASSERT(ret == 0, "csi_iic_slave_receive fail");
	while (transfer_done_flag);

	/*uninit iic*/
	free(send_data);
	free(receive_data);	
	ret = csi_iic_power_control(iic, DRV_POWER_OFF);
	if(ret != (CSI_DRV_ERRNO_IIC_BASE | DRV_ERROR_UNSUPPORTED))	
	{
		TEST_CASE_ASSERT(ret == 0, "csi_iic_power_control interface test fail");
	}
    csi_iic_uninitialize(iic);	
	return 0;
}

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

#include "dma_test.h"


static volatile uint8_t tran_flag;

static void dma_callback(int32_t ch, dma_event_e event, void *arg)
{
	if (event == DMA_EVENT_TRANSFER_DONE) {
		tran_flag = 0;
	}
}

int test_dma_interface(char *args)
{
	int32_t ret,channel;

	channel = csi_dma_alloc_channel();	
	TEST_CASE_TIPS("channel = %d",channel);

	ret = csi_dma_config_channel(channel, NULL, dma_callback, NULL);
	TEST_CASE_ASSERT(ret != 0, "csi_dma_config_channel interface test fail");

	ret = csi_dma_power_control(100,DRV_POWER_FULL);
	if(ret != (CSI_DRV_ERRNO_DMA_BASE | DRV_ERROR_UNSUPPORTED))
	{
		TEST_CASE_ASSERT(ret != 0, "csi_dma_config_channel interface test fail");
	}
	
	ret = csi_dma_power_control(channel,DRV_POWER_FULL);
	if(ret != (CSI_DRV_ERRNO_DMA_BASE | DRV_ERROR_UNSUPPORTED))
	{
		TEST_CASE_ASSERT(ret == 0, "csi_dma_config_channel interface test fail");
	}

	csi_dma_release_channel(channel);
	return 0;
}
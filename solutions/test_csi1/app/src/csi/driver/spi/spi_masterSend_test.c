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

#include "spi_test.h"

static volatile uint8_t spi_cb_tx_flag=1;
static volatile uint8_t spi_cb_rx_flag=1;
static void spi_event_cb_fun(int32_t idx,spi_event_e event)
{
    if(event == SPI_EVENT_TX_COMPLETE)
    {
        spi_cb_tx_flag=0;

    }
    else if (event == SPI_EVENT_RX_COMPLETE)
    {
        spi_cb_rx_flag=0;

    }
    else if(event == SPI_EVENT_TRANSFER_COMPLETE) 
	{
        spi_cb_rx_flag=0;
        spi_cb_tx_flag=0;
    }
}

int test_spi_masterSend(char *args)
{

    test_spi_args_t td;
    spi_handle_t spi = NULL;
	int32_t ret = 0;


	td.idx = (uint8_t)*((uint32_t *)args);
	td.frame_len = *((uint32_t *)args+1);
	td.cp_format = (uint32_t)*((uint32_t *)args+2);
	td.baud = (uint32_t)*((uint32_t *)args+3);
	td.bit_order = (uint32_t)*((uint32_t *)args+4);


    TEST_CASE_TIPS("config spi's mode MASTER");

    TEST_CASE_TIPS("spi idx:%d", td.idx);
    TEST_CASE_TIPS("spi frame_len:%d", td.frame_len);
    TEST_CASE_TIPS("spi cp_format:%d", td.cp_format);
    TEST_CASE_TIPS("spi baud:%d", td.baud);
    TEST_CASE_TIPS("spi bit_order:%d", td.bit_order);

	/*spi init*/	
    spi = csi_spi_initialize(td.idx, (spi_event_cb_t)spi_event_cb_fun);
	TEST_CASE_ASSERT(spi != NULL, "csi_spi_initialize interface test fail");

	/*spi config*/	
    ret = csi_spi_config(spi, td.baud, SPI_MODE_MASTER, td.cp_format, td.bit_order, SPI_SS_MASTER_HW_OUTPUT, td.frame_len);
	TEST_CASE_ASSERT(ret == 0, "csi_spi_config interface test fail");

	/*spi malloc buffer*/	
	char *send_data = NULL;
    send_data = (char *)malloc(SPI_TRANSFER_LEN);
	
    if (send_data == NULL) {
		free(send_data);
        csi_spi_uninitialize(spi);
        TEST_CASE_WARN_QUIT("transfer size too long, malloc error");
    }
	transfer_data(send_data, SPI_TRANSFER_LEN);

	/*spi send data*/
	TEST_CASE_READY();
	spi_cb_tx_flag = 1;
	ret = csi_spi_send(spi, send_data, SPI_TRANSFER_LEN);
	
	TEST_CASE_ASSERT(ret == NULL, "csi_spi_send interface test fail");
	while (spi_cb_tx_flag);

	/*uninit spi*/
	free(send_data);
    ret = csi_spi_uninitialize(spi);	
	TEST_CASE_ASSERT(ret == 0, "csi_spi_uninitialize interface test fail");

	return 0;	
}

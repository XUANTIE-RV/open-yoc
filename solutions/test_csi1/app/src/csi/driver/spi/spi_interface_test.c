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
    	TEST_CASE_TIPS("spi enter cb");

    }
    else if (event == SPI_EVENT_RX_COMPLETE)
    {
        spi_cb_rx_flag=0;
    	TEST_CASE_TIPS("spi enter cb");

    }
    else if(event == SPI_EVENT_TRANSFER_COMPLETE) 
	{
        spi_cb_rx_flag=0;
        spi_cb_tx_flag=0;
    	TEST_CASE_TIPS("spi enter cb");
    }
}

int test_spi_interface(char *args)
{
    //get_capabilities
    spi_capabilities_t cap = csi_spi_get_capabilities(5);
	TEST_CASE_ASSERT(cap.simplex == 0, "csi_spi_get_capabilities interface test fail");

    //initialize
    spi_handle_t spi = csi_spi_initialize(5, spi_event_cb_fun);
	TEST_CASE_ASSERT(spi == NULL, "csi_spi_initialize interface test fail");

    spi = csi_spi_initialize(0, NULL);
	TEST_CASE_ASSERT(spi != NULL, "csi_spi_initialize interface test fail");

    int32_t ret;
    //power_control
    ret = csi_spi_power_control(NULL, DRV_POWER_FULL);
	if(ret != (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_UNSUPPORTED))
	{
		ret = csi_spi_power_control(NULL, DRV_POWER_FULL);		
		TEST_CASE_ASSERT(ret != 0, "csi_spi_power_control interface test fail");				
	}

    //uninitialize
    ret = csi_spi_uninitialize(NULL);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_uninitialize interface test fail");

    //config_mode
    ret = csi_spi_config_mode(NULL, SPI_MODE_MASTER);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_mode interface test fail");

    ret = csi_spi_config_mode(spi, 5);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_MODE), "csi_spi_config_mode interface test fail");

    ret = csi_spi_config_mode(spi, -1);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_MODE), "csi_spi_config_mode interface test fail");

    //config_block_mode
    ret = csi_spi_config_block_mode(NULL, 0);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_block_mode interface test fail");

    ret = csi_spi_config_block_mode(spi, -1);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_block_mode interface test fail");

    //config_baudrate
    ret = csi_spi_config_baudrate(NULL, 115200);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_baudrate interface test fail");

    //config_bit_order
    ret = csi_spi_config_bit_order(NULL, SPI_ORDER_MSB2LSB);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_bit_order interface test fail");

    ret = csi_spi_config_bit_order(spi, 2);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_BIT_ORDER), "csi_spi_config_bit_order interface test fail");


    ret = csi_spi_config_bit_order(spi, -1);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_BIT_ORDER), "csi_spi_config_bit_order interface test fail");

    //config_datawidth
    ret = csi_spi_config_datawidth(NULL, 32);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_datawidth interface test fail");

    ret = csi_spi_config_datawidth(spi, 0);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_DATA_BITS), "csi_spi_config_datawidth interface test fail");

    ret = csi_spi_config_datawidth(spi, 33);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_DATA_BITS), "csi_spi_config_datawidth interface test fail");

    ret = csi_spi_config_datawidth(spi, -1);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_DATA_BITS), "csi_spi_config_datawidth interface test fail");

    //config_format
    ret = csi_spi_config_format(NULL, SPI_FORMAT_CPOL0_CPHA0);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_format interface test fail");

    ret = csi_spi_config_format(spi, 4);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_FRAME_FORMAT), "csi_spi_config_format interface test fail");

    ret = csi_spi_config_format(spi, -1);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_FRAME_FORMAT), "csi_spi_config_format interface test fail");

    //config_ss_mode
    ret = csi_spi_config_ss_mode(NULL, SPI_SS_MASTER_UNUSED);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_config_ss_mode interface test fail");

    ret = csi_spi_config_ss_mode(spi, 6);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_SS_MODE), "csi_spi_config_ss_mode interface test fail");

    ret = csi_spi_config_ss_mode(spi, -1);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | SPI_ERROR_SS_MODE), "csi_spi_config_ss_mode interface test fail");

    uint8_t data[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    //send
    ret = csi_spi_send(NULL, data, 8);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_send interface test fail");

    ret = csi_spi_send(spi, NULL, 8);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_send interface test fail");

    ret = csi_spi_send(spi, data, 0);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_send interface test fail");

    ret = csi_spi_send(spi, data, 8);

    uint8_t recv[8];
    //receive
    ret = csi_spi_receive(NULL, recv, 8);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_receive interface test fail");

    ret = csi_spi_receive(spi, NULL, 8);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_receive interface test fail");

    ret = csi_spi_receive(spi, recv, 0);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_receive interface test fail");

    //ss_control
	if(ret != (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_UNSUPPORTED))
	{
		ret = csi_spi_ss_control(NULL, SPI_SS_ACTIVE);
		TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_SPI_BASE | DRV_ERROR_PARAMETER), "csi_spi_ss_control interface test fail");	
	}

    ret = csi_spi_uninitialize(spi);
	TEST_CASE_ASSERT(ret == 0, "csi_spi_uninitialize interface test fail");

	return 0;
}
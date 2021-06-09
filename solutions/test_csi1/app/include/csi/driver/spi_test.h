/*
* Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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

#ifndef _SPI_TEST_H_
#define _SPI_TEST_H_
#include <stdint.h>
#include <drv/spi.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include "test_common.h"
#define SPI_TRANSFER_LEN 20

//Test arguments from AT test commands
typedef struct{
    uint32_t idx;                    // device idx
    spi_mode_e spi_mode;         // master / slave
    uint32_t frame_len;   // data frame length
    spi_format_e cp_format;   // clock Polarity / Phase
    uint32_t baud;                   // spi work frequency
    spi_bit_order_e bit_order; 
} test_spi_args_t;


extern int test_spi_interface(char *args);
extern int test_spi_masterSend(char *args);
extern int test_spi_slaveReceive(char *args);
extern int test_spi_slaveSend(char *args);
extern int test_spi_masterReceive(char *args);
extern uint16_t spi_transfer_data[SPI_TRANSFER_LEN+1];

#endif

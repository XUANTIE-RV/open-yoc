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

#ifndef __IIC_TEST__
#define __IIC_TEST__

#include <stdint.h>
#include <drv/iic.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
    uint8_t     dev_idx;
	uint8_t		addr_mode;
	uint8_t		speed;
	uint32_t	own_addr;
	uint32_t	trans_size;
	uint32_t	slave_addr;
	uint16_t	mem_addr;
	uint8_t		mem_addr_size;
	uint32_t	timeout;
	uint8_t     iic_mode;
}test_iic_args_t;


extern int test_iic_interface(char *args);
extern int test_iic_memoryTransfer(char *args);
extern int test_iic_masterSend(char *args);
extern int test_iic_slaveReceive(char *args);
extern int test_iic_slaveSend(char *args);
extern int test_iic_masterReceive(char *args);

#endif

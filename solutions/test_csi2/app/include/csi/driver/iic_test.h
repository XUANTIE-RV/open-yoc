/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __IIC_TEST__
#define __IIC_TEST__

#include <stdint.h>
#include <drv/iic.h>
#include <drv/dma.h>
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
}test_iic_args_t;


extern int test_iic_interface(char *args);
extern int test_iic_masterAsyncSend(char *args);
extern int test_iic_masterSyncSend(char *args);
extern int test_iic_masterDmaSend(char *args);
extern int test_iic_slaveAsyncSend(char *args);
extern int test_iic_slaveSyncSend(char *args);
extern int test_iic_slaveDmaSend(char *args);
extern int test_iic_masterAsyncReceive(char *args);
extern int test_iic_masterSyncReceive(char *args);
extern int test_iic_masterDmaReceive(char *args);
extern int test_iic_slaveAsyncReceive(char *args);
extern int test_iic_slaveSyncReceive(char *args);
extern int test_iic_slaveDmaReceive(char *args);
extern int test_iic_memoryTransfer(char *args);
extern int test_iic_getStateMasterSend(char *args);
extern int test_iic_getStateSlaveReceive(char *args);
extern int test_iic_main(char *args);

#endif

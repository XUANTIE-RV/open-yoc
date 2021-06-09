/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __SPIFLASH_TEST_H_
#define __SPIFLASH_TEST_H_

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

#include <drv/spiflash.h>

typedef struct{
    uint32_t idx;
    uint32_t offset;
    uint32_t size;
} test_spiflash_args_t;

extern int test_spiflash_interface(char *args);
extern int test_spiflash_program_read(char *args);
extern int test_spiflash_qspi_programread(char *args);
extern int test_spiflash_lock(char *args);
extern int test_spiflash_performance_read(char *args);
extern int test_spiflash_main(char *args); 
#endif
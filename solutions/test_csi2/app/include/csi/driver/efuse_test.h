/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __EFUSE_TEST__
#define __EFUSE_TEST__
 
#include <stdint.h>
#include <drv/efuse.h>
#include <drv/tick.h>
#include <soc.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
	uint32_t	idx;
    uint32_t    addr;
    uint32_t    size;
}test_efuse_args_t;

extern int test_efuse_interface(char *args);
extern int test_efuse_readprogram(char *args);
extern int test_efuse_main(char *args);

#endif
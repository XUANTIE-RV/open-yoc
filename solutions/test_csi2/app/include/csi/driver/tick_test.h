/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __TICK_TEST__
#define __TICK_TEST__

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>
#include <drv/tick.h>


typedef struct {
    uint32_t    idx;
}test_tick_args_t;


extern int test_tick_func(char *args);
extern int test_tick_main(char *args);

#endif

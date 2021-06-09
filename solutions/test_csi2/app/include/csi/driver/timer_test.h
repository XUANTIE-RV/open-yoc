/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __TIMER_TEST__
#define __TIMER_TEST__
 
#include <stdint.h>
#include <drv/timer.h>
#include <drv/tick.h>
#include <soc.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include "test_common.h"

typedef struct {
	uint8_t		timer_idx;
    uint32_t    timeout;
}test_timer_args_t;


extern int test_timer_interface(char *args);
extern int test_timer_precision(char *args);
extern int test_timer_state(char *args);
extern int test_timer_setting(char *args);
extern int test_timer_getLoadvalue(char *args);
extern int test_timer_getRemainingvalue(char *args);
extern int test_timer_callback(char *args);
extern int test_timer_main(char *args);

#endif

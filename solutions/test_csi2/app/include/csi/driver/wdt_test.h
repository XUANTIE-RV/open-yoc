/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __WDT_TEST__
#define __WDT_TEST__

#include <stdint.h>
#include <drv/wdt.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
	uint8_t		dev_idx;
	uint32_t	timeout_ms;
	uint32_t	wait_timeout_ms;
}test_wdt_args_t;



extern int test_wdt_interface(char *args);
extern int test_wdt_queryFeed(char *args);
extern int test_wdt_interruptFeed(char *args);
extern int test_wdt_timeoutReset(char *args);
extern int test_wdt_startStop(char *args);
extern int test_wdt_getInfo(char *args);
extern int test_wdt_main(char *args);
#endif

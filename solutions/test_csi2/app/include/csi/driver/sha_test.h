/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __SHA_TEST__
#define __SHA_TEST__

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

#include <drv/sha.h>

#define MAX_SHA_TEST_IN_BUFFER_LEN 2048
#define MAX_SHA_TEST_SOURCE_LEN  (MAX_SHA_TEST_IN_BUFFER_LEN * 2)
#define MAX_SHA_TEST_OUTPUT_BUFFER_LEN 128

extern int test_set_sha_mode(char *args);
extern int test_sha_sync(char *args);
extern int test_sha_async(char *args);
extern int test_sha_sync_performance(char *args);
extern int test_sha_async_performance(char *args);
extern int test_sha_sync_stability(char *args);
extern int test_sha_async_stability(char *args);
extern int test_sha_main(char *args);
#endif

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#ifndef __TRNG_TEST__
#define __TRNG_TEST__

#include <stdint.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>
#include <drv/rng.h>

#define TRNG_DATA_LEN	2048

typedef struct {
    uint32_t    idx;
}test_trng_args_t;

extern int test_trng_main(char *args);
extern int test_trng_handle(char *args);
extern int test_trng_multi(char *args);
extern int test_trng_single(char *args);
extern int test_trng_multi_performance(char *args);
extern int test_trng_single_performance(char *args);
extern int test_trng_interface(char *args);
extern int test_trng_multi_stability(char *args);
extern int test_trng_single_stability(char *args);
#endif

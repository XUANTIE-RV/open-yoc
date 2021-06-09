/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __MBOX_TEST__
#define __MBOX_TEST__

#include <stdint.h>
#include <drv/mbox.h>
#include <drv/tick.h>
#include <soc.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

#define MBOX_TEST_SIZE 308
#define RX_SIZE 64

typedef struct {
    uint32_t	idx;
    uint32_t    channel_id;
    uint32_t    trans_size;
} test_mbox_args_t;

extern int test_mbox_interface(char *args);
extern int test_mbox_transferSend(char *args);
extern int test_mbox_transferReceive(char *args);
extern int test_mbox_main(char *args);
#endif
/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#ifndef INC_RAMBUS_H
#define INC_RAMBUS_H

// Driver Framework Basic Defs API
#include "basic_defs.h"

// Driver Framework Device API
#include "device_types.h"
#include <drv/common.h>
#include "rambus_errcode.h"
#include "rambus_log.h"
#include "dmares_types.h"
#include "device_rw.h"

uint32_t rb_get_random_byte(uint8_t *buf, uint32_t count);

/* 0 if NONE */
void rb_perf_init(uint32_t data_len_in_bits, uint32_t type);
void rb_perf_start();
void rb_perf_end();
void rb_perf_get(char *ncase);

#define RB_PERF_START_POINT()                                                  \
        do {                                                                   \
                rb_perf_start();                                               \
        } while (0)

#define RB_PERF_END_POINT()                                                    \
        do {                                                                   \
                rb_perf_end();                                                 \
        } while (0)

#endif
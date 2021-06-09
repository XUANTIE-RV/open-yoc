/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef KM_PORT_H
#define KM_PORT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

uint32_t get_data_from_addr(uint32_t addr, uint8_t *data, uint32_t data_len);

uint32_t get_word_from_addr(uint32_t addr);

#endif

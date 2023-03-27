/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _ETH_INTERNAL_H
#define _ETH_INTERNAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int eth_init(void);

int32_t eth_get_macaddr(uint8_t *mac);

int32_t eth_set_macaddr(const uint8_t *mac);

#ifdef __cplusplus
}
#endif

#endif
/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DRIVERS_DW_GMAC_H_
#define _DRIVERS_DW_GMAC_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>

#include <hal/ethernet_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void eth_dw_gmac_register(eth_config_t *eth_config);

#ifdef __cplusplus
}
#endif

#endif

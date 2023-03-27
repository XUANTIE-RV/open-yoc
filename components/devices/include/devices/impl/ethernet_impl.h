/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DRIVERS_ETHDRV_IMPL_H_
#define _DRIVERS_ETHDRV_IMPL_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct eth_driver {
    /*common*/
    int (*mac_control)(rvm_dev_t *dev, eth_config_t *config);
    int (*set_packet_filter)(rvm_dev_t *dev, int type);
    int (*start)(rvm_dev_t *dev);
    int (*stop)(rvm_dev_t *dev);
    int (*reset)(rvm_dev_t *dev);
} eth_driver_t;

#ifdef __cplusplus
}
#endif

#endif

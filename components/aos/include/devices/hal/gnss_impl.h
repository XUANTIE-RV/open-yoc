/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_GNSS_IMPL_H
#define HAL_GNSS_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/gnss.h>

typedef struct {
    driver_t drv;
    int (*get_info)(aos_dev_t *dev, gnss_info_t *info);

} gnss_driver_t;

#ifdef __cplusplus
}
#endif

#endif

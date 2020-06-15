/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock_impl.h
 * @brief    user sock implement header
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#ifndef HAL_USRSOCK_IMPL_H
#define HAL_USRSOCK_IMPL_H

#include <stdint.h>
#include <unistd.h>

#include <devices/driver.h>
#include <devices/usrsock.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct usrsock_driver {
    driver_t drv;
    ssize_t (*write)(aos_dev_t *dev, const void *data, size_t size);
    ssize_t (*read)(aos_dev_t *dev, void *data, size_t size);
    void (*set_event)(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv), void *priv);
} usrsock_driver_t;

#ifdef __cplusplus
}
#endif

#endif

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_HCI_IMPL_H
#define HAL_HCI_IMPL_H

#include <devices/hci.h>

typedef struct hci_driver {
    driver_t drv;
    int (*set_event)(aos_dev_t *dev, hci_event_cb event, void *priv);
    int (*start)(aos_dev_t *dev);
    int (*send)(aos_dev_t *dev, uint8_t *data, uint32_t size);
    int (*recv)(aos_dev_t *dev, uint8_t *data, uint32_t size);
    void* (*get_vendor_interface)(aos_dev_t *dev);
} hci_driver_t;

#endif
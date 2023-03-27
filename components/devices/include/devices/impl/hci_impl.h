/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_HCI_IMPL_H
#define HAL_HCI_IMPL_H

#include <devices/hci.h>

typedef struct hci_driver {
    driver_t drv;
    int (*set_event)(rvm_dev_t *dev, hci_event_cb_t event, void *priv);
    int (*start)(rvm_dev_t *dev, hci_driver_send_cmd_t send_cmd);
    int (*send)(rvm_dev_t *dev, uint8_t *data, uint32_t size);
    int (*recv)(rvm_dev_t *dev, uint8_t *data, uint32_t size);
} hci_driver_t;

#endif
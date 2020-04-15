/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICES_HCI_H_
#define DEVICES_HCI_H_
#include <stdint.h>

#include <devices/driver.h>

typedef enum {
  HCI_EVENT_READ,
} hci_event_t;

#define hci_open(name) device_open(name)
#define hci_open_id(name, id) device_open_id(name, id)
#define hci_close(dev) device_close(dev)

typedef void (*hci_event_cb)(hci_event_t event, uint32_t size, void *priv);

int hci_set_event(aos_dev_t *dev, hci_event_cb event, void *priv);

/**
  \brief       send hci format data
  \param[in]   dev      Pointer to device object.
  \param[out]  data     data address to store data read.
  \param[in]   size     data length expected to read.
  \return      0 on success, else on fail.
*/
int hci_send(aos_dev_t *dev, void *data, uint32_t size);

int hci_recv(aos_dev_t *dev, void* data, uint32_t size);

int hci_start(aos_dev_t *dev);

void* hci_get_vendor_interface(aos_dev_t *dev);
#endif

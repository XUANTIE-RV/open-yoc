/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICES_HCI_H_
#define DEVICES_HCI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <devices/device.h>

typedef enum {
    HCI_EVENT_READ,
} rvm_hal_hci_event_t;

#define rvm_hal_hci_open(name) rvm_hal_device_open(name)
#define rvm_hal_hci_close(dev) rvm_hal_device_close(dev)

typedef void (*hci_event_cb_t)(rvm_hal_hci_event_t event, uint32_t size, void *priv);

typedef int (*hci_driver_send_cmd_t)(uint16_t opcode, uint8_t* send_data, uint32_t send_len, uint8_t* resp_data, uint32_t *resp_len);

/**
  \brief       set hci event
  \param[in]   dev      Pointer to device object.
  \param[in]   event    data read event callback.
  \param[in]   priv     event callback userdata arg.
  \return      0 on success, else on fail.
*/
int rvm_hal_hci_set_event(rvm_dev_t *dev, hci_event_cb_t event, void *priv);

/**
  \brief       send hci format data
  \param[in]   dev      Pointer to device object.
  \param[in]   data     data address to send.
  \param[in]   size     data length expected to read.
  \return      send data len.
*/
int rvm_hal_hci_send(rvm_dev_t *dev, void *data, uint32_t size);

/**
  \brief       recv hci format data
  \param[in]   dev      Pointer to device object.
  \param[in]   data     data address to read.
  \param[in]   size     data length expected to read.
  \return      read data len.
*/
int rvm_hal_hci_recv(rvm_dev_t *dev, void* data, uint32_t size);

/**
  \brief       start hci driver
  \param[in]   dev      Pointer to device object.
  \param[out]  send_cmd sned hci command callback.
  \return      0 on success, else on fail.
*/
int rvm_hal_hci_start(rvm_dev_t *dev, hci_driver_send_cmd_t send_cmd);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_hci.h>
#endif

#ifdef __cplusplus
}
#endif

#endif

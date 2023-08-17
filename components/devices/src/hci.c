/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <dev_internal.h>
#include <devices/impl/hci_impl.h>
#include <devices/hci.h>

int rvm_hal_hci_send(rvm_dev_t *dev, void *data, uint32_t size)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    device_lock(dev);
    ret = hci->send(dev, data, size);
    device_unlock(dev);

    return ret;
}

int rvm_hal_hci_set_event(rvm_dev_t *dev, hci_event_cb_t event, void *pirv)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    device_lock(dev);
    ret = hci->set_event(dev, event, pirv);
    device_unlock(dev);

    return ret;
}

int rvm_hal_hci_recv(rvm_dev_t *dev, void* data, uint32_t size)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    device_lock(dev);
    ret = hci->recv(dev, data, size);
    device_unlock(dev);

    return ret;
}

int rvm_hal_hci_start(rvm_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    // device_lock(dev);
    ret = hci->start(dev, send_cmd);
    // device_unlock(dev);

    return ret;
}

/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/can_impl.h>

#define CAN_DRIVER(dev)  ((can_driver_t*)(dev->drv))
#define CAN_VAILD(dev) do { \
    if (device_valid(dev, "can") != 0) \
        return -1; \
} while(0)

int rvm_hal_can_config(rvm_dev_t *dev, rvm_hal_can_config_t *config)
{
    int ret;

    CAN_VAILD(dev);

    device_lock(dev);
    ret = CAN_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_can_config_get(rvm_dev_t *dev, rvm_hal_can_config_t *config)
{
    int ret;

    CAN_VAILD(dev);

    device_lock(dev);
    ret = CAN_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_can_send(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout)
{
    int ret;

    CAN_VAILD(dev);

    device_lock(dev);
    ret = CAN_DRIVER(dev)->send(dev, msg, timeout);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_can_recv(rvm_dev_t *dev, rvm_hal_can_msg_t *msg, uint32_t timeout)
{
    int ret;

    CAN_VAILD(dev);

    device_lock(dev);
    ret = CAN_DRIVER(dev)->recv(dev, msg, timeout);
    device_unlock(dev);

    return ret;  
}

int rvm_hal_can_set_event(rvm_dev_t *dev, rvm_hal_can_callback callback, void *arg)
{
    int ret;

    CAN_VAILD(dev);

    device_lock(dev);
    ret = CAN_DRIVER(dev)->set_event(dev, callback, arg);
    device_unlock(dev);

    return ret; 
}

int rvm_hal_can_filter_init(rvm_dev_t *dev, rvm_hal_can_filter_config_t *filter_config)
{
    int ret;
	
    CAN_VAILD(dev);
	
    device_lock(dev);
    ret = CAN_DRIVER(dev)->filter_init(dev, filter_config);
    device_unlock(dev);
	
    return ret; 
}





/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/can.h>
#include <string.h>

static aos_status_t _devfs_can_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret = 0;
    rvm_dev_t *dev = devfs_file2dev(file);
    if (!dev) {
        return -EINVAL;
    }
    switch (cmd)
    {
        case CAN_IOC_SET_CONFIG:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_hal_can_config_t config;
            memcpy(&config, (void *)arg, sizeof(rvm_hal_can_config_t));
            rvm_hal_can_config(dev, &config);
            break;
        }
        case CAN_IOC_GET_CONFIG:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_hal_can_config_t config;
            rvm_hal_can_config_get(dev, &config);
            memcpy((void *)arg, &config, sizeof(rvm_hal_can_config_t));
            break;
        }  
        case CAN_IOC_SET_FILTER_INIT:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_hal_can_filter_config_t filter_config;
            memcpy(&filter_config, (void *)arg, sizeof(rvm_hal_can_filter_config_t));
            rvm_hal_can_filter_init(dev, &filter_config);
            break;
        }
        case CAN_IOC_SEND:
        {
            if (!(void *)arg){
                ret = -EFAULT;
                break;
            }
            if (!devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
           rvm_can_dev_tx_rx_msg_t msg;
           memcpy(&msg, (const void *)arg, sizeof(rvm_can_dev_tx_rx_msg_t));
           rvm_hal_can_send(dev, &msg.can_msg, msg.timeout);
           break;
        }        
        case CAN_IOC_RECV:
        {
            if (!(void *)arg){
                ret = -EFAULT;
                break;
            }
            if (!devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            }
            rvm_can_dev_tx_rx_msg_t msg;
            rvm_hal_can_recv(dev, &msg.can_msg, msg.timeout);
            memcpy((void*)arg, &msg, sizeof(rvm_can_dev_tx_rx_msg_t));
            break;
        }         
        case CAN_IOC_SET_EVENT:
        {
            if (!(void *)arg){
                ret = -EFAULT;
                break;
            }
            rvm_can_dev_set_event_msg_t msg;
            memcpy(&msg, (const void *)arg, sizeof(rvm_can_dev_set_event_msg_t));
            rvm_hal_can_set_event(dev, msg.callback, msg.arg);
            break;
        } 
        default:
            ret = -EINVAL;
            break;
    }
    return ret;
}

static const devfs_file_ops_t devfs_can_ops = {
    .ioctl      = _devfs_can_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t can_ops_node = {
    .name = "can",
    .ops = (devfs_file_ops_t *)&devfs_can_ops,
};

void can_devfs_init(void)
{
    devices_add_devfs_ops_node(&can_ops_node);
}
VFS_DEV_DRIVER_ENTRY(can_devfs_init);
#endif
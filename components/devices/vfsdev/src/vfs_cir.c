/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/cir.h>
#include <string.h>

static aos_status_t _devfs_cir_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret = 0;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    switch (cmd) {
        case CIR_IOC_SET_CONFIG:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_hal_cir_config_t config;
            memcpy(&config, (void *)arg, sizeof(rvm_hal_cir_config_t));
            rvm_hal_cir_config(dev, &config);
            break;
        }
        case CIR_IOC_GET_CONFIG:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_hal_cir_config_t config;
            rvm_hal_cir_config_get(dev, &config);
            memcpy((void *)arg, &config, sizeof(rvm_hal_cir_config_t));
            break;
        }
        case CIR_IOC_SEND_SCANCODE:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            if (!devfs_file_is_writable(file)) {
                ret = -EPERM;
                break;
            }
            uint32_t scancode;
            memcpy(&scancode, (void *)arg, sizeof(uint32_t));
            rvm_hal_cir_send(dev, scancode);
            break;
        }
        case CIR_IOC_RECV_START:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_hal_cir_recv_start(dev);
            break;
        }
        case CIR_IOC_SET_RX_EVENT:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            }
            rvm_cir_dev_rx_event_msg_t msg;
            memcpy(&msg, (void *)arg, sizeof(rvm_cir_dev_rx_event_msg_t));
            rvm_hal_cir_set_rx_event(dev, msg.callback, msg.arg);
            break;
        }
        case CIR_IOC_GET_RECV_SCANCODE:
        {
            if (!(void *)arg) {
                ret = -EFAULT;
                break;
            } 
            if (!devfs_file_is_readable(file)) {
                ret = -EPERM;
                break;
            } 
            uint32_t scancode;
            rvm_hal_cir_get_recv_scancode(dev, &scancode);
            memcpy((void *)arg, &scancode, sizeof(uint32_t));
            break;
        }
        default:
            ret = -EINVAL;
            break;
    }
    return ret;
}

static const devfs_file_ops_t devfs_cir_ops = {
    .ioctl      = _devfs_cir_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t cir_ops_node = {
    .name = "cir",
    .ops = (devfs_file_ops_t *)&devfs_cir_ops,
};

void cir_devfs_init(void)
{
    devices_add_devfs_ops_node(&cir_ops_node);
}

VFS_DEV_DRIVER_ENTRY(cir_devfs_init);

#endif
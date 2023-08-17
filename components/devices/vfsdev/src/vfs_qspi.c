/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/qspi.h>

static aos_status_t _devfs_qspi_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case QSPI_IOC_SET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_qspi_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_qspi_config_t));
        if (rvm_hal_qspi_config(dev, &config)) {
            return -EIO;
            break;
        }
        break;
    }
    case QSPI_IOC_GET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_qspi_config_t config;
        if (rvm_hal_qspi_config_get(dev, &config)) {
            return -EIO;
            break;
        }
        memcpy((void *)arg, &config, sizeof(rvm_hal_qspi_config_t));
        break;
    }
    case QSPI_IOC_SEND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_qspi_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_qspi_dev_msg_t));
        if (rvm_hal_qspi_send(dev, &msg.cmd, msg.data, msg.size, msg.timeout)) {
            ret = -EIO;
        }
        break;
    }
    case QSPI_IOC_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_qspi_dev_msg_t msg;
        if (rvm_hal_qspi_recv(dev, &msg.cmd, msg.data, msg.size, msg.timeout)) {
            ret = -EIO;
            break;
        }
        memcpy((void *)arg, &msg, sizeof(rvm_qspi_dev_msg_t));
        break;
    }
    case QSPI_IOC_SEND_AND_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_qspi_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_qspi_dev_msg_t));
        if (rvm_hal_qspi_send_recv(dev, &msg.cmd, msg.tx_data, msg.rx_data, msg.size, msg.timeout)) {
            ret = -EIO;
            break;
        }
        memcpy((void *)arg, &msg, sizeof(rvm_qspi_dev_msg_t));
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_qspi_ops = {
    .ioctl      = _devfs_qspi_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t qspi_ops_node = {
    .name = "qspi",
    .ops = (devfs_file_ops_t *)&devfs_qspi_ops,
};

void qspi_devfs_init(void)
{
    devices_add_devfs_ops_node(&qspi_ops_node);
}

VFS_DEV_DRIVER_ENTRY(qspi_devfs_init);

#endif

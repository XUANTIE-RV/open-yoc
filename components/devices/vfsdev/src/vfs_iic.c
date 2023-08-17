/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/iic.h>
#include <string.h>

static aos_status_t _devfs_iic_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case IIC_IOC_GET_DEFAULT_CONFIG:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_iic_config_t config;
        rvm_hal_iic_config_default(&config);
        memcpy((void *)arg, &config, sizeof(rvm_hal_iic_config_t));
        break;
    }
    case IIC_IOC_SET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_iic_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_iic_config_t));
        if (rvm_hal_iic_config(dev, &config)) {
            return -EIO;
        }
        break;
    }
    case IIC_IOC_DMA_ENABLE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        bool enable;
        memcpy(&enable, (void *)arg, sizeof(bool));
        if (rvm_hal_iic_trans_dma_enable(dev, enable)) {
            return -EIO;
        }
        break;
    }
    case IIC_IOC_MASTER_SEND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_iic_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_iic_dev_msg_t));
        if (rvm_hal_iic_master_send(dev, msg.dev_addr, msg.data, msg.size, msg.timeout) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case IIC_IOC_MASTER_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_iic_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_iic_dev_msg_t));
        if (rvm_hal_iic_master_recv(dev, msg.dev_addr, msg.data, msg.size, msg.timeout) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case IIC_IOC_SLAVE_SEND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_iic_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_iic_dev_msg_t));
        if (rvm_hal_iic_slave_send(dev, msg.data, msg.size, msg.timeout) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case IIC_IOC_SLAVE_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_iic_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_iic_dev_msg_t));
        if (rvm_hal_iic_slave_recv(dev, msg.data, msg.size, msg.timeout) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case IIC_IOC_MEM_WRITE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_iic_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_iic_dev_msg_t));
        if (rvm_hal_iic_mem_write(dev, msg.dev_addr, msg.mem_addr, msg.mem_addr_size, msg.data, msg.size, msg.timeout) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case IIC_IOC_MEM_READ:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_iic_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_iic_dev_msg_t));
        if (rvm_hal_iic_mem_read(dev, msg.dev_addr, msg.mem_addr, msg.mem_addr_size, msg.data, msg.size, msg.timeout) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_iic_ops = {
    .ioctl      = _devfs_iic_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t iic_ops_node = {
    .name = "iic",
    .ops = (devfs_file_ops_t *)&devfs_iic_ops,
};

void iic_devfs_init(void)
{
    devices_add_devfs_ops_node(&iic_ops_node);
}
VFS_DEV_DRIVER_ENTRY(iic_devfs_init);

#endif
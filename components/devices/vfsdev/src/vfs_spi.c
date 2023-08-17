/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/spi.h>
#include <string.h>

static aos_status_t _devfs_spi_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case SPI_IOC_GET_DEFAULT_CONFIG:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_spi_config_t config;
        rvm_hal_spi_default_config_get(&config);
        memcpy((void *)arg, &config, sizeof(rvm_hal_spi_config_t));
        break;
    }
    case SPI_IOC_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_spi_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_spi_config_t));
        if (rvm_hal_spi_config(dev, &config)) {
            return -EIO;
        }
        break;
    }
    case SPI_IOC_CONFIG_GET:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_spi_config_t config;
        if (rvm_hal_spi_config_get(dev, &config)) {
            return -EIO;
        }
        memcpy((void *)arg, &config, sizeof(rvm_hal_spi_config_t));
        break;
    }
    case SPI_IOC_SEND_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_spi_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_spi_dev_msg_t));
        if (rvm_hal_spi_send_recv(dev, msg.tx_data, msg.rx_data, msg.size, msg.timeout)) {
            return -EIO;
        }
        break;
    }    
    case SPI_IOC_SEND_THEN_RECV:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_spi_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_spi_dev_msg_t));
        if (rvm_hal_spi_send_then_recv(dev, msg.tx_data, msg.tx_size, msg.rx_data,
                                       msg.rx_size, msg.timeout)) {
            return -EIO;
        }
        break;
    }
    case SPI_IOC_SEND_THEN_SEND:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_spi_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_spi_dev_msg_t));
        if (rvm_hal_spi_send_then_send(dev, msg.tx_data, msg.tx_size, msg.tx1_data,
                                       msg.tx1_size, msg.timeout)) {
            return -EIO;
        }
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

ssize_t _devfs_spi_write(devfs_file_t *file, const void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    mode_t mode = devfs_file_get_mode(file);

    if (!devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_spi_send(dev, buf, count, (mode & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER) < 0) {
        return -1;
    }
    return 0;
}

ssize_t _devfs_spi_read(devfs_file_t *file, void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    mode_t mode = devfs_file_get_mode(file);

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_spi_recv(dev, buf, count, (mode & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER) < 0) {
        return -1;
    }
    return 0;
}

static const devfs_file_ops_t devfs_spi_ops = {
    .ioctl      = _devfs_spi_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = _devfs_spi_read,
    .write      = _devfs_spi_write,
    .lseek      = NULL,
};

static devfs_ops_node_t spi_ops_node = {
    .name = "spi",
    .ops = (devfs_file_ops_t *)&devfs_spi_ops,
};

void spi_devfs_init(void)
{
    devices_add_devfs_ops_node(&spi_ops_node);
}
VFS_DEV_DRIVER_ENTRY(spi_devfs_init);

#endif

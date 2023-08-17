/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/i2s.h>

static aos_status_t _devfs_i2s_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case I2S_IOC_SET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_i2s_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_i2s_config_t));
        if (rvm_hal_i2s_config(dev, &config) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case I2S_IOC_GET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_i2s_config_t config;
        if (rvm_hal_i2s_config_get(dev, &config) < 0) {
            ret = -EIO;
            break;
        }
        memcpy((void *)arg, &config, sizeof(rvm_hal_i2s_config_t));
        break;
    }
    case I2S_IOC_PAUSE:
    {
        if (rvm_hal_i2s_pause(dev) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case I2S_IOC_RESUME:
    {
        if (rvm_hal_i2s_resume(dev) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case I2S_IOC_STOP:
    {
        if (rvm_hal_i2s_stop(dev) < 0) {
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

ssize_t _devfs_i2s_write(devfs_file_t *file, const void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    mode_t mode = devfs_file_get_mode(file);

    if (!devfs_file_is_writable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_i2s_send(dev, buf, count, (mode & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER) < 0)
        return -1;
    return count;
}

ssize_t _devfs_i2s_read(devfs_file_t *file, void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    mode_t mode = devfs_file_get_mode(file);

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    if (rvm_hal_i2s_recv(dev, buf, count, (mode & O_NONBLOCK) ? AOS_NO_WAIT : AOS_WAIT_FOREVER) < 0)
        return -1;
    return count;
}




static const devfs_file_ops_t devfs_i2s_ops = {
    .ioctl      = _devfs_i2s_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = _devfs_i2s_read,
    .write      = _devfs_i2s_write,
    .lseek      = NULL,
};

static devfs_ops_node_t i2s_ops_node = {
    .name = "iis",
    .ops = (devfs_file_ops_t *)&devfs_i2s_ops,
};

void i2s_devfs_init(void)
{
    devices_add_devfs_ops_node(&i2s_ops_node);
}
VFS_DEV_DRIVER_ENTRY(i2s_devfs_init);

#endif

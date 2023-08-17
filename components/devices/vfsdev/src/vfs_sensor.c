/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/sensor.h>
#include <string.h>

static aos_status_t _devfs_sensor_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case SENSOR_IOC_FETCH:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }

        if (rvm_hal_sensor_fetch(dev)) {
            return -EIO;
        }
        break;
    }
    case SENSOR_IOC_GETVALUE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_sensor_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_sensor_dev_msg_t));
        if (rvm_hal_sensor_getvalue(dev, msg.value, msg.size)) {
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

static const devfs_file_ops_t devfs_sensor_ops = {
    .ioctl      = _devfs_sensor_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t sensor_ops_node = {
    .name = "sensor",
    .ops = (devfs_file_ops_t *)&devfs_sensor_ops,
};

void sensor_devfs_init(void)
{
    devices_add_devfs_ops_node(&sensor_ops_node);
}
VFS_DEV_DRIVER_ENTRY(sensor_devfs_init);

#endif
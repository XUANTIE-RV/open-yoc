/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/timer.h>
#include <string.h>

static aos_status_t _devfs_timer_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case TIMER_IOC_START:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t timeout_us;
        memcpy(&timeout_us, (void *)arg, sizeof(uint32_t));
        if (rvm_hal_timer_start(dev, timeout_us)) {
            return -EIO;
        }
        break;
    }
    case TIMER_IOC_STOP:
    {
        if (rvm_hal_timer_stop(dev)) {
            return -EIO;
        }
        break;
    }
    case TIMER_IOC_ATTACH_CALLBACK:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_timer_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_timer_dev_msg_t));
        if (rvm_hal_timer_attach_callback(dev, msg.callback, msg.arg)) {
            return -EIO;
        }
        break;
    }
    case TIMER_IOC_DETACH_CALLBACK:
    {
        if (rvm_hal_timer_detach_callback(dev)) {
            return -EIO;
        }
        break;
    }
    case TIMER_IOC_GET_REMAINING_VALUE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t remaining_value;
        remaining_value = rvm_hal_timer_get_remaining_value(dev);
        memcpy((void *)arg, &remaining_value, sizeof(uint32_t));
        break;
    }
    case TIMER_IOC_GET_LOAD_VALUE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t load_value;
        load_value = rvm_hal_timer_get_load_value(dev);
        memcpy((void *)arg, &load_value, sizeof(uint32_t));
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_timer_ops = {
    .ioctl      = _devfs_timer_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t timer_ops_node = {
    .name = "timer",
    .ops = (devfs_file_ops_t *)&devfs_timer_ops,
};

void timer_devfs_init(void)
{
    devices_add_devfs_ops_node(&timer_ops_node);
}
VFS_DEV_DRIVER_ENTRY(timer_devfs_init);

#endif

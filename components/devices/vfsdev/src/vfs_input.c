/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/input.h>
#include <string.h>

static unsigned int read_timeout_ms;

static aos_status_t _devfs_input_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case INPUT_IOC_SET_EVENT:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_input_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_input_dev_msg_t));
        if (rvm_hal_input_set_event(dev, msg.event_cb, msg.priv)) {
            return -EIO;
        }
        break;
    }
    // TODO：rvm_hal_input_unset_event未实现
    // case INPUT_IOC_UNSET_EVENT:
    // {
    //     if (!(const void *)arg) {
    //         ret = -EFAULT;
    //         break;
    //     }
    //     rvm_input_dev_msg_t msg;
    //     memcpy(&msg, (void *)arg, sizeof(rvm_input_dev_msg_t));
    //     if (rvm_hal_input_unset_event(dev, msg.event_cb)) {
    //         return -EIO;
    //     }
    //     break;
    // }
    case INPUT_IOC_READ_TIMEOUT_MS:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        memcpy(&read_timeout_ms, (void *)arg, sizeof(unsigned int));
        break;
    }    
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

ssize_t _devfs_input_read(devfs_file_t *file, void *buf, size_t count)
{
    rvm_dev_t *dev = devfs_file2dev(file);
    // mode_t mode = devfs_file_get_mode(file);

    if (!devfs_file_is_readable(file))
        return -EPERM;

    if (!buf || count == 0)
        return -EFAULT;

    count = rvm_hal_input_read(dev, buf, count, read_timeout_ms);

    return count;
}

static const devfs_file_ops_t devfs_input_ops = {
    .ioctl      = _devfs_input_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = _devfs_input_read,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t input_ops_node = {
    .name = "input",
    .ops = (devfs_file_ops_t *)&devfs_input_ops,
};

void input_devfs_init(void)
{
    devices_add_devfs_ops_node(&input_ops_node);
}
VFS_DEV_DRIVER_ENTRY(input_devfs_init);

#endif
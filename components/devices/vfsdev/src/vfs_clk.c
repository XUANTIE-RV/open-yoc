/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/clk.h>
#include <string.h>

static aos_status_t _devfs_clk_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case CLK_IOC_ENABLE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        char *dev_name = (char *)arg;
        if (rvm_hal_clk_enable(dev, dev_name)) {
            return -EIO;
        }
        break;
    }
    case CLK_IOC_DISABLE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        char *dev_name = (char *)arg;
        if (rvm_hal_clk_disable(dev, dev_name)) {
            return -EIO;
        }
        break;
    }
    case CLK_IOC_GET_FREQ:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_clk_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_clk_dev_msg_t));
        if (rvm_hal_clk_get_freq(dev, msg.clk_id, msg.idx, &msg.freq) < 0) {
            ret = -EIO;
            break;
        }
        memcpy((void *)arg, &msg, sizeof(rvm_clk_dev_msg_t));
        break;
    }
    case CLK_IOC_SET_FREQ:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_clk_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_clk_dev_msg_t));
        if (rvm_hal_clk_set_freq(dev, msg.clk_id, msg.idx, msg.freq) < 0) {
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

static const devfs_file_ops_t devfs_clk_ops = {
    .ioctl      = _devfs_clk_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t clk_ops_node = {
    .name = "clk",
    .ops = (devfs_file_ops_t *)&devfs_clk_ops,
};

void clk_devfs_init(void)
{
    devices_add_devfs_ops_node(&clk_ops_node);
}
VFS_DEV_DRIVER_ENTRY(clk_devfs_init);

#endif
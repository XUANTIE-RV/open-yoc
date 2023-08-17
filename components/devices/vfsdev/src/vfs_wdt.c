/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/wdt.h>
#include <string.h>

static aos_status_t _devfs_wdt_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case WDT_IOC_SET_TIMEOUT:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t ms;
        memcpy(&ms, (void *)arg, sizeof(uint32_t));
        rvm_hal_wdt_set_timeout(dev, ms);
        break;
    }
    case WDT_IOC_START:
    {
        if (rvm_hal_wdt_start(dev)) {
            return -EIO;
        }
        break;
    }
    case WDT_IOC_STOP:
    {
        if (rvm_hal_wdt_stop(dev)) {
            return -EIO;
        }
        break;
    }
    case WDT_IOC_FEED:
    {
        if (rvm_hal_wdt_feed(dev)) {
            return -EIO;
        }
        break;
    }
    case WDT_IOC_GET_REMAINING_TIME:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t remaining_time;
        remaining_time = rvm_hal_wdt_get_remaining_time(dev);
        memcpy((void *)arg, &remaining_time, sizeof(uint32_t));
        break;
    }
    case WDT_IOC_ATTACH_CALLBACK:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_wdt_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_wdt_dev_msg_t));
        rvm_hal_wdt_attach_callback(dev, msg.callback, msg.arg);
        break;
    }
    case WDT_IOC_DETACH_CALLBACK:
    {
        if (rvm_hal_wdt_detach_callback(dev)) {
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

static const devfs_file_ops_t devfs_wdt_ops = {
    .ioctl      = _devfs_wdt_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t wdt_ops_node = {
    .name = "wdt",
    .ops = (devfs_file_ops_t *)&devfs_wdt_ops,
};

void wdt_devfs_init(void)
{
    devices_add_devfs_ops_node(&wdt_ops_node);
}
VFS_DEV_DRIVER_ENTRY(wdt_devfs_init);

#endif
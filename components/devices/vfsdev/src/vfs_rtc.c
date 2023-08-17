/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/rtc.h>
#include <string.h>

static aos_status_t _devfs_rtc_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case RTC_IOC_SET_TIME:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        struct tm time;
        memcpy(&time, (void *)arg, sizeof(struct tm));
        rvm_hal_rtc_set_time(dev, &time);
        break;
    }
    case RTC_IOC_GET_TIME:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }

        struct tm time;
        if (rvm_hal_rtc_get_time(dev, &time)) {
            return -EIO;
        }
        memcpy((void *)arg, &time, sizeof(struct tm));
        break;
    }
    case RTC_IOC_GET_ALARM_REMAINING_TIME:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint32_t remaining_time;
        remaining_time = rvm_hal_rtc_get_alarm_remaining_time(dev);
        memcpy((void *)arg, &remaining_time, sizeof(uint32_t));
        break;
    }
    case RTC_IOC_SET_ALARM:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_rtc_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_rtc_dev_msg_t));
        rvm_hal_rtc_set_alarm(dev, msg.time, msg.callback, msg.arg);
        break;
    }
    case RTC_IOC_CANCEL_ALARM:
    {
        if (rvm_hal_rtc_cancel_alarm(dev)) {
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

static const devfs_file_ops_t devfs_rtc_ops = {
    .ioctl      = _devfs_rtc_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t rtc_ops_node = {
    .name = "rtc",
    .ops = (devfs_file_ops_t *)&devfs_rtc_ops,
};

void rtc_devfs_init(void)
{
    devices_add_devfs_ops_node(&rtc_ops_node);
}
VFS_DEV_DRIVER_ENTRY(rtc_devfs_init);

#endif
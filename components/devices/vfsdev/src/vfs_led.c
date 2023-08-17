/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/led.h>
#include <string.h>

static aos_status_t _devfs_led_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case LED_IOC_CONTROL:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_led_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_led_dev_msg_t));
        rvm_hal_led_control(dev, msg.color, msg.on_time, msg.off_time);
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_led_ops = {
    .ioctl      = _devfs_led_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t led_ops_node = {
    .name = "led",
    .ops = (devfs_file_ops_t *)&devfs_led_ops,
};

void led_devfs_init(void)
{
    devices_add_devfs_ops_node(&led_ops_node);
}
VFS_DEV_DRIVER_ENTRY(led_devfs_init);

#endif
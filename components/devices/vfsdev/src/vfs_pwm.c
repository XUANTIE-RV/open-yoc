/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/pwm.h>
#include <string.h>

static aos_status_t _devfs_pwm_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case PWM_IOC_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_pwm_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_pwm_dev_msg_t));
        if (rvm_hal_pwm_config(dev, &msg.config, msg.channel)) {
            return -EIO;
        }
        break;
    }
    case PWM_IOC_CONFIG_GET:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_pwm_dev_msg_t msg;
        memcpy(&msg, (void *)arg, sizeof(rvm_pwm_dev_msg_t));
        if (rvm_hal_pwm_config_get(dev, &msg.config, msg.channel)) {
            return -EIO;
        }
        memcpy((void *)arg, &msg, sizeof(rvm_pwm_dev_msg_t));
        break;
    }
    case PWM_IOC_START:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint8_t channel;
        memcpy(&channel, (void *)arg, sizeof(uint8_t));
        if (rvm_hal_pwm_start(dev, channel)) {
            return -EIO;
        }
        break;
    }
    case PWM_IOC_STOP:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        uint8_t channel;
        memcpy(&channel, (void *)arg, sizeof(uint8_t));
        if (rvm_hal_pwm_stop(dev, channel)) {
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

static const devfs_file_ops_t devfs_pwm_ops = {
    .ioctl      = _devfs_pwm_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t pwm_ops_node = {
    .name = "pwm",
    .ops = (devfs_file_ops_t *)&devfs_pwm_ops,
};

void pwm_devfs_init(void)
{
    devices_add_devfs_ops_node(&pwm_ops_node);
}
VFS_DEV_DRIVER_ENTRY(pwm_devfs_init);


#endif

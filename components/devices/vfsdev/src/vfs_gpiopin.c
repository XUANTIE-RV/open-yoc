/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/gpiopin.h>
#include <string.h>

static aos_status_t _devfs_gpiopin_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case GPIOPIN_IOC_SET_DIRECTION:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_gpio_dir_t dir;
        memcpy(&dir, (void *)arg, sizeof(rvm_hal_gpio_dir_t));
        rvm_hal_gpio_pin_set_direction(dev, dir);
        break;
    }
    case GPIOPIN_IOC_SET_MODE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_gpio_mode_t mode;
        memcpy(&mode, (void *)arg, sizeof(rvm_hal_gpio_mode_t));
        rvm_hal_gpio_pin_set_mode(dev, mode);
        break;
    }
    case GPIOPIN_IOC_ATTACH_CALLBACK:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_gpiopin_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gpiopin_dev_msg_t));
        rvm_hal_gpio_pin_attach_callback(dev, msg.callback, msg.arg);
        break;
    }
    case GPIOPIN_IOC_SET_IRQ_MODE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_gpio_mode_t mode;
        memcpy(&mode, (void *)arg, sizeof(rvm_hal_gpio_mode_t));
        rvm_hal_gpio_pin_set_irq_mode(dev, mode);
        break;
    }
    case GPIOPIN_IOC_SET_IRQ_ENABLE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        bool enable;
        memcpy(&enable, (void *)arg, sizeof(bool));
        rvm_hal_gpio_pin_irq_enable(dev, enable);
        break;
    }
    case GPIOPIN_IOC_SET_DEBOUNCE:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        bool enable;
        memcpy(&enable, (void *)arg, sizeof(bool));
        rvm_hal_gpio_pin_set_debounce(dev, enable);
        break;
    }
    case GPIOPIN_IOC_WRITE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_writable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_hal_gpio_pin_data_t value;
        memcpy(&value, (const void *)arg, sizeof(rvm_hal_gpio_pin_data_t));
        if (rvm_hal_gpio_pin_write(dev, value) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case GPIOPIN_IOC_READ:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }

        rvm_gpiopin_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_gpiopin_dev_msg_t));
        if (rvm_hal_gpio_pin_read(dev, msg.value) < 0) {
            ret = -EIO;
            break;
        }
        break;
    }
    case GPIOPIN_IOC_OUTPUT_TOGGLE:
    {
        if (rvm_hal_gpio_pin_output_toggle(dev)) {
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

static const devfs_file_ops_t devfs_gpiopin_ops = {
    .ioctl      = _devfs_gpiopin_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t gpiopin_ops_node = {
    .name = "gpio_pin",
    .ops = (devfs_file_ops_t *)&devfs_gpiopin_ops,
};

void gpiopin_devfs_init(void)
{
    devices_add_devfs_ops_node(&gpiopin_ops_node);
}
VFS_DEV_DRIVER_ENTRY(gpiopin_devfs_init);

#endif